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
// Created by Subhabrata Ghosh on 14/11/16.
//

#ifndef REACTFS_BASE_BLOCK_H_H
#define REACTFS_BASE_BLOCK_H_H

#include "common/includes/common.h"
#include "common/includes/__state__.h"
#include "common/includes/log_utils.h"
#include "common/includes/common_utils.h"
#include "common/includes/file_utils.h"
#include "common/includes/timer.h"
#include "common/includes/__alarm.h"
#include "common/includes/shared_lock_utils.h"
#include "common/includes/mapped_data.h"
#include "common/includes/buffer_utils.h"
#include "common/includes/metrics.h"

#include "common_structs.h"
#include "fs_error_base.h"
#include "compression.h"
#include "compression_factory.h"
#include "base_block_index.h"
#include "clients/node_client_env.h"
#include "block_shared_defs.h"

#define BLOCK_VERSION_MAJOR ((uint16_t) 0)
#define BLOCK_VERSION_MINOR ((uint16_t) 1)

#define BLOCK_METRIC_READ_PREFIX "metric.block.read"
#define BLOCK_METRIC_WRITE_PREFIX "metric.block.write"

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;
using namespace com::wookler::reactfs::core::client;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {

                typedef struct __block_check_record__ {
                    uint64_t block_id;
                    string *block_uuid = nullptr;
                    uint64_t record_index_start;
                    uint64_t record_index_last;
                    uint64_t commit_sequence;
                } __block_check_record;

                /*!
                 * Base data block class. This class is responsible for managing read/write of data into the memory mapped
                 * files. The format of the binary file is:
                 * <HEADER: __block_header>[<RECORD: <HEADER:__record_header><BYTES: data>>...]
                 *
                 * The class is defined as a template type to facilitate typed read/write funtions. The data representation
                 * is in bytes and hence type agnostic.
                 *
                 */
                class base_block {
                protected:
                    __version_header version;

                    /// Mutex used for updating read usage stats.
                    mutex stat_lock;

                    //! State of this instance of the block object.
                    __state__ state;

                    //! Backing file path for the memory mapped file.
                    string filename;

                    //! Shared lock for writing
                    write_lock *block_lock = nullptr;

                    /// Memory-mapped file handle.
                    file_mapped_data *mm_data = nullptr;

                    /// Block header pointer.
                    __block_header *header = nullptr;

                    /// Current (committed) write pointer
                    void *write_ptr = nullptr;

                    /// Base pointer for the mapped data.
                    void *base_ptr = nullptr;

                    /// Block has been opened for updates.
                    bool updateable = false;

                    /// Rollback information (in case a write transaction is in progress)
                    __rollback_info *rollback_info = nullptr;

                    vector<uint64_t> rollback_info_deletes;

                    /// Compression handler instance (in case compression is turned on)
                    compression_handler *compression = nullptr;

                    __block_usage_stat write_usage;
                    __block_usage_stat read_usage;

                    void flush_write_usage() {
                        node_client_env *n_env = node_init_client::get_client_env();
                        mount_client *m_client = n_env->get_mount_client();
                        m_client->update_write_metrics(&filename, write_usage.bytes_used, write_usage.elapsed_time);
                        write_usage.bytes_used.store(0);
                        write_usage.elapsed_time.store(0);
                        write_usage.last_synced = time_utils::now();
                    }

                    void flush_read_usage() {
                        node_client_env *n_env = node_init_client::get_client_env();
                        mount_client *m_client = n_env->get_mount_client();
                        m_client->update_write_metrics(&filename, read_usage.bytes_used, read_usage.elapsed_time);
                        read_usage.bytes_used.store(0);
                        read_usage.elapsed_time.store(0);
                        read_usage.last_synced = time_utils::now();
                    }

                    void update_write_usage(uint64_t bytes, uint64_t elapsed) {
                        write_usage.bytes_used += bytes;
                        write_usage.elapsed_time += elapsed;
                        uint64_t delta = time_utils::now() - write_usage.last_synced;
                        if (delta > BLOCK_USAGE_FLUSH_INTERVAL) {
                            flush_write_usage();
                        }
                    }

                    void update_read_usage(uint64_t bytes, uint64_t elapsed) {
                        read_usage.bytes_used += bytes;
                        read_usage.elapsed_time += elapsed;
                        uint64_t delta = time_utils::now() - read_usage.last_synced;
                        if (delta > BLOCK_USAGE_FLUSH_INTERVAL) {
                            std::lock_guard<std::mutex> lock(stat_lock);
                            delta = time_utils::now() - read_usage.last_synced;
                            if (delta > BLOCK_USAGE_FLUSH_INTERVAL)
                                flush_write_usage();
                        }
                    }

                    /*!
                     * Create a new file backed data block.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param parent_id - Unique ID of the parent structure.
                     * @param filename - Backing filename for this data block.
                     * @param usage - Usage of this data block. (PRIMARY or REPLICATED)
                     * @param type - Type of this data block.
                     * @param block_size - Maximum data size allowed for this block.
                     * @param start_index - Starting record index for this block.
                     * @param overwrite - Overwrite if data block file already exists?
                     * @return - UUID of the new block created.
                     */
                    string __create_block(uint64_t block_id, uint64_t parent_id, string filename, __block_usage usage,
                                          __block_def type, uint64_t block_size, uint64_t start_index,
                                          bool overwrite);

                    /*!
                     * Open a new instance of the specified data block.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param filename - Backing filename for this data block.
                     * @param for_update - Open this block for update.
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    void *__open_block(uint64_t block_id, string filename, bool for_update);

                    /*!
                     * Read a record from this block at the specified index.
                     *
                     * @param index - Start index of the record.
                     * @param offset - Data file offset, read from the index.
                     * @param size - Size of the data record, read from the index.
                     * @return - Fetched record.
                     */
                    __record *__read_record(uint64_t index, uint64_t offset, uint64_t size);

                    /*!
                    * Read a record from this block at the specified index.
                    *
                    * @param index - Start index of the record.
                    * @return - Fetched record.
                    */
                    __record *__read_record(uint64_t index);

                    /*!
                     * Create a new data record and save it to the backing file.
                     *
                     * @param source - Source address to copy the data bytes from.
                     * @param size - Size of the record data.
                     * @param uncompressed_size - In case of compression, the size of the record before compression.
                     * @return - Record pointer of the added record.
                     */
                    __record *
                    __write_record(void *source, uint64_t size, string transcation_id, uint64_t uncompressed_size);

                    /*!
                     * Close this data block instance.
                     */
                    virtual void close();

                    /*!
                     * Get the base address pointing to where the block data starts.
                     *
                     * @return - Base data address
                     */
                    virtual void *get_data_ptr() {
                        return buffer_utils::increment_data_ptr(base_ptr, sizeof(__block_header));
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
                            CHECK_ALLOC(rollback_info, TYPE_NAME(__rollback_info));
                            rollback_info->transaction_id = new string();
                            CHECK_ALLOC(rollback_info->transaction_id, TYPE_NAME(string));
                        }

                        rollback_info->in_transaction = true;
                        rollback_info->last_index = header->last_index;
                        rollback_info->start_time = time_utils::now();
                        rollback_info->transaction_id->assign(txid);
                        rollback_info->write_offset = header->write_offset;
                        rollback_info->start_offset = header->write_offset;
                        rollback_info->start_index = nullptr;
                        rollback_info->used_bytes = 0;

                        return *rollback_info->transaction_id;
                    }

                    /*!
                     * End the currently active transaction session.
                     *
                     */
                    void end_transaction() {
                        string thread_id = thread_utils::get_current_thread();

                        PRECONDITION(block_lock->has_write_lock(thread_id));
                        CHECK_NOT_NULL(rollback_info);

                        rollback_info->in_transaction = false;
                        rollback_info->last_index = 0;
                        rollback_info->start_time = 0;
                        rollback_info->transaction_id->clear();
                        rollback_info->write_offset = 0;
                        rollback_info->start_index = nullptr;
                        rollback_info->used_bytes = 0;

                        block_lock->release_lock();
                    }

                    /*!
                     * Get the write pointer from where the next record should be writter.
                     *
                     * @return - Write pointer.
                     */
                    void *get_write_ptr() {
                        PRECONDITION(in_transaction());
                        return buffer_utils::increment_data_ptr(write_ptr, rollback_info->write_offset);
                    }

                    uint64_t get_write_offset() {
                        PRECONDITION(in_transaction());
                        return rollback_info->write_offset;
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
                     * Get the index for the record being added.
                     *
                     * @return - Next record index.
                     */
                    uint64_t get_next_index() {
                        PRECONDITION(in_transaction());
                        return rollback_info->last_index++;
                    }

                    /*!
                     * Is this record currently readable or an uncommitted record.?
                     *
                     * @param record - Record to check.
                     * @return - Is valid?
                     */
                    bool is_record_in_valid_state(__record *record) {
                        CHECK_NOT_NULL(record);
                        return (record->header->state == __record_state::R_READABLE ||
                                record->header->state == __record_state::R_DIRTY);
                    }

                    /*!
                     * Get the last index written to the block.
                     *
                     * @return - Last written index.
                     */
                    uint64_t get_current_index() {
                        if (in_transaction()) {
                            return rollback_info->last_index;
                        }
                        return header->last_index;
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
                    __record *write_record(void *source, uint32_t length, string transaction_id);

                    virtual void process_record_data(__record *ptr, temp_buffer *buffer,
                                                     temp_buffer *writebuff,
                                                     vector<shared_read_ptr> *data);

                    string get_metric_name(const string name) {
                        if (NOT_NULL(header)) {
                            return common_utils::format("%s.%lu", name.c_str(), header->block_id);
                        }
                        return common_consts::EMPTY_STRING;
                    }

                public:
                    base_block() {
                        version.major = BLOCK_VERSION_MAJOR;
                        version.minor = BLOCK_VERSION_MINOR;
                        write_usage.bytes_used.store(0);
                        write_usage.elapsed_time.store(0);
                        write_usage.last_synced = time_utils::now();
                        read_usage.bytes_used.store(0);
                        read_usage.elapsed_time.store(0);
                        read_usage.last_synced = time_utils::now();
                    }

                    /*!< destructor
                     * Base class destructor.
                     */
                    virtual ~base_block() {
                        DUMP_METRIC(get_metric_name(BLOCK_METRIC_READ_PREFIX));
                        DUMP_METRIC(get_metric_name(BLOCK_METRIC_WRITE_PREFIX));

                        REMOVE_METRIC(get_metric_name(BLOCK_METRIC_READ_PREFIX));
                        REMOVE_METRIC(get_metric_name(BLOCK_METRIC_WRITE_PREFIX));

                        this->close();
                    }

                    /*!
                     * Get the absolute file path of the backing file.
                     *
                     * @return - Backing file path.
                     */
                    string get_filename() const {
                        return filename;
                    }

                    /*!
                     * Get the available free space that this block has.
                     *
                     * @return - Space available (in bytes).
                     */
                    uint64_t get_free_space() const {
                        CHECK_STATE_AVAILABLE(state);
                        if (in_transaction()) {
                            uint64_t used = (header->used_bytes + rollback_info->used_bytes + sizeof(__record_header));
                            if (header->block_size > used)
                                return (header->block_size - used);
                        }
                        uint64_t used = (header->used_bytes + sizeof(__record_header));
                        if (header->block_size > used)
                            return (header->block_size - used);
                        return 0;
                    }

                    /*!
                     * Get the space currently used by this block.
                     *
                     * @return - Space used (in bytes).
                     */
                    uint64_t get_used_space() const {
                        CHECK_STATE_AVAILABLE(state);
                        if (in_transaction()) {
                            return (header->used_bytes + rollback_info->used_bytes);
                        }
                        return header->used_bytes;
                    }

                    /*!
                     * Check if the data in this block is compressed.
                     *
                     * @return - Is data compressed?
                     */
                    bool is_compressed() const {
                        CHECK_STATE_AVAILABLE(state);
                        return header->compression.compressed;
                    }

                    /*!
                     * Get the compression type, in case of data compression.
                     *
                     * @return - Data compression type.
                     */
                    __compression_type get_compression_type() const {
                        CHECK_STATE_AVAILABLE(state);
                        return header->compression.type;
                    }

                    /*!
                     * Check if the block data is encrypted.
                     *
                     * @return - Is encrypted?
                     */
                    bool is_encrypted() {
                        CHECK_STATE_AVAILABLE(state);
                        return header->encryption.encrypted;
                    }

                    /*!
                     * Set the block data compression type.
                     *
                     * @param type - Data compression type.
                     * @return - Self pointer.
                     */
                    base_block *with_compression_type(__compression_type type) {
                        CHECK_STATE_AVAILABLE(state);

                        if (type != __compression_type::NO_COMPRESSION)
                            header->compression.compressed = true;
                        else
                            header->compression.compressed = false;
                        header->compression.type = type;

                        return this;
                    }

                    /*!
                     * Check if the data in this block is encrypted.
                     *
                     * @return - Is data encrypted.
                     */
                    __encryption_type get_encryption_type() {
                        CHECK_STATE_AVAILABLE(state);

                        return header->encryption.type;
                    }

                    /*!
                     * Set the data encryption type of this block.
                     *
                     * @param type - Encryption type.
                     * @return - Self pointer.
                     */
                    base_block *with_encryption_type(__encryption_type type) {
                        CHECK_STATE_AVAILABLE(state);

                        if (type != __encryption_type::NO_ENCRYPTION)
                            header->encryption.encrypted = true;
                        else
                            header->encryption.encrypted = false;

                        header->encryption.type = type;

                        return this;
                    }

                    /*!
                     * Get the TTL for records in this block.
                     *
                     * @return - TTL for records.
                     */
                    uint64_t get_block_ttl() const {
                        CHECK_STATE_AVAILABLE(state);

                        return header->block_ttl;
                    }

                    /*!
                     * Set the TTL for records in this block.
                     *
                     * @param ttl - TTL for records.
                     * @return - Self pointer.
                     */
                    base_block *with_block_ttl(uint64_t ttl) {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(ttl >= 0);
                        header->block_ttl = ttl;

                        return this;
                    }

                    /*!
                     * Get the created at time for this block instance.
                     *
                     * @return - Created at time.
                     */
                    uint64_t get_create_time() const {
                        CHECK_STATE_AVAILABLE(state);
                        return header->create_time;
                    }

                    /*!
                     * Get the last updated at time for this block. Updated at time is updated
                     * only when data is committed to the block.
                     *
                     * @return - Last updated at timestamp.
                     */
                    uint64_t get_update_time() const {
                        CHECK_STATE_AVAILABLE(state);
                        return header->update_time;
                    }

                    /*!
                     * Get the maximum data size for this block.
                     *
                     * @return - Maximum data size.
                     */
                    uint64_t get_block_size() const {
                        CHECK_STATE_AVAILABLE(state);
                        return header->block_size;
                    }

                    /*!
                     * Get the type of this block instance. (PRIMAIRY or REPLICA)
                     *
                     * @return - Block type.
                     */
                    __block_usage get_block_type() const {
                        CHECK_STATE_AVAILABLE(state);
                        return header->usage;
                    }

                    /*!
                     * Get the index of the last record committed to this block.
                     *
                     * @return - Last comitted record index.
                     */
                    uint64_t get_last_index() const {
                        CHECK_STATE_AVAILABLE(state);
                        return header->last_index;
                    }

                    /*!
                     * Get the first record index in this block;
                     *
                     * @return - First record index.
                     */
                    uint64_t get_start_index() const {
                        CHECK_STATE_AVAILABLE(state);
                        return header->start_index;
                    }

                    /*!
                     * Start a new write transcation on this block. If block is currently locked for write,
                     * then the call will wait till the block is available.
                     *
                     * @return - Transaction UUID.
                     */
                    virtual string start_transaction() {
                        return start_transaction(0);
                    }

                    /*!
                     * Check if there is an active transaction that has expired.
                     *
                     * @param timeout - Transaction exipry timeout.
                     * @return - Has expired transcation?
                     */
                    bool has_expired_transaction(uint64_t timeout) {
                        if (in_transaction()) {
                            if ((time_utils::now() - rollback_info->start_time) > timeout) {
                                return true;
                            }
                        }
                        return false;
                    }

                    /*!
                     * Close this block for further writes, updates can still be supported where applicable.
                     */
                    void finish() {
                        CHECK_STATE_AVAILABLE(state);
                        header->write_state = __write_state::UPDATEABLE;
                    }

                    /*!
                     * Close this block for any further writes/updates.
                     */
                    void archive() {
                        CHECK_STATE_AVAILABLE(state);
                        header->write_state = __write_state::CLOSED;
                    }

                    /*!
                     * Start a new write transcation on this block. If block is currently locked for write,
                     * then the call will wait till the block is available or the specified timeout is reached.
                     *
                     * @return - Transaction UUID.
                     */
                    virtual string start_transaction(uint64_t timeout, const string &tid = common_consts::EMPTY_STRING);

                    /*!
                     * Commit the current transcation.
                     *
                     * @param transaction_id - Transaction ID obtained via a start_transaction call.
                     */
                    virtual void commit(string transaction_id);

                    /*!
                     * Rollback the current transcation.
                     *
                     * @param transaction_id - Transaction ID obtained via a start_transaction call.
                     */
                    virtual void rollback(string transaction_id);

                    /*!
                     * Force a transaction rollback.
                     *
                     * Should be used only in exception cases, such as transaction timeout.
                     */
                    void force_rollback();

                    /*!
                     * Check if there is a write transaction in progress.
                     *
                     * @return - Is transaction in progress?
                     */
                    bool in_transaction() const {
                        if (NOT_NULL(rollback_info)) {
                            if (rollback_info->in_transaction) {
                                string thread_id = thread_utils::get_current_thread();
                                if (block_lock->has_write_lock(thread_id)) {
                                    return true;
                                } else if (block_lock->is_lock_reset()) {
                                    LOG_INFO("Lock has been reset. Re-registering lock instance. [name=%s]",
                                             block_lock->get_name().c_str());
                                    block_lock->check_and_register();
                                    if (block_lock->has_write_lock(thread_id))
                                        return true;
                                }
                            }
                        }
                        return false;
                    }

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
                                          bool overwrite) {
                        string uuid = __create_block(block_id, parent_id, filename, usage, __block_def::BASIC,
                                                     block_size, start_index, overwrite);
                        close();
                        POSTCONDITION(!IS_EMPTY(uuid));
                        return uuid;
                    }

                    /*!
                     * Open a new instance of the specified data block.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param filename - Backing filename for this data block.
                     * @param for_update - Open this block for update.
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    virtual void open(uint64_t block_id, string filename, bool for_update);

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
                    virtual uint64_t write(void *source, uint32_t length, string transaction_id);

                    /*!
                     * Delete the record at the specified index.
                     *
                     * @param index - Record index.
                     * @param transaction_id - Current transaction Id.
                     * @return - Is deleted?
                     */
                    virtual bool delete_record(uint64_t index, string transaction_id) {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(is_updateable());
                        PRECONDITION(in_transaction());
                        PRECONDITION(!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id));
                        PRECONDITION(index >= header->start_index && index <= get_current_index());

                        __record *ptr = __read_record(index);
                        if (IS_NULL(ptr)) {
                            throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_RECORD_NOT_FOUND,
                                                 "Record not found in block. [index=%lu]", index);
                        }
                        PRECONDITION(is_record_in_valid_state(ptr));
                        rollback_info_deletes.push_back(index);

                        CHECK_AND_FREE(ptr);

                        header->update_time = time_utils::now();

                        return true;
                    }

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
                                          __record_state r_state = __record_state::R_READABLE);

                    /*!
                     * Get the state of this block instance.
                     *
                     * @return - Instance state.
                     */
                    const __state_enum get_block_state() const {
                        return state.get_state();
                    }

                    /*!
                     * Check if this block is writeable.
                     *
                     * @return - Can write?
                     */
                    bool is_writeable() {
                        CHECK_STATE_AVAILABLE(state);
                        return (this->updateable && header->write_state == __write_state::WRITABLE);
                    }

                    /*!
                     * Block can  updated?
                     *
                     * @return - Can update?
                     */
                    bool is_updateable() {
                        CHECK_STATE_AVAILABLE(state);
                        return (header->write_state == __write_state::WRITABLE ||
                                (this->updateable && header->write_state == __write_state::UPDATEABLE));
                    }

                    /*!
                     * Validate the data sanity of this block.
                     */
                    virtual __block_check_record *check_block_sanity();


                    void dump_block_state() {
                        CHECK_STATE_AVAILABLE(state);
                        string ct = time_utils::get_time_string(header->create_time);
                        string ut = time_utils::get_time_string(header->update_time);

                        LOG_DEBUG("[block id=%lu][block uuid=%s][block state=%d][created at=%s][updated at=%s]",
                                  header->block_id,
                                  header->block_uid, header->state, ct.c_str(), ut.c_str());
                        LOG_DEBUG(
                                "[block id=%lu][used bytes=%lu][total records=%lu][deleted records=%lu]",
                                header->block_id, header->used_bytes, header->total_records,
                                header->deleted_records);
                    }

                    /*!
                     * Static utility funtion to get the lockname for the specified block id.
                     *
                     * @param parent_id - Parent ID
                     * @param block_id - Block ID
                     * @return - Lock name associated with this block.
                     */
                    static string get_lock_name(uint64_t block_id, char *block_uid) {
                        return common_utils::format("blk_%024lu", block_id);
                    }
                };
            }
        }
    }
}

#endif //REACTFS_BASE_BLOCK_H_H
