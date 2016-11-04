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

#ifndef REACTFS_FS_BLOCK_H
#define REACTFS_FS_BLOCK_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/exclusive_lock.h"

#include "fs_error_base.h"
#include "fmstream.h"
#include "common_structs.h"
#include "archive_reader.h"
#include "block_utils.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class fs_block {
                private:
                    string filename;

                protected:
                    exclusive_lock *block_lock = nullptr;

                    fmstream *stream = nullptr;

                    __block_header *header = nullptr;
                    void *data_ptr = nullptr;
                    void *write_ptr = nullptr;


                    void *write_r(const void *source, uint32_t len);

                    string _create(uint64_t block_id, string filename, __block_type block_type,
                                   __block_record_type record_type, uint64_t block_size, bool overwrite);

                    void *_open(uint64_t block_id, string filename);

                    void read_compressed_block(void *base_ptr);

                    virtual void *get_data_ptr(void *base_ptr) {
                        char *cptr = static_cast<char *>(base_ptr);
                        return cptr + sizeof(__block_header);
                    }

                public:
                    virtual ~fs_block() {
                        CHECK_AND_FREE(block_lock);
                        close();
                    }


                    virtual string open(uint64_t block_id, string filename);

                    virtual string
                    create(uint64_t block_id, string filename, __block_type block_type, uint64_t block_size,
                           bool overwrite) {
                        return create(block_id, filename, block_type, block_size, false, __compression_type::NONE, 0,
                                      overwrite);
                    }

                    virtual string
                    create(uint64_t block_id, string filename, __block_type block_type, uint64_t block_size,
                           bool compressed, __compression_type compression_type, uint64_t raw_size,
                           bool overwrite);

                    virtual const void *read(uint64_t size, uint64_t offset = 0);

                    virtual const void *write(const void *source, uint32_t len) {
                        PRECONDITION(header->writable)
                        return _write(source, len);
                    }

                    virtual const void *write_c(const void *source, uint32_t len) {
                        PRECONDITION(header->compression.compressed);

                        const void *ptr = _write(source, len);
                        POSTCONDITION(header->used_bytes > 0);

                        void *d_ptr = get_data_ptr(stream->data());
                        read_compressed_block(d_ptr);

                        return ptr;
                    }

                    const void *_write(const void *source, uint32_t len);

                    void flush() {
                        CHECK_NOT_NULL(header);
                        CHECK_NOT_NULL(stream);
                        PRECONDITION(stream->is_open());

                        stream->flush();
                    }

                    uint64_t get_block_id() {
                        CHECK_NOT_NULL(header);

                        return header->block_id;
                    }

                    string get_block_uuid() {
                        CHECK_NOT_NULL(header);

                        return header->block_uid;
                    }

                    __block_record_type get_record_type() {
                        CHECK_NOT_NULL(header);

                        return header->record_type;
                    }

                    void *get_data_ptr() {
                        CHECK_NOT_NULL(header);

                        return data_ptr;
                    }

                    virtual void finish() {
                        CHECK_NOT_NULL(header);

                        header->writable = false;
                    }

                    bool is_writable() {
                        CHECK_NOT_NULL(header);

                        return header->writable;
                    }

                    virtual void close() {
                        if (NOT_NULL(stream)) {
                            if (stream->is_open())
                                stream->close();
                            CHECK_AND_FREE (stream);
                        }

                        data_ptr = nullptr;
                        write_ptr = nullptr;
                        header = nullptr;
                    }

                    string get_filename() const {
                        CHECK_NOT_NULL(header);

                        return filename;
                    }

                    uint64_t get_free_space() const {
                        CHECK_NOT_NULL(header);

                        return (header->block_size - header->used_bytes);
                    }

                    uint64_t get_used_space() const {
                        CHECK_NOT_NULL(header);

                        return header->used_bytes;
                    }

                    bool is_compressed() const {
                        CHECK_NOT_NULL(header);

                        return header->compression.compressed;
                    }

                    fs_block *with_compression(bool compression) {
                        CHECK_NOT_NULL(header);

                        header->compression.compressed = compression;

                        return this;
                    }

                    __compression_type get_compression_type() const {
                        CHECK_NOT_NULL(header);

                        return header->compression.type;
                    }

                    fs_block *with_compression_type(__compression_type type) {
                        CHECK_NOT_NULL(header);

                        header->compression.type = type;

                        return this;
                    }

                    uint64_t get_block_ttl() const {
                        CHECK_NOT_NULL(header);

                        return header->block_ttl;
                    }

                    fs_block *with_block_ttl(uint64_t ttl) {
                        PRECONDITION(ttl >= 0);
                        CHECK_NOT_NULL(header);

                        header->block_ttl = ttl;

                        return this;
                    }

                    uint64_t get_create_time() const {
                        CHECK_NOT_NULL(header);

                        return header->create_time;
                    }

                    uint64_t get_update_time() const {
                        CHECK_NOT_NULL(header);

                        return header->update_time;
                    }

                    uint64_t get_block_size() const {
                        CHECK_NOT_NULL(header);

                        return header->block_size;
                    }

                    __block_type get_block_type() const {
                        CHECK_NOT_NULL(header);

                        return header->block_type;
                    }

                    __archival get_archival_type() {
                        CHECK_NOT_NULL(header);

                        return header->archival;
                    }

                    fs_block *with_archival_type(__archival type) {
                        CHECK_NOT_NULL(header);

                        header->archival = type;

                        return this;
                    }

                    virtual void remove();

                    virtual string
                    create_compressed_block(uint64_t size,
                                            __compression_type compression_type) {
                        Path p(filename);
                        string newf = p.get_filename();
                        newf.append(block_utils::get_compression_ext(compression_type));

                        Path np(p.get_parent_dir());
                        np.append(newf);

                        fs_block *block = new fs_block();
                        block->create(this->header->block_id, np.get_path(), this->header->block_type, size, true,
                                      compression_type,
                                      header->used_bytes, true);
                        POSTCONDITION(NOT_NULL(block));
                        CHECK_AND_FREE(block);

                        return string(np.get_path());
                    }

                    static string get_lock_name(uint64_t block_id) {
                        return common_utils::format("block_%lu_lock", block_id);
                    }
                };
            }
        }
    }
}


#endif //REACTFS_FS_BLOCK_H
