//
// Created by Subhabrata Ghosh on 16/11/16.
//


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

#ifndef REACTFS_BASE_BLOCK_INDEX_H
#define REACTFS_BASE_BLOCK_INDEX_H

#include "common/includes/common.h"
#include "common/includes/__state__.h"
#include "common/includes/log_utils.h"
#include "common/includes/common_utils.h"
#include "common/includes/file_utils.h"
#include "common/includes/timer.h"
#include "common/includes/__alarm.h"
#include "common/includes/read_write_lock.h"

#include "fmstream.h"
#include "common_structs.h"
#include "fs_error_base.h"

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;

#define DEFAULT_BLOAT_FACTOR 1.5f

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class base_block_index {
                protected:
                    //! State of this instance of the block object.
                    __state__ state;

                    //! Backing file path for the memory mapped file.
                    string filename;

                    /// Memory-mapped file handle.
                    fmstream *stream = nullptr;

                    /// Current (committed) write pointer
                    void *write_ptr = nullptr;

                    /// Base pointer for the mapped data.
                    void *base_ptr = nullptr;

                    /// Data file header for the index data.
                    __record_index_header *header = nullptr;

                    /// Rollback information (in case a write transaction is in progress)
                    __rollback_info *rollback_info = nullptr;

                    /*!
                     * Create a new file backed data block index.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param block_uuid - UUID of the data block.
                     * @param filename - Backing filename for this data block.
                     * @param estimated_records - Estimated number of records the data file is expected to have.
                     * @param start_index - Starting record index for this block.
                     * @param overwrite - Overwrite if data block file already exists?
                     * @return - Filename of the index file created.
                     */
                    string
                    __create_index(uint64_t block_id, string block_uuid, string filename, uint32_t estimated_records,
                                   uint64_t start_index,
                                   bool overwrite);

                    /*!
                     * Open a new instance of the specified data block index.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param block_uuid - UUID of the data block.
                     * @param filename - Backing filename for this data block.
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    void *__open_block(uint64_t block_id, string block_uuid, string filename);

                    /*!
                     * Close this instance of the block index.
                     */
                    void close();

                    /*!
                     * Increment the void pointer by the specified number of bytes.
                     *
                     * @param ptr - Void pointer address.
                     * @param offset - Byte offset to increment by.
                     * @return - Incremented pointer.
                     */
                    void *increment_data_ptr(void *ptr, uint32_t offset) {
                        BYTE_PTR cptr = static_cast<BYTE_PTR>(ptr);
                        return (cptr + offset);
                    }

                    /*!
                     * Get the base address pointing to where the block data starts.
                     *
                     * @return - Base data address
                     */
                    void *get_data_ptr() {
                        BYTE_PTR cptr = static_cast<BYTE_PTR>(base_ptr);
                        return (cptr + sizeof(__record_index_header));
                    }
                };
            }
        }
    }
}

string com::wookler::reactfs::core::base_block_index::__create_index(uint64_t block_id, string block_uuid,
                                                                     string filename, uint32_t estimated_records,
                                                                     uint64_t start_index, bool overwrite) {
    try {
        Path pp(filename);
        string fname = string(pp.get_filename());
        fname.append(".index");

        Path p(pp.get_parent_dir());
        p.append(fname);
        LOG_DEBUG("Creating new index file. [file=%s]", p.get_path().c_str());
        if (p.exists()) {
            if (!overwrite) {
                throw FS_BASE_ERROR("File with specified path already exists. [path=%s]", filename.c_str());
            } else {
                if (!p.remove()) {
                    throw FS_BASE_ERROR("Error deleting existing file. [file=%s]", filename.c_str());
                }
            }
        }
        uint64_t r_size = (estimated_records * sizeof(__record_index_ptr) * DEFAULT_BLOAT_FACTOR);
        uint64_t ifile_size =
                sizeof(__record_index_header) + r_size;
        LOG_DEBUG("Creating index file with size = %lu", ifile_size);
        stream = new fmstream(p.get_path().c_str(), ifile_size);
        CHECK_NOT_NULL(stream);
        base_ptr = stream->data();

        memset(base_ptr, 0, ifile_size);

        header = reinterpret_cast<__record_index_header *>(base_ptr);
        header->block_id = block_id;
        memcpy(header->block_uid, block_uuid.c_str(), block_uuid.length());
        header->create_time = time_utils::now();
        header->update_time = header->create_time;
        header->start_index = start_index;
        header->last_index = header->start_index;
        header->total_size = r_size;
        header->used_size = 0;

        state.set_state(__state_enum::Available);

        return string(p.get_path());
    } catch (const exception &e) {
        fs_error_base err = FS_BASE_ERROR("Error creating block index. [block id=%lu][filename=%s][error=%s]", block_id,
                                          filename.c_str(), e.what());
        state.set_error(&err);
        throw err;
    } catch (...) {
        fs_error_base err = FS_BASE_ERROR("Error creating block index. [block id=%lu][filename=%s][error=%s]", block_id,
                                          filename.c_str(), "UNKNOWN ERROR");
        state.set_error(&err);
        throw err;
    }
}

void *com::wookler::reactfs::core::base_block_index::__open_block(uint64_t block_id, string block_uuid,
                                                                  string filename) {
    try {
        Path pp(filename);
        string fname = string(pp.get_filename());
        fname.append(".index");

        Path p(pp.get_parent_dir());
        p.append(fname);
        LOG_DEBUG("Creating new index file. [file=%s]", p.get_path().c_str());

        if (!p.exists()) {
            throw FS_BASE_ERROR("File not found. [path=%s]", filename.c_str());
        }
        stream = new fmstream(filename.c_str());
        CHECK_NOT_NULL(stream);
        base_ptr = stream->data();

        header = reinterpret_cast<__record_index_header *>(base_ptr);
        POSTCONDITION(header->block_id == block_id);
        POSTCONDITION(strncmp(header->block_uid, block_uuid.c_str(), block_uuid.length()));

        this->filename = string(p.get_path());

        return base_ptr;
    } catch (const exception &e) {
        fs_error_base err = FS_BASE_ERROR("Error opening block index. [block id=%lu][filename=%s][error=%s]", block_id,
                                          filename.c_str(), e.what());
        state.set_error(&err);
        throw err;
    } catch (...) {
        fs_error_base err = FS_BASE_ERROR("Error opening block index. [block id=%lu][filename=%s][error=%s]", block_id,
                                          filename.c_str(), "UNKNOWN ERROR");
        state.set_error(&err);
        throw err;
    }
}

#endif //REACTFS_BASE_BLOCK_INDEX_H
