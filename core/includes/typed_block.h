//
// Created by Subhabrata Ghosh on 12/12/16.
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

#ifndef REACTFS_TYPED_BLOCK_H
#define REACTFS_TYPED_BLOCK_H

#include "common/includes/common.h"
#include "types/includes/schema_def.h"
#include "types/includes/type_defs.h"

#include "core.h"
#include "base_block.h"
#include "base_indexed_block.h"

using namespace REACTFS_NS_CORE_PREFIX::types;

REACTFS_NS_CORE

                class typed_block : public base_indexed_block {
                protected:
                    __complex_type *datetype = nullptr;
                    void *read_ptr = nullptr;

                    /*!
                     * Get the base address pointing to where the block data starts.
                     *
                     * @return - Base data address
                     */
                    virtual void *get_data_ptr() override {
                        CHECK_NOT_NULL(read_ptr);
                        return read_ptr;
                    }

                public:
                    void set_datatype(__complex_type *datatype) {
                        CHECK_NOT_NULL(datatype);
                        this->datetype = datatype;
                    }

                    /*!
                     * Open a new instance of the specified data block.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param filename - Backing filename for this data block.
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    virtual void open(uint64_t block_id, string filename) override;

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
                                          bool overwrite) override;

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
                    virtual uint32_t read_struct(uint64_t index, uint32_t count, vector<shared_read_ptr> *data,
                                          __record_state r_state = __record_state::R_READABLE);

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
                     * Create a new data record and save it to the backing file.
                     *
                     * @param source - Source address to copy the data bytes from.
                     * @param uncompressed_size - In case of compression, the size of the record before compression.
                     * @return - Record pointer of the added record.
                     */
                    __record *
                    __write_record(__struct_datatype__ *source, string transcation_id, uint64_t uncompressed_size);

                    /*!
                     * Read a record from this block at the specified index.
                     *
                     * @param index - Start index of the record.
                     * @param offset - Data file offset, read from the index.
                     * @param size - Size of the data record, read from the index.
                     * @return - Fetched record.
                     */
                    __record *__read_record(uint64_t index, uint64_t offset, uint64_t size);

                };
REACTFS_NS_CORE_END

#endif //REACTFS_TYPED_BLOCK_H
