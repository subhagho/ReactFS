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

#include <vector>

#include "common/includes/common.h"
#include "common/includes/__state__.h"
#include "common/includes/log_utils.h"
#include "common/includes/common_utils.h"
#include "common/includes/file_utils.h"
#include "common/includes/timer.h"
#include "common/includes/__alarm.h"
#include "common/includes/read_write_lock.h"
#include "common/includes/mapped_data.h"

#include "common_structs.h"
#include "fs_error_base.h"

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;

#define DEFAULT_BLOAT_FACTOR (15 / 10)
#define BLOCK_INDEX_VERSION_MAJOR ((uint16_t) 0)
#define BLOCK_INDEX_VERSION_MINOR ((uint16_t) 1)

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class base_block_index {
                protected:
                    __version_header version;

                    //! State of this instance of the block object.
                    __state__ state;

                    //! Backing file path for the memory mapped file.
                    string filename;

                    /// Memory-mapped file handle.
                    file_mapped_data *mm_data = nullptr;

                    /// Current (committed) write pointer
                    void *write_ptr = nullptr;

                    /// Base pointer for the mapped data.
                    void *base_ptr = nullptr;

                    /// Data file header for the index data.
                    __record_index_header *header = nullptr;

                    /// Rollback information (in case a write transaction is in progress)
                    __rollback_info *rollback_info = nullptr;

                    vector<uint64_t> rollback_info_deletes;

                    /*!
                     * Get the index filename for the specified block file.
                     *
                     * @param filename - Block filename.
                     * @return - Index file path.
                     */
                    Path *get_index_file(string filename) {
                        Path pp(filename);
                        string fname = string(pp.get_filename());
                        fname.append(".index");

                        Path *p = new Path(pp.get_parent_dir());
                        p->append(fname);

                        return p;
                    }

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
                    void *__open_index(uint64_t block_id, string block_uuid, string filename);

                    /*!
                     * Close this instance of the block index.
                     */
                    void close();

                    /*!
                     * Get the base address pointing to where the block data starts.
                     *
                     * @return - Base data address
                     */
                    void *get_data_ptr() {
                        return common_utils::increment_data_ptr(base_ptr, sizeof(__record_index_header));
                    }

                    /*!
                     * Setup a new transaction context.
                     *
                     * @param txid - Transaction ID of the current transaction.
                     * @return - New transaction ID.
                     */
                    string setup_transaction(string txid) {
                        if (IS_NULL(rollback_info)) {
                            rollback_info = (__rollback_info *) malloc(sizeof(__rollback_info));
                            rollback_info->transaction_id = new string();
                        }
                        CHECK_NOT_NULL(rollback_info);

                        rollback_info->in_transaction = true;
                        rollback_info->last_index = header->last_index;
                        rollback_info->start_time = time_utils::now();
                        rollback_info->transaction_id->assign(txid);
                        rollback_info->write_offset = header->write_offset;
                        rollback_info->start_offset = header->write_offset;
                        rollback_info->start_index = nullptr;
                        rollback_info->used_bytes = 0;

                        rollback_info_deletes.clear();

                        return *rollback_info->transaction_id;
                    }

                    /*!
                     * End the currently active transaction session.
                     *
                     */
                    void end_transaction() {
                        string thread_id = thread_utils::get_current_thread();

                        CHECK_NOT_NULL(rollback_info);

                        rollback_info->in_transaction = false;
                        rollback_info->last_index = 0;
                        rollback_info->start_time = 0;
                        rollback_info->transaction_id->assign(EMPTY_STRING);
                        rollback_info->write_offset = 0;
                        rollback_info->start_offset = 0;
                        rollback_info->start_index = nullptr;
                        rollback_info->used_bytes = 0;

                        rollback_info_deletes.clear();
                    }

                    /*!
                     * Check if there is a write transaction in progress.
                     *
                     * @return - Is transaction in progress?
                     */
                    bool in_transaction() const {
                        if (NOT_NULL(rollback_info)) {
                            return rollback_info->in_transaction;
                        }
                        return false;
                    }

                    /*!
                     * Get the write pointer from where the next record should be writter.
                     *
                     * @return - Write pointer.
                     */
                    void *get_write_ptr() {
                        if (in_transaction()) {
                            return common_utils::increment_data_ptr(write_ptr, rollback_info->write_offset);
                        } else {
                            return common_utils::increment_data_ptr(write_ptr, header->write_offset);
                        }
                    }

                    /*!
                     * Get the last index for which a record has been written.
                     *
                     * @return - Last written index.
                     */
                    uint64_t get_last_index() {
                        if (in_transaction()) {
                            return rollback_info->last_index;
                        } else {
                            return header->last_index;
                        }
                    }

                    /*!
                     * Check if there is space available to write a data record of the size specified.
                     *
                     * @param size - Data record size required.
                     * @return - Is space available?
                     */
                    bool has_space(uint64_t size) {
                        uint64_t free_space = get_free_space();
                        return (free_space >= size);
                    }

                    /*!
                     * Create a new index record for the specified index and offset.
                     *
                     * @param index - Record sequence index.
                     * @param offset - Read offset in the data file.
                     * @param size - Size of the data record in bytes.
                     * @param transaction_id - Current transaction ID.
                     * @return - Created index pointer.
                     */
                    __record_index_ptr *
                    __write_index(uint64_t index, uint64_t offset, uint64_t size, string transaction_id);

                    /*!
                     * Read the index record for the specified index.
                     *
                     * @param index - Data record index.
                     * @param all - Allow to read dirty/deleted records?
                     * @return - Index record pointer.
                     */
                    __record_index_ptr *__read_index(uint64_t index, bool all);

                public:
                    base_block_index() {
                        version.major = BLOCK_INDEX_VERSION_MAJOR;
                        version.minor = BLOCK_INDEX_VERSION_MINOR;
                    }

                    ~base_block_index() {
                        this->close();
                    }

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
                    virtual string
                    create_index(uint64_t block_id, string block_uuid, string filename, uint32_t estimated_records,
                                 uint64_t start_index,
                                 bool overwrite) {
                        string path = __create_index(block_id, block_uuid, filename, estimated_records, start_index,
                                                     overwrite);
                        close();

                        return string(path);
                    }

                    /*!
                     * Open a new instance of the specified data block index.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param block_uuid - UUID of the data block.
                     * @param filename - Backing filename for this data block.
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    void open_index(uint64_t block_id, string block_uuid, string filename);

                    /*!
                     * Close this block index from further writes.
                     */
                    void finish() {
                        header->write_state = __write_state::CLOSED;
                    }

                    /*!
                     * Start a new write transaction on this block index. The block index does not provide any
                     * locked isolation and assumes the attached block is managing concurrency control.
                     *
                     * @param txid - Transaction ID of the transaction started by the attached block.
                     */
                    void start_transaction(string txid) {
                        setup_transaction(txid);
                    }

                    /*!
                     * Commit the current transcation.
                     *
                     * @param transaction_id - Transaction ID obtained via a start_transaction call.
                     */
                    void commit(string txid) {
                        PRECONDITION(in_transaction());
                        PRECONDITION(*rollback_info->transaction_id == txid);

                        mm_data->flush();

                        uint64_t offset = rollback_info->start_offset;
                        while (offset < rollback_info->write_offset) {
                            void *ptr = common_utils::increment_data_ptr(write_ptr, offset);
                            __record_index_ptr *iptr = reinterpret_cast<__record_index_ptr *>(ptr);
                            PRECONDITION(!iptr->readable);
                            iptr->readable = true;
                            offset += sizeof(__record_index_ptr);
                        }
                        header->last_index = rollback_info->last_index;
                        header->used_size += rollback_info->used_bytes;
                        header->write_offset = rollback_info->write_offset;

                        if (!IS_EMPTY(rollback_info_deletes)) {
                            for (uint64_t index : rollback_info_deletes) {
                                __record_index_ptr *ptr = __read_index(index, true);
                                CHECK_NOT_NULL(ptr);
                                ptr->readable = false;
                            }
                        }
                        end_transaction();
                    }

                    /*!
                     * Rollback the current transcation.
                     *
                     * @param transaction_id - Transaction ID obtained via a start_transaction call.
                     */
                    void rollback(string txid) {
                        PRECONDITION(in_transaction());
                        PRECONDITION(*rollback_info->transaction_id == txid);

                        force_rollback();
                    }

                    /*!
                     * Force a transaction rollback.
                     *
                     * Should be used only in exception cases, such as transaction timeout.
                     */
                    void force_rollback() {
                        end_transaction();
                    }

                    /*!
                     * Delete the specified index record.
                     *
                     * @param index - Index of Record to delete.
                     * @param transaction_id - Current transaction ID.
                     * @return - Is deleted?
                     */
                    bool delete_index(uint64_t index, string transaction_id);

                    /*!
                     * Get the list of indexes that have been deleted.
                     *
                     * @param transaction_id - Current transaction ID.
                     * @return - List of deleted indexes.
                     */
                    vector<uint64_t> get_deleted_indexes(string transaction_id) {
                        return rollback_info_deletes;
                    }

                    /*!
                     * Get the available free space that this block has.
                     *
                     * @return - Space available (in bytes).
                     */
                    const uint64_t get_free_space() const {
                        CHECK_STATE_AVAILABLE(state);
                        if (in_transaction()) {
                            return (header->total_size -
                                    (header->used_size + rollback_info->used_bytes + sizeof(__record_index_header)));
                        }
                        return (header->total_size - (header->used_size + sizeof(__record_index_header)));
                    }

                    /*!
                     * Get the space currently used by this block.
                     *
                     * @return - Space used (in bytes).
                     */
                    const uint64_t get_used_space() const {
                        CHECK_STATE_AVAILABLE(state);
                        if (in_transaction()) {
                            return (header->used_size + rollback_info->used_bytes);
                        }
                        return header->used_size;
                    }

                    /*!
                     * Create a new index record for the specified index and offset.
                     *
                     * @param index - Record sequence index.
                     * @param offset - Read offset in the data file.
                     * @param size - Size of the data record in bytes.
                     * @param transaction_id - Current transaction ID.
                     * @return - Created index pointer.
                     */
                    const __record_index_ptr *
                    write_index(uint64_t index, uint64_t offset, uint64_t size, string transaction_id) {
                        return __write_index(index, offset, size, transaction_id);
                    }

                    /*!
                     * Read the index record for the specified index.
                     *
                     * @param index - Data record index.
                     * @param all - Allow to read dirty/deleted records?
                     * @return - Index record pointer.
                     */
                    const __record_index_ptr *read_index(uint64_t index, bool all = false) {
                        return __read_index(index, all);
                    }
                };
            }
        }
    }
}


#endif //REACTFS_BASE_BLOCK_INDEX_H
