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

#ifndef REACTFS_MM_STRUCTS_V0_H
#define REACTFS_MM_STRUCTS_V0_H

#include "common/includes/common.h"

#define MM_STRUCT_NAME_SIZE 64
#define MM_MAX_BLOCKS 1024
#define MM_MAX_SUB_PATH 128

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                /*!
                 * Structure defines mount points that can be defined to balance IO load
                 * across disks.
                 */
                typedef struct __mm_mount_v0__ {
                    /// Root path of the mount point. All files will be created under this root path.
                    char path[SIZE_MAX_PATH + 1];
                    /// Number of blocks used by this block list on this mount point.
                    uint32_t blocks_used = 0;
                    /// Last timestamp when a block was created on this mount point.
                    uint64_t last_block_time = 0;
                } __mm_mount_v0;

                /*!
                 * Block list record that defines the blocks used by this block list.
                 */
                typedef struct __mm_block_info_v0__ {
                    /// Bit index of this block.
                    uint32_t index = 0;
                    /// Indicates if this block record is already used.
                    bool used = false;
                    /// Absolute block file path.
                    char filename[SIZE_MAX_PATH + 1];
                    /// Unique block id.
                    uint32_t block_id;
                    /// UUID of this block.
                    char block_uuid[SIZE_UUID + 1];
                    /// Has this block been deleted.
                    bool deleted = false;
                    /// Start index of the records in this block.
                    uint64_t block_start_index = 0;
                    /// Last index of the record written to this block.
                    uint64_t block_last_index = 0;
                    /// Timestamp when this block was created.
                    uint64_t created_time = 0;
                    /// Last updated timestamp of this block.
                    uint64_t updated_time = 0;
                    /// The block checksum of the data block.
                    uint64_t block_checksum = 0;
                } __mm_block_info_v0;

                /*!
                 * Header structure used by the block list(s) to capture block information.
                 */
                typedef struct __mm_data_header_v0__ {
                    /// Schema version of the saved data
                    __version_header version;
                    /// Unique name of this block list.
                    char name[MM_STRUCT_NAME_SIZE + 1];
                    /// Sub-directory prefix, used while creating block files under the mount points.
                    char dir_prefix[MM_MAX_SUB_PATH + 1];
                    /// File system filename that these blocks are mapped to.
                    char filename[SIZE_MAX_PATH + 1];
                    /// Size of the blocks to be created.
                    uint32_t block_size = 0;
                    /// Estimated block record size (used for estimating the index file size.)
                    uint32_t block_record_size = 0;
                    /// Number of blocks created by this list (count includes the deleted blocks)
                    uint32_t block_count = 0;
                    /// Index of the last block created in this chain
                    uint32_t last_block_index = 0;
                    /// Block index of the currently writable block.
                    uint32_t write_block_index = 0;
                    /// Last record index that has been written.
                    uint64_t last_written_index = 0;
                    /// Time since last updated when a block will be auto-deleted
                    uint64_t block_expiry_time = 0;
                    /// Array of block information.
                    __mm_block_info_v0 block_array[MM_MAX_BLOCKS];
                } __mm_data_header_v0;

            }
        }
    }
}
#endif //REACTFS_MM_STRUCTS_V0_H
