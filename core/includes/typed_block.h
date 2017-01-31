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
#include "typed_index_base.h"
#include "typed_hash_index.h"

#define BLOCK_TYPED_METRIC_READ_PREFIX "metric.typed.block.read"
#define BLOCK_TYPED_METRIC_WRITE_PREFIX "metric.typed.block.write"

#define BLOCK_MAX_INDEXES 16

using namespace REACTFS_NS_CORE_PREFIX::types;

REACTFS_NS_CORE

                typedef struct __index_type__ {
                    bool used = false;
                    __index_type_enum type;
                    uint64_t block_id;
                    char name[SIZE_MAX_NAME + 1];
                    char block_path[SIZE_MAX_PATH + 1];
                } __index_type;

                class typed_block : public base_indexed_block {
                protected:
                    __complex_type *datetype = nullptr;
                    uint64_t *type_header_size = nullptr;
                    uint8_t *index_count = nullptr;
                    __index_type *index_ptrs = nullptr;
                    unordered_map<string, typed_index_base *> indexes;
                    void *read_ptr = nullptr;
                    uint32_t estimated_record_size = 0;

                    /*!
                     * Get the base address pointing to where the block data starts.
                     *
                     * @return - Base data address
                     */
                    virtual void *get_data_ptr() override {
                        CHECK_NOT_NULL(read_ptr);
                        return read_ptr;
                    }

                    uint64_t get_header_offset() {
                        return (sizeof(uint64_t) + (*type_header_size * sizeof(BYTE)) +
                                (sizeof(__index_type) * BLOCK_MAX_INDEXES) + sizeof(uint8_t));
                    }

                    void open_index(__index_type *def);

                    typed_index_base *create_index_instance(const __index_type_enum type) {
                        CHECK_NOT_NULL(this->datetype);

                        if (type == __index_type_enum::HASH_INDEX) {
                            typed_index_base *ptr = new typed_hash_index(this->datetype);
                            CHECK_ALLOC(ptr, TYPE_NAME(typed_hash_index));
                            return ptr;
                        }
                        throw new FS_BASE_ERROR("Invalid index type specified. [type=%d]", type);
                    }

                    __index_type *find_free_index_ptr() {
                        __index_type *ptr = index_ptrs;
                        for (uint8_t ii = 0; ii < *index_count; ii++) {
                            if (!ptr->used) {
                                return ptr;
                            }
                            ptr++;
                        }
                        return nullptr;
                    }

                    void find_used_indexes(vector<__index_type *> *indexes) {
                        __index_type *ptr = index_ptrs;
                        for (uint8_t ii = 0; ii < *index_count; ii++) {
                            if (ptr->used) {
                                indexes->push_back(ptr);
                            }
                            ptr++;
                        }
                    }

                public:
                    typed_block() {
                    }

                    virtual ~typed_block() {
                        DUMP_METRIC(get_metric_name(BLOCK_TYPED_METRIC_READ_PREFIX));
                        DUMP_METRIC(get_metric_name(BLOCK_TYPED_METRIC_WRITE_PREFIX));

                        REMOVE_METRIC(get_metric_name(BLOCK_TYPED_METRIC_READ_PREFIX));
                        REMOVE_METRIC(get_metric_name(BLOCK_TYPED_METRIC_WRITE_PREFIX));
                    }

                    void set_datatype(__complex_type *datatype) {
                        CHECK_NOT_NULL(datatype);
                        this->datetype = datatype;
                    }

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
                    virtual uint32_t read_struct(uint64_t index, uint32_t count, vector<record_struct *> *data,
                                                 __record_state r_state = __record_state::R_READABLE);

                    /*!
                     * Write a data record to the block. Check should be done to ensure that
                     * enough space is available in the block for the required data length, else
                     * the call will throw an exception.
                     *
                     * @param source - Source buffer to copy data from.
                     * @param length - Length of data to copy (Redundant field, not used for typed blocks).
                     * @param transaction_id - Transaction ID.
                     * @return - Record index of the create record.
                     */
                    virtual uint64_t write(void *source, uint32_t length, string transaction_id) override;

                    virtual void close() override;

                    /*!
                     * Write a data record to the block. Check should be done to ensure that
                     * enough space is available in the block for the required data length, else
                     * the call will throw an exception.
                     *
                     * @param source - Source buffer to copy data from.
                     * @return - Record index of the create record.
                     */
                    uint64_t write(void *source, string transaction_id) {
                        return write(source, 0, transaction_id);
                    }


                    /*!
                     * Create a new data record and save it to the backing file.
                     *
                     * @param source - Source address to copy the data bytes from.
                     * @param uncompressed_size - In case of compression, the size of the record before compression.
                     * @return - Record pointer of the added record.
                     */
                    __record *
                    __write_record(mutable_record_struct *source, string transcation_id);

                    /*!
                     * Read a record from this block at the specified index.
                     *
                     * @param index - Start index of the record.
                     * @param offset - Data file offset, read from the index.
                     * @param size - Size of the data record, read from the index.
                     * @return - Fetched record.
                     */
                    __record *__read_record(uint64_t index, uint64_t offset, uint64_t size);

                    void add_index(record_index *index);
                };

REACTFS_NS_CORE_END

#endif //REACTFS_TYPED_BLOCK_H
