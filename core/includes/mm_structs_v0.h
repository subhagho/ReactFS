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
#define MM_MAX_MOUNTS 32

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
                    char path[SIZE_MAX_PATH];
                    /// Number of blocks used by this block list on this mount point.
                    uint32_t blocks_used = 0;
                    /// Last timestamp when a block was created on this mount point.
                    uint64_t last_block_time = 0;
                } __mm_mount_v0;

                /*!
                 * Block list record that defines the blocks used by this block list.
                 */
                typedef struct __mm_block_info_v0__ {
                    /// Absolute block file path.
                    char filename[SIZE_MAX_PATH];
                    /// Unique block id.
                    uint32_t block_id;
                    /// Has this block been deleted.
                    bool deleted = false;
                    /// Start index of the records in this block.
                    uint64_t block_start_index = 0;
                    /// Last index of the record written to this block.
                    uint64_t block_last_index = 0;
                } __mm_block_info_v0;

                /*!
                 * Header structure used by the block list(s) to capture block information.
                 */
                typedef struct __mm_data_header_v0__ {
                    /// Schema version of the saved data
                    __version_header version;
                    /// Unique name of this block list.
                    char name[MM_STRUCT_NAME_SIZE];
                    /// Sub-directory prefix, used while creating block files under the mount points.
                    char dir_prefix[SIZE_MAX_PATH];
                    /// Number of mount points defined.
                    uint16_t mounts = 0;
                    /// Size of the blocks to be created.
                    uint32_t block_size = 0;
                    /// Estimated block record size (used for estimating the index file size.)
                    uint32_t block_record_size = 0;
                    /// Number of blocks created by this list (count includes the deleted blocks)
                    uint32_t block_count = 0;
                    /// Block index of the currently writable block.
                    uint32_t write_block_index = 0;
                    /// Last record index that has been written.
                    uint64_t last_written_index = 0;
                    /// Array of block information.
                    __mm_block_info_v0 block_array[MM_MAX_BLOCKS];
                    /// Array of defined mount points.
                    __mm_mount_v0 mount_points[MM_MAX_MOUNTS];
                } __mm_data_header_v0;

            }
        }
    }
}
#endif //REACTFS_MM_STRUCTS_V0_H