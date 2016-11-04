/* 
 * Copyright [2016] [Subhabrata Ghosh] 
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 *       limitations under the License. 
 * 
 */
//
// Created by Subhabrata Ghosh on 25/10/16.
//

#include <core/includes/fs_block.h>
#include <core/includes/common_structs.h>
#include "core/includes/fs_block.h"
#include "common/includes/file_utils.h"
#include "common/includes/timer.h"

string com::wookler::reactfs::core::fs_block::open(uint64_t block_id, string filename) {
    CHECK_NOT_EMPTY(filename);
    try {

        void *base_ptr = _open(block_id, filename);

        PRECONDITION(header->record_type == __block_record_type::RAW);

        data_ptr = get_data_ptr(base_ptr);
        POSTCONDITION(NOT_NULL(data_ptr));

        BYTE_PTR wptr = static_cast<BYTE_PTR>(data_ptr);
        if (header->write_offset > 0) {
            write_ptr = wptr + header->write_offset;
        } else {
            write_ptr = wptr;
        }
        if (header->compression.compressed) {
            void *ptr = data_ptr;
            data_ptr = nullptr;
            read_compressed_block(ptr);
        }
        return header->block_uid;
    } catch (const exception &e) {
        throw FS_BASE_ERROR_E(e);
    } catch (...) {
        throw FS_BASE_ERROR("Unkown error occurred while opening block.");
    }
}

void *com::wookler::reactfs::core::fs_block::_open(uint64_t block_id, string filename) {
    Path p(filename);
    if (!p.exists()) {
        throw FS_BASE_ERROR("File not found. [path=%s]", filename.c_str());
    }
    stream = new fmstream(filename.c_str());
    CHECK_NOT_NULL(stream);
    void *base_ptr = stream->data();

    header = static_cast<__block_header *>(base_ptr);
    PRECONDITION(header->block_id == block_id);

    if (IS_NULL(block_lock)) {
        string lock_name = get_lock_name(header->block_id);
        block_lock = new exclusive_lock(&lock_name);
        block_lock->create();
    }
    this->filename = string(filename);

    return base_ptr;
}

string
com::wookler::reactfs::core::fs_block::create(uint64_t block_id, string filename, __block_type block_type,
                                              uint64_t block_size, bool compressed, __compression_type compression_type,
                                              uint64_t raw_size,
                                              bool overwrite) {
    CHECK_NOT_EMPTY(filename);
    PRECONDITION(block_size > 0);
    PRECONDITION(block_id > 0);
    try {
        string uuid = _create(block_id, filename, block_type, __block_record_type::RAW, block_size, overwrite);
        if (compressed) {
            header->writable = false;
            header->compression.compressed = true;
            header->compression.uncompressed_size = raw_size;
            header->compression.type = compression_type;
        }
        CHECK_AND_FREE (block_lock);

        close();

        return uuid;
    } catch (const exception &e) {
        throw FS_BASE_ERROR_E(e);
    } catch (...) {
        throw FS_BASE_ERROR("Unkown error occurred while opening block.");
    }
}

string com::wookler::reactfs::core::fs_block::_create(uint64_t block_id, string filename, __block_type block_type,
                                                      __block_record_type record_type, uint64_t block_size,
                                                      bool overwrite) {
    Path p(filename);
    if (p.exists()) {
        if (!overwrite) {
            throw FS_BASE_ERROR("File with specified path already exists. [path=%s]", filename.c_str());
        } else {
            if (!p.remove()) {
                throw FS_BASE_ERROR("Error deleting existing file. [file=%s]", filename.c_str());
            }
        }
    }
    uint64_t ts = sizeof(__block_header) + block_size;

    stream = new fmstream(filename.c_str(), ts);
    CHECK_NOT_NULL(stream);
    void *base_ptr = stream->data();

    memset(base_ptr, 0, ts);

    header = static_cast<__block_header *>(base_ptr);
    header->block_id = block_id;

    string uuid = common_utils::uuid();
    memcpy(header->block_uid, uuid.c_str(), uuid.length());

    header->block_size = block_size;
    header->create_time = time_utils::now();
    header->update_time = header->create_time;
    header->block_type = block_type;
    header->record_type = record_type;
    header->writable = true;
    header->compression.compressed = false;
    header->compression.uncompressed_size = 0;
    header->compression.type = __compression_type::NONE;
    header->archival = __archival::DISABLED;

    string lock_name = get_lock_name(header->block_id);
    exclusive_lock *block_lock = new exclusive_lock(&lock_name);
    block_lock->create();
    block_lock->reset();
    CHECK_AND_FREE(block_lock);

    return uuid;
}

const void *com::wookler::reactfs::core::fs_block::read(uint64_t size, uint64_t offset) {
    CHECK_NOT_NULL(stream);
    CHECK_NOT_NULL(header);

    uint64_t data_size = header->block_size;
    if (header->compression.compressed) {
        data_size = header->compression.uncompressed_size;
    }
    if ((offset + size) > data_size) {
        return nullptr;
    }

    BYTE_PTR ptr = static_cast<BYTE_PTR>(data_ptr);
    if (offset > 0)
        ptr += offset;

    return ptr;
}

const void *com::wookler::reactfs::core::fs_block::_write(const void *source, uint32_t len) {
    CHECK_NOT_NULL(stream);
    CHECK_NOT_NULL(header);
    CHECK_NOT_NULL(source);
    PRECONDITION(len > 0);
    PRECONDITION(header->used_bytes + len <= header->block_size);

    BYTE_PTR start_ptr = static_cast<BYTE_PTR>(write_ptr);
    if (block_lock->wait_lock()) {
        try {
            write_r(source, len);
        } catch (const exception &e) {
            block_lock->release_lock();
            throw FS_BASE_ERROR_E(e);
        } catch (...) {
            block_lock->release_lock();
            throw FS_BASE_ERROR("Unhandled type exception.");
        }
        block_lock->release_lock();

        return start_ptr;
    } else {
        throw FS_BASE_ERROR("Error getting write lock on file. [block id=%s]", header->block_uid);
    }
}

void *com::wookler::reactfs::core::fs_block::write_r(const void *source, uint32_t len) {
    void *ptr = write_ptr;

    memcpy(write_ptr, source, len);

    header->used_bytes += len;
    header->write_offset += len;

    BYTE_PTR cptr = static_cast<BYTE_PTR>(data_ptr);
    write_ptr = cptr + header->write_offset;

    if (header->used_bytes >= header->block_size) {
        header->writable = false;
        write_ptr = nullptr;
    }
    return ptr;
}

void com::wookler::reactfs::core::fs_block::remove() {
    this->close();

    if (!IS_EMPTY(filename)) {
        LOG_WARN("Deleting block file. [file=%s]", filename.c_str());
        Path p(filename);
        if (!p.remove()) {
            LOG_ERROR("Error deleting filesystem block. [file=%s]", filename.c_str());
        }
    }
    if (NOT_NULL(block_lock)) {
        block_lock->dispose();
    }
}

void com::wookler::reactfs::core::fs_block::read_compressed_block(void *base_ptr) {
    data_ptr = malloc(header->compression.uncompressed_size * sizeof(char));
    CHECK_NOT_NULL(data_ptr);
    if (header->used_bytes > 0) {
        archive_reader reader;
        reader.read_archive_data(base_ptr, header->block_size, data_ptr, header->compression.uncompressed_size,
                                 header->compression.type);
    }
}