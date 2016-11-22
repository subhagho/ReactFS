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
// Created by Subhabrata Ghosh on 28/10/16.
//

#ifndef REACTFS_COMMON_STRUCTS_H
#define REACTFS_COMMON_STRUCTS_H

#include "common/includes/common.h"
#include "common/includes/base_error.h"

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                typedef enum __block_record_type__ {
                    RAW = 0, // Raw data block.
                    TYPED = 1 // Blocked contains typed records.
                } __block_record_type;

                typedef enum __block_type__ {
                    PRIMARY = 0, // Block is currently assigned to be the primary block.
                    REPLICA = 1, // Block is currently assigned as a replica block.
                    DELETED = 2 // Block has been marked as deleted.
                } __block_type;


                typedef enum __compression_type__ {
                    NO_COMPRESSION = -1, // Data is not compressed.
                    GZIP = 0, // Data compressed using GZIP
                    LZO, // Data compressed using LZO
                    ZLIB, // Data compressed using GZIP
                    SNAPPY // Data compressed using Snappy
                } __compression_type;

                typedef struct __compression__ {
                    bool compressed = false;
                    __compression_type type = __compression_type::NO_COMPRESSION;
                } __compression;

                typedef enum __encryption_type__ {
                    NO_ENCRYPTION = -1
                } __encryption_type;

                typedef struct __encryption__ {
                    bool encrypted = false;
                    __encryption_type type = __encryption_type::NO_ENCRYPTION;
                } __encryption;

                typedef enum __block_state__ {
                    AVAILABLE = 0, // Data block is available for read/write
                    LOCKED = 1, // Data block is locked for writes
                    OUT_OF_SYNC = 2, // Data block is out-of-sync, needs to be synced
                    COMMIT_PENDING = 3, // Data block has commit pending.
                    CORRUPTED = 4 // Block is corrupted and needs to be re-synced.
                } __block_state;

                typedef enum __write_state__ {
                    WRITABLE = 1, CLOSED = 2
                } __write_state;

                typedef enum __record_state__ {
                    R_FREE = 0, R_READABLE = 1, R_DELETED = 2, R_DIRTY = 3, R_ALL = 4
                } __record_state;

                typedef struct __record_header__ {
                    uint64_t index = 0;
                    uint32_t offset = 0;
                    __record_state state = __record_state::R_FREE;
                    uint32_t data_size = 0;
                    uint32_t uncompressed_size = 0;
                    PADCHAR(0, 4);
                    uint64_t timestamp = 0;
                } __record_header;

                typedef struct __record__ {
                    __record_header *header;
                    void *data_ptr = nullptr;
                } __record;


                typedef struct __record_index_header__ {
                    char block_uid[SIZE_UUID];
                    uint64_t block_id;
                    uint64_t create_time = 0;
                    uint64_t update_time = 0;
                    uint64_t start_index = 0;
                    uint64_t last_index = 0;
                    __write_state write_state = __write_state::WRITABLE;
                    uint32_t total_size = 0;
                    uint32_t used_size = 0;
                    uint32_t write_offset = 0;
                } __record_index_header;

                typedef struct __record_index_ptr__ {
                    uint64_t index = 0;
                    bool readable = false;
                    uint32_t offset = 0;
                    uint32_t size = 0;
                } __record_index_ptr;

                typedef struct __rollback_info__ {
                    bool in_transaction = false;
                    string *transaction_id;
                    uint64_t start_offset = 0;
                    uint64_t write_offset = 0; // Write-offset of the last committed write.
                    uint32_t used_bytes = 0;
                    uint64_t last_index = 0;
                    uint64_t start_time = 0;
                    __record_index_ptr *start_index = nullptr;
                } __rollback_info;

                typedef struct {
                    char block_uid[SIZE_UUID];
                    __block_type block_type = __block_type::PRIMARY;
                    __block_record_type record_type = __block_record_type::RAW;
                    __block_state state = __block_state::AVAILABLE;
                    __write_state write_state = __write_state::WRITABLE;
                    uint64_t block_id;
                    uint64_t create_time;
                    uint64_t update_time;
                    uint64_t start_index = 0;
                    uint64_t last_index = 0;
                    uint64_t block_ttl = 0;
                    uint32_t block_size;
                    uint32_t write_offset;
                    uint32_t used_bytes = 0;
                    __compression compression;
                    __encryption encryption;
                } __block_header;

                class __read_ptr {
                private:
                    bool allocated = false;
                    uint32_t size = 0;
                    void *data_ptr = nullptr;
                    const void *const_ptr = nullptr;
                public:
                    __read_ptr(uint32_t size) {
                        this->size = size;
                    }

                    ~__read_ptr() {
                        if (allocated) {
                            FREE_PTR(data_ptr);
                        }
                        data_ptr = nullptr;
                    }

                    void set_data_ptr(const void *data_ptr, uint32_t size) {
                        PRECONDITION(NOT_NULL(data_ptr));

                        this->const_ptr = data_ptr;
                        this->size = size;
                    }

                    void copy(const void *data_ptr, uint32_t size = 0) {
                        PRECONDITION(NOT_NULL(data_ptr));
                        if (allocated) {
                            FREE_PTR(this->data_ptr);
                        }
                        if (size == 0) {
                            size = this->size;
                        }
                        PRECONDITION(size > 0);

                        this->data_ptr = (BYTE_PTR) malloc(size * sizeof(BYTE_PTR));
                        CHECK_NOT_NULL(this->data_ptr);

                        allocated = true;
                        memcpy(this->data_ptr, data_ptr, size);
                    }

                    const void *get_data_ptr() {
                        if (allocated)
                            return this->data_ptr;
                        else
                            return this->const_ptr;
                    }

                    uint32_t get_size() {
                        return this->size;
                    }
                };

                typedef shared_ptr<__read_ptr> shared_read_ptr;
            }
        }
    }
}
#endif //REACTFS_COMMON_STRUCTS_H
