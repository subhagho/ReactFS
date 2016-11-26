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
// Created by Subhabrata Ghosh on 25/11/16.
//

#ifndef REACTFS_BLOCK_COMMON_STRUCTS_V0_H
#define REACTFS_BLOCK_COMMON_STRUCTS_V0_H

#include "common/includes/common.h"
#include "common/includes/base_error.h"

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                typedef enum __block_record_type__ {
                    RAW = 0, /// Raw data block.
                    TYPED = 1 /// Blocked contains typed records.
                } __block_record_type;

                typedef enum __block_type__ {
                    PRIMARY = 0, /// Block is currently assigned to be the primary block.
                    REPLICA = 1, /// Block is currently assigned as a replica block.
                    DELETED = 2 /// Block has been marked as deleted.
                } __block_type;


                typedef enum __compression_type__ {
                    NO_COMPRESSION = -1, /// Data is not compressed.
                    GZIP = 0, /// Data compressed using GZIP
                    LZO, /// Data compressed using LZO
                    ZLIB, /// Data compressed using GZIP
                    SNAPPY /// Data compressed using Snappy
                } __compression_type;

                typedef struct __compression_v0__ {
                    bool compressed = false;
                    __compression_type type = __compression_type::NO_COMPRESSION;
                } __compression_v0;

                typedef enum __encryption_type__ {
                    NO_ENCRYPTION = -1
                } __encryption_type;

                typedef struct __encryption_v0__ {
                    bool encrypted = false;
                    __encryption_type type = __encryption_type::NO_ENCRYPTION;
                } __encryption_v0;

                typedef enum __block_state__ {
                    AVAILABLE = 0, /// Data block is available for read/write
                    LOCKED = 1, /// Data block is locked for writes
                    OUT_OF_SYNC = 2, /// Data block is out-of-sync, needs to be synced
                    COMMIT_PENDING = 3, /// Data block has commit pending.
                    CORRUPTED = 4 /// Block is corrupted and needs to be re-synced.
                } __block_state;

                typedef enum __write_state__ {
                    WRITABLE = 1, /// Block is writable
                    CLOSED = 2 /// Block has been closed for write.
                } __write_state;

                typedef enum __record_state__ {
                    R_FREE = 0, /// Record is unused
                    R_READABLE = 1, /// Record is available to be read.
                    R_DELETED = 2, /// Record has been deleted.
                    R_DIRTY = 3, /// Record is dirty
                    R_ALL = 4 /// Record fetch state (Read records in all states)
                } __record_state;

                typedef struct __record_header__v0__ {
                    uint64_t index = 0;
                    uint64_t offset = 0;
                    __record_state state = __record_state::R_FREE;
                    uint64_t data_size = 0;
                    uint64_t uncompressed_size = 0;
                    uint64_t timestamp = 0;
                } __record_header_v0;

                typedef struct __record__v0__ {
                    __record_header_v0 *header;
                    void *data_ptr = nullptr;
                } __record_v0;


                typedef struct __record_index_header_v0__ {
                    char block_uid[SIZE_UUID];
                    __version_header version;
                    uint64_t block_id;
                    uint64_t create_time = 0;
                    uint64_t update_time = 0;
                    uint64_t start_index = 0;
                    uint64_t last_index = 0;
                    __write_state write_state = __write_state::WRITABLE;
                    uint64_t total_size = 0;
                    uint64_t used_size = 0;
                    uint64_t write_offset = 0;
                } __record_index_header_v0;

                typedef struct __record_index_ptr_v0__ {
                    uint64_t index = 0;
                    bool readable = false;
                    uint64_t offset = 0;
                    uint64_t size = 0;
                } __record_index_ptr_v0;

                typedef struct __rollback_info_v0__ {
                    bool in_transaction = false;
                    string *transaction_id;
                    uint64_t start_offset = 0;
                    uint64_t write_offset = 0; // Write-offset of the last committed write.
                    uint64_t used_bytes = 0;
                    uint64_t last_index = 0;
                    uint64_t start_time = 0;
                    __record_index_ptr_v0 *start_index = nullptr;
                } __rollback_info_v0;

                typedef struct __block_header_v0__ {
                    __version_header version;
                    char block_uid[SIZE_UUID];
                    uint64_t source_id;
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
                    uint64_t block_size;
                    uint64_t write_offset;
                    uint64_t used_bytes = 0;
                    __compression_v0 compression;
                    __encryption_v0 encryption;
                } __block_header_v0;
            }
        }
    }
}
#endif //REACTFS_BLOCK_COMMON_STRUCTS_V0_H
