//
// Created by Subhabrata Ghosh on 21/12/16.
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

#ifndef REACTFS_TYPED_HASH_INDEX_H
#define REACTFS_TYPED_HASH_INDEX_H

#include "common/includes/common.h"
#include "common/includes/__state__.h"
#include "common/includes/log_utils.h"
#include "common/includes/common_utils.h"
#include "common/includes/file_utils.h"
#include "common/includes/timer.h"
#include "common/includes/__alarm.h"
#include "common/includes/read_write_lock.h"
#include "common/includes/mapped_data.h"
#include "common/includes/buffer_utils.h"
#include "common/includes/ext/murmur3.h"

#include "core.h"
#include "common_structs.h"
#include "fs_error_base.h"
#include "typed_index_base.h"

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;

#define DEFAULT_BLOAT_FACTOR (15 / 10)
#define HASH_INDEX_VERSION_MAJOR ((uint16_t) 0)
#define HASH_INDEX_VERSION_MINOR ((uint16_t) 1)

REACTFS_NS_CORE
                class typed_hash_index : public typed_index_base {

                public:

                    virtual ~typed_hash_index() {

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
                    create_index(uint64_t block_id, string block_uuid, string filename, uint32_t estimated_records,
                                 uint64_t start_index,
                                 bool overwrite) override;

                    /*!
                     * Open a new instance of the specified data block index.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param block_uuid - UUID of the data block.
                     * @param filename - Backing filename for this data block.
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    void open_index(uint64_t block_id, string block_uuid, string filename) override;

                    /*!
                    * Commit the current transcation.
                    *
                    * @param transaction_id - Transaction ID obtained via a start_transaction call.
                    */
                    void commit(string txid) override;

                    /*!
                     * Rollback the current transcation.
                     *
                     * @param transaction_id - Transaction ID obtained via a start_transaction call.
                     */
                    void rollback(string txid) override;

                    /*!
                     * Force a transaction rollback.
                     *
                     * Should be used only in exception cases, such as transaction timeout.
                     */
                    void force_rollback() override;

                    /*!
                     * Delete the specified index record.
                     *
                     * @param index - Index key of Record to delete.
                     * @param transaction_id - Current transaction ID.
                     * @return - Is deleted?
                     */
                    bool delete_index(__index_key_set *index, string transaction_id) override;

                    /*!
                     * Get the available free space that this block has.
                     *
                     * @return - Space available (in bytes).
                     */
                    const uint64_t get_free_space() const override;

                    /*!
                     * Get the space currently used by this block.
                     *
                     * @return - Space used (in bytes).
                     */
                    const uint64_t get_used_space() const override;

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
                    write_index(__index_key_set *index, uint64_t offset, uint64_t size, string transaction_id) override;

                    /*!
                     * Read the index record for the specified index.
                     *
                     * @param index - Data record index.
                     * @param all - Allow to read dirty/deleted records?
                     * @return - Index record pointer.
                     */
                    const __record_index_ptr *read_index(__index_key_set *index, bool all = false) override;

                    /*!
                     * Close this instance of the block index.
                     */
                    void close() override;

                    void sync(bool recreate = false) override;
                };
REACTFS_NS_CORE_END

#endif //REACTFS_TYPED_HASH_INDEX_H
