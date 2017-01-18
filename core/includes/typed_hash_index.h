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
#include "common/includes/metrics.h"

#include "core.h"
#include "common_structs.h"
#include "fs_error_base.h"
#include "typed_index_base.h"

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;

#define HASH_DEFAULT_BLOAT_FACTOR (15 / 10)
#define HASH_COLLISION_ESTIMATE 8
#define HASH_INDEX_METRIC_READ_PREFIX "metric.hash.index.block.read"
#define HASH_INDEX_METRIC_WRITE_PREFIX "metric.hash.index.block.write"

#define HASH_INDEX_VERSION_MAJOR ((uint16_t) 0)
#define HASH_INDEX_VERSION_MINOR ((uint16_t) 1)

REACTFS_NS_CORE
                struct __hash_index_header_v0__ {
                    uint16_t bucket_prime = 0;
                    uint32_t bucket_count = 0;
                    uint16_t key_size = 0;
                    uint32_t overflow_count = 0;
                    uint64_t overflow_offset = 0;
                    uint32_t overflow_write_index = 0;
                };

                typedef __hash_index_header_v0__ __hash_index_header;

                class typed_hash_index : public typed_index_base {
                protected:
                    __hash_index_header *hash_header = nullptr;
                    void *overflow = nullptr;

                    uint16_t compute_bucket_prime(uint32_t est_record_count) {
                        if (est_record_count <= 4096) {
                            return common_utils::find_prime(100);
                        } else if (est_record_count <= 4096 * 10) {
                            return common_utils::find_prime(256);
                        } else if (est_record_count <= 4096 * 100) {
                            return common_utils::find_prime(512);
                        } else if (est_record_count <= 4096 * 1000) {
                            return common_utils::find_prime(1024);
                        } else {
                            return common_utils::find_prime(1536);
                        }
                    }

                    uint32_t compute_bucket_size(uint32_t est_record_count, uint16_t prime) {
                        uint32_t b_size = est_record_count / prime / 2;
                        return common_utils::find_prime(b_size);
                    }

                    uint16_t compute_record_size() {
                        uint32_t i_size = 0;
                        if (NOT_NULL(hash_header)) {
                            i_size = hash_header->key_size;
                        }
                        if (i_size == 0) {
                            i_size = index_def->compute_index_record_size();
                        }
                        return __typed_index_bucket::get_index_record_size(HASH_COLLISION_ESTIMATE, i_size);
                    }

                    uint64_t estimate_file_size(uint32_t est_record_count, record_index *index_def) {
                        uint32_t b_size = 0;
                        uint16_t b_count = compute_bucket_prime(est_record_count);
                        uint32_t bucket_size = compute_bucket_size(est_record_count, b_count);
                        uint16_t r_size = compute_record_size();
                        uint64_t bt_size = b_count * bucket_size * r_size;
                        b_size += (bt_size * HASH_DEFAULT_BLOAT_FACTOR);
                        return b_size;
                    }

                    void *get_base_overflow_ptr() {
                        void *ptr = get_data_ptr();
                        return buffer_utils::increment_data_ptr(ptr, hash_header->overflow_offset);
                    }

                    void *get_index_ptr(uint32_t bucket, uint32_t offset) {
                        void *ptr = get_data_ptr();
                        uint64_t off = ((bucket * hash_header->bucket_prime) + offset) * hash_header->key_size;
                        POSTCONDITION(off < header->total_size);
                        return buffer_utils::increment_data_ptr(ptr, off);
                    }

                    bool has_overflow_space() {
                        return (hash_header->overflow_write_index < hash_header->overflow_count);
                    }

                    uint64_t get_next_overflow_offset() {
                        PRECONDITION(has_overflow_space());
                        uint16_t r_size = compute_record_size();

                        uint64_t offset = hash_header->overflow_offset + (r_size * hash_header->overflow_write_index);
                        hash_header->overflow_write_index++;

                        return offset;
                    }

                public:
                    typed_hash_index(const __complex_type *datatype) {
                        CHECK_NOT_NULL(datatype);
                        version.major = HASH_INDEX_VERSION_MAJOR;
                        version.minor = HASH_INDEX_VERSION_MINOR;

                        this->datatype = datatype;
                    }

                    virtual ~typed_hash_index() {
                        DUMP_METRIC(get_metric_name(HASH_INDEX_METRIC_READ_PREFIX));
                        DUMP_METRIC(get_metric_name(HASH_INDEX_METRIC_WRITE_PREFIX));

                        REMOVE_METRIC(get_metric_name(HASH_INDEX_METRIC_READ_PREFIX));
                        REMOVE_METRIC(get_metric_name(HASH_INDEX_METRIC_WRITE_PREFIX));
                        this->close();
                    }

                    /*!
                     * Create a new file backed data block index.
                     *
                     * @param name - Name of this index definition.
                     * @param block_id - Unique block id for this data block.
                     * @param block_uuid - UUID of the data block.
                     * @param filename - Backing filename for this data block.
                     * @param estimated_records - Estimated number of records the data file is expected to have.
                     * @param start_index - Starting record index for this block.
                     * @param index_def - Index definition.
                     * @param overwrite - Overwrite if data block file already exists?
                     * @return - Filename of the index file created.
                     */
                    virtual string
                    create_index(const string &name, uint64_t block_id, string block_uuid, string filename,
                                 uint32_t estimated_records, uint64_t start_index, record_index *index_def,
                                 bool overwrite) override;

                    /*!
                     * Open a new instance of the specified data block index.
                     *
                     * @param name - Name of this index definition.
                     * @param block_id - Unique block id for this data block.
                     * @param block_uuid - UUID of the data block.
                     * @param filename - Backing filename for this data block.
                     * @param for_update - Block is write closed but opened for updates.
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    virtual void
                    open_index(const string &name, uint64_t block_id, string block_uuid, string filename,
                               bool for_update) override;

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
