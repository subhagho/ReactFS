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

                };
REACTFS_NS_CORE_END

#endif //REACTFS_TYPED_BLOCK_H
