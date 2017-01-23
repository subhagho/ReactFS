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
using namespace com::wookler::reactfs::core::types;

#define HASH_DEFAULT_BLOAT_FACTOR (15 / 10)
#define HASH_COLLISION_ESTIMATE 8
#define HASH_INDEX_METRIC_READ_PREFIX "metric.hash.index.block.read"
#define HASH_INDEX_METRIC_WRITE_PREFIX "metric.hash.index.block.write"

#define HASH_INDEX_VERSION_MAJOR ((uint16_t) 0)
#define HASH_INDEX_VERSION_MINOR ((uint16_t) 1)

#define HASH_INDEX_ERROR_NO_SPACE 255

#define HASH_UPDATE_NONE 0
#define HASH_UPDATE_WRITE 1
#define HASH_UPDATE_UPDATE 2
#define HASH_UPDATE_DELETE 4

REACTFS_NS_CORE
                struct __hash_index_header_v0__ {
                    uint32_t block_seed = 0;
                    uint16_t bucket_prime = 0;
                    uint32_t bucket_count = 0;
                    uint16_t key_size = 0;
                    uint32_t overflow_count = 0;
                    uint64_t overflow_offset = 0;
                    uint32_t overflow_write_index = 0;
                };

                typedef struct __rollback_record__ {
                    uint8_t update_type = HASH_UPDATE_NONE;
                    uint64_t bucket_offset = 0;
                    uint8_t bucket_cell = 0;
                } __rollback_record;

                typedef struct __write_rollback__ : __rollback_record {
                    bool has_next_ptr = false;
                    uint64_t next_ptr_offset = 0;
                } __write_rollback;

                typedef struct __delete_rollback__ : __rollback_record {
                    uint32_t hash_value = 0;
                } __delete_rollback;

                typedef struct __update_rollback__ : __rollback_record {
                    uint32_t hash_value = 0;
                    uint64_t data_offset = 0;
                    uint64_t data_size = 0;
                } __update_rollback;

                struct __hash_rollback_info_v0__ {
                    bool in_transaction = false;
                    string *transaction_id = nullptr;
                    uint64_t start_time = 0;
                    uint64_t overflow_offset = 0;
                    uint32_t overflow_write_index = 0;
                    vector<__rollback_record *> *updates = nullptr;
                };

                typedef __hash_index_header_v0__ __hash_index_header;
                typedef __hash_rollback_info_v0__ __hash_rollback_info;

                class typed_hash_index : public typed_index_base {
                protected:
                    __hash_index_header *hash_header = nullptr;
                    void *overflow = nullptr;
                    __hash_rollback_info *rollback_info = nullptr;

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

                    void *get_index_ptr(uint32_t bucket, uint32_t offset, uint64_t *base_offset) {
                        void *ptr = get_data_ptr();
                        uint64_t off = ((bucket * hash_header->bucket_prime) + offset) * hash_header->key_size;
                        POSTCONDITION(off < header->total_size);
                        *base_offset = off;
                        return buffer_utils::increment_data_ptr(ptr, off);
                    }

                    bool has_overflow_space() const {
                        if (in_transaction()) {
                            return (rollback_info->overflow_write_index < hash_header->overflow_count);
                        }
                        return (hash_header->overflow_write_index < hash_header->overflow_count);
                    }

                    uint64_t get_next_overflow_offset() {
                        PRECONDITION(in_transaction());
                        PRECONDITION(has_overflow_space());
                        uint16_t r_size = compute_record_size();

                        uint64_t offset = (r_size * rollback_info->overflow_write_index);
                        rollback_info->overflow_write_index++;

                        return offset;
                    }

                    bool in_transaction() const {
                        if (NOT_NULL(rollback_info)) {
                            if (rollback_info->in_transaction) {
                                if (NOT_EMPTY_P(rollback_info->transaction_id)) {
                                    return true;
                                }
                            }
                        }
                        return false;
                    }

                    bool in_transaction(string txid) {
                        if (in_transaction()) {
                            return (txid == *rollback_info->transaction_id);
                        }
                        return false;
                    }

                    void setup_transaction(string tnx_id) {
                        PRECONDITION(!in_transaction());
                        CHECK_NOT_EMPTY(tnx_id);

                        if (IS_NULL(rollback_info)) {
                            rollback_info = (__hash_rollback_info *) malloc(sizeof(__hash_rollback_info));
                            CHECK_ALLOC(rollback_info, TYPE_NAME(__hash_rollback_info));

                            rollback_info->transaction_id = new string();
                        }
                        rollback_info->in_transaction = true;
                        rollback_info->transaction_id->assign(tnx_id);
                        rollback_info->start_time = time_utils::now();
                        rollback_info->overflow_write_index = hash_header->overflow_write_index;
                        rollback_info->overflow_offset = hash_header->overflow_offset;
                    }

                    void free_rollback_data() {
                        if (NOT_NULL(rollback_info)) {
                            rollback_info->in_transaction = false;
                            rollback_info->transaction_id->clear();

                            if (NOT_EMPTY_P(rollback_info->updates)) {
                                for (__rollback_record *wr : *(rollback_info->updates)) {
                                    FREE_PTR(wr);
                                }
                                rollback_info->updates->clear();
                            }
                            rollback_info->overflow_write_index = 0;
                            rollback_info->overflow_offset = 0;
                        }
                    }

                    __write_rollback *get_write_rollback() {
                        PRECONDITION(in_transaction());

                        __write_rollback *wr = (__write_rollback *) malloc(sizeof(__write_rollback));
                        CHECK_ALLOC(wr, TYPE_NAME(__write_rollback));
                        wr->update_type = HASH_UPDATE_WRITE;

                        if (IS_NULL(rollback_info->updates)) {
                            rollback_info->updates = new vector<__rollback_record *>();
                            CHECK_ALLOC(rollback_info->updates, TYPE_NAME(vector));
                        }
                        rollback_info->updates->push_back(wr);

                        return wr;
                    }


                    __update_rollback *get_update_rollback() {
                        PRECONDITION(in_transaction());

                        __update_rollback *ur = (__update_rollback *) malloc(sizeof(__update_rollback));
                        CHECK_ALLOC(ur, TYPE_NAME(__update_rollback));

                        if (IS_NULL(rollback_info->updates)) {
                            rollback_info->updates = new vector<__rollback_record *>();
                            CHECK_ALLOC(rollback_info->updates, TYPE_NAME(vector));
                        }
                        rollback_info->updates->push_back(ur);

                        return ur;
                    }

                    __delete_rollback *get_delete_rollback() {
                        PRECONDITION(in_transaction());

                        __delete_rollback *dr = (__delete_rollback *) malloc(sizeof(__delete_rollback));
                        CHECK_ALLOC(dr, TYPE_NAME(__delete_rollback));

                        if (IS_NULL(rollback_info->updates)) {
                            rollback_info->updates = new vector<__rollback_record *>();
                            CHECK_ALLOC(rollback_info->updates, TYPE_NAME(vector));
                        }
                        rollback_info->updates->push_back(dr);

                        return dr;
                    }

                    __typed_index_record *
                    __write_index(uint32_t hash, __index_key_set *index, uint64_t offset, uint64_t size,
                                  uint8_t *error);

                    __typed_index_read *__read_index(uint32_t hash, __index_key_set *index, uint8_t rec_state);

                    void commit_change(__rollback_record *record) {
                        if (record->update_type == HASH_UPDATE_WRITE) {
                            __write_rollback *r = static_cast<__write_rollback *>(record);
                            commit_write_change(r);
                        } else if (record->update_type == HASH_UPDATE_UPDATE) {
                            __update_rollback *r = static_cast<__update_rollback *>(record);
                            commit_update_change(r);
                        } else if (record->update_type == HASH_UPDATE_DELETE) {
                            __delete_rollback *r = static_cast<__delete_rollback *>(record);
                            commit_delete_change(r);
                        } else {
                            throw FS_BASE_ERROR("Invalid record change type. [type=%d]", record->update_type);
                        }
                    }

                    void write_rollback(__write_rollback *record) {
                        CHECK_NOT_NULL(record);
                        void *ptr = get_data_ptr();
                        __typed_index_bucket ib(this->index_def);
                        ib.read(ptr, record->bucket_offset, hash_header->key_size);
                        if (record->has_next_ptr) {
                            ib.write_extend_rollback();
                        } else {
                            ib.write_rollback(record->bucket_cell, hash_header->key_size);
                        }
                    }

                    void commit_write_change(__write_rollback *record) {
                        CHECK_NOT_NULL(record);
                        void *ptr = get_data_ptr();
                        __typed_index_bucket ib(this->index_def);
                        ib.read(ptr, record->bucket_offset, hash_header->key_size);
                        if (!record->has_next_ptr) {
                            ib.write_commit(record->bucket_cell, hash_header->key_size);
                        }
                    }

                    void commit_update_change(__update_rollback *record) {
                        CHECK_NOT_NULL(record);
                        void *ptr = get_data_ptr();
                        __typed_index_bucket ib(this->index_def);
                        ib.read(ptr, record->bucket_offset, hash_header->key_size);
                        ib.update(record->hash_value, record->bucket_cell, record->data_offset, record->data_size,
                                  hash_header->key_size);
                    }

                    void commit_delete_change(__delete_rollback *record) {
                        CHECK_NOT_NULL(record);
                        void *ptr = get_data_ptr();
                        __typed_index_bucket ib(this->index_def);
                        ib.read(ptr, record->bucket_offset, hash_header->key_size);
                        ib.remove(record->hash_value, record->bucket_cell, hash_header->key_size);
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

                        if (NOT_NULL(rollback_info)) {
                            free_rollback_data();
                            CHECK_AND_FREE(rollback_info->updates);
                            CHECK_AND_FREE(rollback_info->transaction_id);
                        }
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
                    * Check if this index block can be written to.
                    *
                    * @return - Index can be written to?
                    */
                    virtual const bool has_write_space() const override;

                    /*!
                     * Create a new index record for the specified index and offset.
                     *
                     * @param index - Record sequence index.
                     * @param offset - Read offset in the data file.
                     * @param size - Size of the data record in bytes.
                     * @param transaction_id - Current transaction ID.
                     * @return - Created index pointer.
                     */
                    const __typed_index_record *
                    write_index(__index_key_set *index, uint64_t offset, uint64_t size, string transaction_id) override;

                    /*!
                     * Read the index record for the specified index.
                     *
                     * @param index - Data record index.
                     * @param rec_state - Allow to read dirty/deleted records?
                     * @return - Index record pointer.
                     */
                    const __typed_index_record *
                    read_index(__index_key_set *index, uint8_t rec_state = BLOCK_RECORD_STATE_READABLE) override;

                    /*!
                     * Close this instance of the block index.
                     */
                    void close() override;

                    void sync(bool recreate = false) override;

                    static bool update_rollback_compare(__rollback_record *w1, __rollback_record *w2) {
                        if (IS_NULL(w1) && NOT_NULL(w2)) {
                            return true;
                        } else if (NOT_NULL(w1) && IS_NULL(w2)) {
                            return false;
                        } else if (IS_NULL(w1) && IS_NULL(w1)) {
                            return true;
                        }
                        if (w1->bucket_offset < w2->bucket_offset) {
                            return true;
                        } else if (w1->bucket_offset > w2->bucket_offset) {
                            return false;
                        }
                        return true;
                    }
                };
REACTFS_NS_CORE_END

#endif //REACTFS_TYPED_HASH_INDEX_H
