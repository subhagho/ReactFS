//
// Created by Subhabrata Ghosh on 01/12/16.
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

#ifndef REACTFS_BASE_INDEXED_BLOCK_H
#define REACTFS_BASE_INDEXED_BLOCK_H

#include "base_block.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class base_indexed_block : public base_block {
                protected:

                    /// Index pointer to load the data record
                    base_block_index *index_ptr = nullptr;

                public:
                    virtual ~base_indexed_block() {
                        this->close();
                    }

                    /*!
                     * Write a data record to the block. Check should be done to ensure that
                     * enough space is available in the block for the required data length, else
                     * the call will throw an exception.
                     *
                     * @param source - Source buffer to copy data from.
                     * @param length - Length of data to copy.
                     * @param transaction_id - Transaction ID.
                     * @return - Record index of the create record.
                     */
                    virtual uint64_t write(void *source, uint32_t length, string transaction_id) override;

                    /*!
                     * Close this data block instance.
                     */
                    virtual void close() override;

                    /*!
                     * Start a new write transcation on this block. If block is currently locked for write,
                     * then the call will wait till the block is available or the specified timeout is reached.
                     *
                     * @return - Transaction UUID.
                     */
                    virtual string
                    start_transaction(uint64_t timeout, const string &tid = common_consts::EMPTY_STRING) override;

                    /*!
                     * Read a set of records from the block starting at the specified record index. If count exceeds the
                     * number of available record, only the set of available records will be returned.
                     *
                     * @param index - Start record index.
                     * @param count - No. of records to read.
                     * @param data - Result vector to copy the records to.
                     * @param r_state - Type of records to read.
                     * @return - No. of records returned.
                     */
                    virtual uint32_t read(uint64_t index, uint32_t count, vector<shared_read_ptr> *data,
                                          __record_state r_state = __record_state::R_READABLE) override;


                    /*!
                     * Commit the current transcation.
                     *
                     * @param transaction_id - Transaction ID obtained via a start_transaction call.
                     */
                    virtual void commit(string transaction_id) override;

                    /*!
                     * Rollback the current transcation.
                     *
                     * @param transaction_id - Transaction ID obtained via a start_transaction call.
                     */
                    virtual void rollback(string transaction_id) override;

                    /*!
                     * Open a new instance of the specified data block.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param filename - Backing filename for this data block.
                     * @param for_update - Open block for update.
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    virtual void open(uint64_t block_id, string filename, bool for_update) override;

                    /*!
                     * Validate the data sanity of this block.
                     */
                    virtual __block_check_record *check_block_sanity() override;

                    /*!
                     * Create a new instance of a raw data block.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param parent_id - Unique ID of the parent structure.
                     * @param filename - Backing filename for this data block.
                     * @param usage - Usage type of this data block. (PRIMARY or REPLICATED)
                     * @param block_size - Maximum data size allowed for this block.
                     * @param start_index - Starting record index for this block.
                     * @param overwrite - Overwrite if data block file already exists?
                     * @return - UUID of the new block created.
                     */
                    virtual string create(uint64_t block_id, uint64_t parent_id, string filename, __block_usage usage,
                                          uint64_t block_size, uint64_t start_index, uint32_t est_record_size,
                                          bool overwrite) override {
                        string uuid = __create_block(block_id, parent_id, filename, usage, __block_def::INDEXED,
                                                     block_size, start_index, overwrite);
                        uint32_t estimated_records = (block_size / est_record_size);

                        index_ptr = new base_block_index();
                        CHECK_ALLOC(index_ptr, TYPE_NAME(base_block_index));

                        index_ptr->create_index(header->block_id, header->block_uid, this->filename, estimated_records,
                                                header->start_index, overwrite);
                        close();

                        POSTCONDITION(!IS_EMPTY(uuid));
                        return uuid;
                    }


                    /*!
                     * Delete the record at the specified index.
                     *
                     * @param index - Record index.
                     * @param transaction_id - Current transaction Id.
                     * @return - Is deleted?
                     */
                    virtual bool delete_record(uint64_t index, string transaction_id) override {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(is_writeable());
                        PRECONDITION(in_transaction());
                        PRECONDITION(!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id));
                        PRECONDITION(index >= header->start_index && index <= get_current_index());

                        const __record_index_ptr *iptr = index_ptr->read_index(index, true);
                        if (IS_NULL(iptr)) {
                            throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_INDEX_NOT_FOUND,
                                                 "Index missing from block index. [index=%lu]", index);
                        }
                        __record *ptr = __read_record(index, iptr->offset, iptr->size);
                        if (IS_NULL(ptr)) {
                            throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_RECORD_NOT_FOUND,
                                                 "Record not found in block. [index=%lu]", index);
                        }
                        PRECONDITION(is_record_in_valid_state(ptr));
                        CHECK_AND_FREE(ptr);

                        header->update_time = time_utils::now();

                        return index_ptr->delete_index(index, transaction_id);
                    }
                };
            }
        }
    }
}

#endif //REACTFS_BASE_INDEXED_BLOCK_H
