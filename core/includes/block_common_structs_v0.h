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

#define BLOCK_RECORD_STATE_FREE 0
#define BLOCK_RECORD_STATE_USED 1
#define BLOCK_RECORD_STATE_READABLE 2
#define BLOCK_RECORD_STATE_DELETED 4
#define BLOCK_RECORD_STATE_UPDATED 8

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {

                /*!
                 * Enum defines how the specified data block is to be used.
                 */
                typedef enum __block_usage__ {
                    /// Block is currently assigned to be the primary block.
                            PRIMARY = 0,
                    /// Block is currently assigned as a replica block.
                            REPLICA = 1,
                    /// Block has been marked as deleted.
                            DELETED = 2
                } __block_usage;

                /*!
                 * Enum defines the type of the data block.
                 */
                typedef enum __block_def__ {
                    /// Basic (unindexed data block)
                            BASIC = 0,
                    /// Row indexed data block.
                            INDEXED = 1,
                    /// Data block with sortable index.
                            TYPED = 2,
                    /// Data block with Map based index.
                            MAP = 3
                } __block_def;

                /*!
                 * Enum defines the compression type (if any) for the data block.
                 */
                typedef enum __compression_type__ {
                    /// Data is not compressed.
                            NO_COMPRESSION = -1,
                    /// Data compressed using GZIP
                            GZIP = 0,
                    /// Data compressed using LZO
                            LZO,
                    /// Data compressed using GZIP
                            ZLIB,
                    /// Data compressed using Snappy
                            SNAPPY
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

                /*!
                 * Enum defines the current state of the data block.
                 */
                typedef enum __block_state__ {
                    /// Data block is available for read/write
                            AVAILABLE = 0,
                    /// Data block is locked for writes
                            LOCKED = 1,
                    /// Data block is out-of-sync, needs to be synced
                            OUT_OF_SYNC = 2,
                    /// Data block has commit pending.
                            COMMIT_PENDING = 3,
                    /// Block is corrupted and needs to be re-synced.
                            CORRUPTED = 4
                } __block_state;

                /*!
                 * Enum defines the write state of the data block.
                 */
                typedef enum __write_state__ {
                    /// Block is writable
                            WRITABLE = 1,
                    /// Block has been closed for write.
                            CLOSED = 2,
                    /// Block records can be updated (deleted)
                            UPDATEABLE = 3
                } __write_state;

                /*!
                 * Enum defines the current state of the data record.
                 */
                typedef enum __record_state__ {
                    /// Record is unused
                            R_FREE = 0,
                    /// Record is available to be read.
                            R_READABLE = 1,
                    /// Record has been deleted.
                            R_DELETED = 2,
                    /// Record is dirty
                            R_DIRTY = 3,
                    /// Record fetch state (Read records in all states)
                            R_ALL = 4
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
                    char block_uid[SIZE_UUID + 1];
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
                    uint8_t state = BLOCK_RECORD_STATE_FREE;
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
                    char block_uid[SIZE_UUID + 1];
                    uint64_t parent_id;
                    __block_usage usage = __block_usage::PRIMARY;
                    __block_def type = __block_def::BASIC;
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
                    uint64_t commit_sequence = 0;
                    uint32_t total_records = 0;
                    uint32_t deleted_records = 0;
                    __compression_v0 compression;
                    __encryption_v0 encryption;
                } __block_header_v0;
            }
        }
    }
}
#endif //REACTFS_BLOCK_COMMON_STRUCTS_V0_H
