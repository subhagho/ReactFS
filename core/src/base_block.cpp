/*
 * Copyright [yyyy] [name of copyright owner]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
//
// Created by Subhabrata Ghosh on 24/11/16.
//

#include "core/includes/base_block.h"

using namespace com::wookler::reactfs::core;

string
com::wookler::reactfs::core::base_block::__create_block(uint64_t block_id, string filename, __block_type block_type,
                                                        __block_record_type record_type, uint64_t block_size,
                                                        uint64_t start_index, bool overwrite) {
    try {
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
        uint64_t b_size = (block_size * 110) / 100;

        uint64_t ts = sizeof(__block_header) + b_size;

        mm_data = new file_mapped_data(p.get_path(), ts, overwrite);
        CHECK_ALLOC(mm_data, TYPE_NAME(file_mapped_data));
        base_ptr = mm_data->get_base_ptr();

        header = static_cast<__block_header *>(base_ptr);
        header->block_id = block_id;

        string uuid = common_utils::uuid();
        memcpy(header->block_uid, uuid.c_str(), uuid.length());

        header->version.major = version.major;
        header->version.minor = version.minor;
        header->block_size = block_size;
        header->create_time = time_utils::now();
        header->update_time = header->create_time;
        header->block_type = block_type;
        header->record_type = record_type;
        header->write_state = __write_state::WRITABLE;
        header->compression.compressed = false;
        header->compression.type = __compression_type::NO_COMPRESSION;
        header->encryption.encrypted = false;
        header->encryption.type = __encryption_type::NO_ENCRYPTION;
        header->start_index = start_index;
        header->last_index = header->start_index;
        header->block_checksum = 0;
        header->commit_sequence = 0;

        string lock_name = get_lock_name(header->block_id);
        read_write_lock_client *env = shared_lock_utils::get();

        block_lock = env->add_lock(lock_name);
        CHECK_NOT_NULL(block_lock);
        block_lock->reset();

        this->filename = string(filename);

        mm_data->flush();

        state.set_state(__state_enum::Available);

        return uuid;
    } catch (const exception &e) {
        fs_error_base err = FS_BASE_ERROR("Error creating block. [block id=%lu][filename=%s][error=%s]", block_id,
                                          filename.c_str(), e.what());
        state.set_error(&err);
        throw err;
    } catch (...) {
        fs_error_base err = FS_BASE_ERROR("Error creating block. [block id=%lu][filename=%s][error=%s]", block_id,
                                          filename.c_str(), "UNKNOWN ERROR");
        state.set_error(&err);
        throw err;
    }
}

void *com::wookler::reactfs::core::base_block::__open_block(uint64_t block_id, string filename) {
    Path p(filename);
    if (!p.exists()) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_BLOCK_FILE_NOT_FOUND, "File not found. [path=%s]",
                             filename.c_str());
    }
    mm_data = new file_mapped_data(p.get_path());
    CHECK_ALLOC(mm_data, TYPE_NAME(file_mapped_data));
    base_ptr = mm_data->get_base_ptr();

    header = static_cast<__block_header *>(base_ptr);
    if (header->block_id != block_id) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_BLOCK_COPRRUPTED,
                             "Block id read from block file does not match. [read block id=%lu][expected block id=%lu]",
                             header->block_id, block_id);
    }
    if (!version_utils::compatible(header->version, version)) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_BLOCK_DATA_VERSION,
                             "Block data structure version does not match. [read varion=%s][expected version=%s]",
                             version_utils::get_version_string(header->version).c_str(),
                             version_utils::get_version_string(version).c_str());
    }

    if (IS_NULL(block_lock)) {
        string lock_name = get_lock_name(header->block_id);
        read_write_lock_client *env = shared_lock_utils::get();

        block_lock = env->add_lock(lock_name);
        CHECK_NOT_NULL(block_lock);
    }
    this->filename = string(p.get_path());

    if (header->compression.compressed) {
        compression = compression_factory::get_compression_handler(header->compression.type);
    }
    return base_ptr;
}

void com::wookler::reactfs::core::base_block::close() {
    CHECK_AND_DISPOSE(state);

    CHECK_AND_FREE(index_ptr);
    CHECK_AND_FREE(mm_data);

    if (NOT_NULL(block_lock)) {
        string thread_id = thread_utils::get_current_thread();
        if (block_lock->has_write_lock(thread_id)) {
            block_lock->release_write_lock();
        }
        if (block_lock->has_thread_lock(thread_id)) {
            block_lock->release_read_lock();
        }
        read_write_lock_client *env = shared_lock_utils::get();
        env->remove_lock(block_lock->get_name());

        block_lock = nullptr;
    }
    if (NOT_NULL(rollback_info)) {
        CHECK_AND_FREE(rollback_info->transaction_id);
        FREE_PTR(rollback_info);
    }
    rollback_info = nullptr;
    header = nullptr;
    write_ptr = nullptr;
    base_ptr = nullptr;
}

__record *
com::wookler::reactfs::core::base_block::__write_record(void *source, uint64_t size, string transaction_id,
                                                        uint64_t uncompressed_size) {
    CHECK_STATE_AVAILABLE(state);
    CHECK_NOT_NULL(source);

    PRECONDITION(is_writeable());
    PRECONDITION(has_space(size));
    PRECONDITION(in_transaction());
    PRECONDITION(!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id));

    void *w_ptr = get_write_ptr();


    __record *record = (__record *) malloc(sizeof(__record));
    CHECK_ALLOC(record, TYPE_NAME(__record));

    uint32_t checksum = common_utils::crc32c(0, (BYTE *) source, size);

    record->header = static_cast<__record_header *>(w_ptr);

    record->header->index = get_next_index();
    record->header->offset = get_write_offset();
    record->header->data_size = size;
    record->header->timestamp = time_utils::now();
    record->header->uncompressed_size = uncompressed_size;
    record->header->state = __record_state::R_DIRTY;
    record->header->checksum = checksum;

    w_ptr = common_utils::increment_data_ptr(w_ptr, sizeof(__record_header));
    record->data_ptr = w_ptr;

    memcpy(record->data_ptr, source, size);

    rollback_info->used_bytes += size;
    rollback_info->write_offset += (sizeof(__record_header) + size);
    rollback_info->block_checksum += record->header->checksum;

    return record;
}

__record *
com::wookler::reactfs::core::base_block::__read_record(uint64_t index, uint64_t offset, uint64_t size) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(index >= header->start_index && index <= header->last_index);

    void *ptr = get_data_ptr();
    void *rptr = common_utils::increment_data_ptr(ptr, offset);
    __record *record = (__record *) malloc(sizeof(__record));
    CHECK_ALLOC(record, TYPE_NAME(__record));
    record->header = static_cast<__record_header *>(rptr);
    rptr = common_utils::increment_data_ptr(rptr, sizeof(__record_header));
    record->data_ptr = rptr;

    POSTCONDITION(record->header->index == index);
    POSTCONDITION(record->header->offset == offset);
    POSTCONDITION(record->header->data_size == size);

    uint32_t checksum = common_utils::crc32c(0, (BYTE *) record->data_ptr, record->header->data_size);
    POSTCONDITION(checksum == record->header->checksum);

    return record;
}

string com::wookler::reactfs::core::base_block::start_transaction(uint64_t timeout) {
    CHECK_STATE_AVAILABLE(state);
    string txid = EMPTY_STRING;
    char *user_name = getenv("USER");
    if (IS_NULL(user_name)) {
        user_name = getenv("USERNAME");
    }
    PRECONDITION(NOT_NULL(user_name));
    string uname(user_name);

    txid = block_lock->write_lock(uname, timeout);
    if (!IS_EMPTY(txid)) {
        setup_transaction(txid);
    } else {
        throw FS_BASE_ERROR("Error getting wait lock. [lock=%s]", block_lock->get_name().c_str());
    }

    index_ptr->start_transaction(txid);

    return txid;
}

uint64_t com::wookler::reactfs::core::base_block::write(void *source, uint32_t length, string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    CHECK_NOT_NULL(source);

    temp_buffer buffer;
    bool use_buffer = false;

    uint64_t uncompressed_size = 0;

    if (is_compressed()) {
        use_buffer = true;
        CHECK_NOT_NULL(compression);

        uncompressed_size = length;
        compression->write_archive_data(source, length, &buffer);
    }
    if (is_encrypted()) {
        use_buffer = true;
        // TODO : Implement encryption handlers.
    }


    void *data_ptr = (use_buffer ? buffer.get_ptr() : source);
    uint64_t size = (use_buffer ? buffer.get_used_size() : length);

    __record *r_ptr = __write_record(data_ptr, size, transaction_id, uncompressed_size);

    index_ptr->write_index(r_ptr->header->index, r_ptr->header->offset, r_ptr->header->data_size, transaction_id);

    uint64_t index = r_ptr->header->index;
    CHECK_AND_FREE(r_ptr);

    return index;
}

uint32_t com::wookler::reactfs::core::base_block::read(uint64_t index, uint32_t count, vector<shared_read_ptr> *data,
                                                       __record_state r_state) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(r_state != __record_state::R_FREE);

    uint64_t si = index;
    uint32_t c = 0;
    temp_buffer *buffer = new temp_buffer();
    CHECK_ALLOC(buffer, TYPE_NAME(temp_buffer));

    temp_buffer *writebuff = nullptr;

    bool r_type = false;
    switch (r_state) {
        case __record_state::R_DELETED:
        case __record_state::R_DIRTY:
        case __record_state::R_ALL:
            r_type = true;
            break;
        default:
            break;
    }

    while (si < header->last_index && c < count) {
        const __record_index_ptr *iptr = index_ptr->read_index(si, r_type);
        if (IS_NULL(iptr)) {
            si++;
            continue;
        }
        __record *ptr = __read_record(iptr->index, iptr->offset, iptr->size);
        CHECK_NOT_NULL(ptr);

        switch (r_state) {
            case __record_state::R_DELETED:
            case __record_state::R_DIRTY:
                if (ptr->header->state != r_state) {
                    si++;
                    continue;
                }
                break;
            default:
                break;
        }

        if (is_encrypted() || is_compressed()) {
            bool use_buffer = false;
            CHECK_NOT_NULL(ptr);
            if (is_encrypted()) {
                use_buffer = true;
                // TODO : Implement encryption handlers.
            }
            if (is_compressed()) {
                CHECK_NOT_NULL(compression);

                void *data_ptr = (use_buffer ? buffer->get_ptr() : ptr->data_ptr);
                uint64_t data_size = (use_buffer ? buffer->get_used_size() : ptr->header->data_size);

                if (use_buffer && IS_NULL(writebuff)) {
                    writebuff = new temp_buffer();
                    CHECK_ALLOC(writebuff, TYPE_NAME(temp_buffer));
                }
                temp_buffer *buff = (use_buffer ? writebuff : buffer);
                int ret = compression->read_archive_data(data_ptr, ptr->header->uncompressed_size, buff);
                POSTCONDITION(ret > 0);

                shared_read_ptr s_ptr = make_shared<__read_ptr>(buff->get_used_size());
                (*s_ptr).copy(buff->get_ptr());

                data->push_back(s_ptr);
            }
        } else {
            shared_read_ptr s_ptr = make_shared<__read_ptr>(ptr->header->data_size);
            (*s_ptr).set_data_ptr(ptr->data_ptr, ptr->header->data_size);

            data->push_back(s_ptr);
        }
        si++;
        c++;
    }

    return data->size();
}

void com::wookler::reactfs::core::base_block::commit(string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(in_transaction());
    PRECONDITION(!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id));

    vector<uint64_t> deleted = index_ptr->get_deleted_indexes(transaction_id);
    if (!IS_EMPTY(deleted)) {
        for (uint64_t index : deleted) {
            const __record_index_ptr *iptr = index_ptr->read_index(index, true);
            CHECK_NOT_NULL(iptr);
            __record *ptr = __read_record(iptr->index, iptr->offset, iptr->size);
            CHECK_NOT_NULL(ptr);
            ptr->header->state = __record_state::R_DELETED;
        }
    }
    index_ptr->commit(transaction_id);
    mm_data->flush();

    void *d_ptr = get_data_ptr();
    uint64_t offset = header->write_offset;
    while (offset <= rollback_info->write_offset) {
        void *ptr = common_utils::increment_data_ptr(d_ptr, offset);
        __record_header *header = static_cast<__record_header *>(ptr);
        header->state = __record_state::R_READABLE;
        offset += (sizeof(__record_header) + header->data_size);
    }
    header->last_index = rollback_info->last_index;
    header->used_bytes += rollback_info->used_bytes;
    header->write_offset = rollback_info->write_offset;
    header->block_checksum = rollback_info->block_checksum;
    header->commit_sequence++;

    end_transaction();
}

void com::wookler::reactfs::core::base_block::rollback(string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(in_transaction());
    PRECONDITION(!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id));
    index_ptr->rollback(transaction_id);

    force_rollback();
}

void com::wookler::reactfs::core::base_block::force_rollback() {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(in_transaction());

    end_transaction();
}

void com::wookler::reactfs::core::base_block::open(uint64_t block_id, string filename) {

    void *ptr = __open_block(block_id, filename);
    POSTCONDITION(NOT_NULL(ptr));
    void *bptr = get_data_ptr();
    CHECK_NOT_NULL(bptr);

    if (header->write_state == __write_state::WRITABLE) {
        write_ptr = bptr;
    } else {
        write_ptr = nullptr;
    }

    index_ptr = new base_block_index();
    CHECK_ALLOC(index_ptr, TYPE_NAME(base_block_index));
    index_ptr->open_index(header->block_id, header->block_uid, this->filename);

    state.set_state(__state_enum::Available);
}

void com::wookler::reactfs::core::base_block::validation_block() {
    CHECK_STATE_AVAILABLE(state);
}