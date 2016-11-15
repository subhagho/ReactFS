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
#include "common/includes/exclusive_lock.h"

#include "fmstream.h"
#include "common_structs.h"
#include "fs_error_base.h"
#include "compression.h"
#include "compression_factory.h"

#define DEFAULT_LOCK_RETRY_INTERVAL 30

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {

                /*!
                 * Base data block class. This class is responsible for managing read/write of data into the memory mapped
                 * files. The format of the binary file is:
                 * <HEADER: __block_header>[<RECORD: <HEADER:__record_header><BYTES: DATA>>...]
                 *
                 * The class is defined as a template type to facilitate typed read/write funtions. The data representation
                 * is in bytes and hence type agnostic.
                 *
                 */
                class base_block {
                protected:
                    /// State of this instance of the block object.
                    __state__ state;

                    /// Backing file path for the memory mapped file.
                    string filename;

                    /// Shared lock for writing
                    exclusive_lock *block_lock = nullptr;

                    /// Memory-mapped file handle.
                    fmstream *stream = nullptr;

                    /// Block header pointer.
                    __block_header *header = nullptr;

                    /// Current (committed) write pointer
                    void *write_ptr = nullptr;

                    /// Base pointer for the mapped data.
                    void *base_ptr = nullptr;

                    /// Rollback information (in case a write transaction is in progress)
                    __rollback_info *rollback_info = nullptr;

                    /// Index pointer to load the data record
                    __record_index *index_ptr = nullptr;

                    /// Compression handler instance (in case compression is turned on)
                    compression_handler *compression = nullptr;

                    /*!
                     * Create a new file backed data block.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param filename - Backing filename for this data block.
                     * @param block_type - Type of this data block. (PRIMARY or REPLICATED)
                     * @param record_type - Record type of the data stored in this block.
                     * @param block_size - Maximum data size allowed for this block.
                     * @param overwrite - Overwrite if data block file already exists?
                     * @return - UUID of the new block created.
                     */
                    string __create_block(uint64_t block_id, string filename, __block_type block_type,
                                          __block_record_type record_type, uint64_t block_size, bool overwrite);

                    /*!
                     * Open a new instance of the specified data block.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param filename - Backing filename for this data block.
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    void *__open_block(uint64_t block_id, string filename);

                    /*!
                     * Read (N) records from this block.
                     *
                     * @param index - Start index of the record.
                     * @param count - Number of records to read. (default = 1)
                     * @return - Vector of record pointers.
                     */
                    vector<__record *> __read_records(uint64_t index, uint32_t count = 1);

                    /*!
                     * Create a new data record and save it to the backing file.
                     *
                     * @param source - Source address to copy the data bytes from.
                     * @param size - Size of the record data.
                     * @param uncompressed_size - In case of compression, the size of the record before compression.
                     * @return - Record pointer of the added record.
                     */
                    __record *
                    __write_record(void *source, uint32_t size, string transcation_id, uint32_t uncompressed_size);

                    /*!
                     * Close this data block instance.
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
                        return (cptr + sizeof(__block_header));
                    }

                    /*!
                     * Check if there is a write transaction in progress.
                     *
                     * @return - Is transaction in progress?
                     */
                    bool in_transaction() const {
                        return NOT_NULL(rollback_info);
                    }

                    /*!
                     * Setup a new transaction context.
                     *
                     * @return - New transaction ID.
                     */
                    string setup_transaction() {
                        CHECK_AND_FREE(rollback_info);

                        rollback_info = (__rollback_info *) malloc(sizeof(__rollback_info));
                        CHECK_NOT_NULL(rollback_info);
                        rollback_info->last_index = header->last_index;
                        rollback_info->start_time = time_utils::now();
                        rollback_info->transaction_id = string(common_utils::uuid());

                        return rollback_info->transaction_id;
                    }

                    /*!
                     * Get the write pointer from where the next record should be writter.
                     *
                     * @return - Write pointer.
                     */
                    void *get_write_ptr() {
                        BYTE_PTR wptr = static_cast<BYTE_PTR >(write_ptr);
                        if (in_transaction()) {
                            return (wptr + rollback_info->write_offset);
                        } else {
                            return (wptr + header->write_offset);
                        }
                    }

                    /*!
                     * Check if there is space available to write a data record of the size specified.
                     *
                     * @param size - Data record size required.
                     * @return - Is space available?
                     */
                    bool has_space(uint32_t size) {
                        uint32_t free_space = get_free_space();
                        return (free_space >= size);
                    }

                    /*!
                     * Get the index for the record being added.
                     *
                     * @return - Next record index.
                     */
                    uint64_t get_next_index() {
                        if (in_transaction()) {
                            return rollback_info->last_index++;
                        }
                        return header->last_index++;
                    }

                    /*!
                     * Allocate an index pointer for a new record being written.
                     *
                     * @return - New index pointer.
                     */
                    __record_index *get_next_index_ptr() {
                        __record_index *ptr = (__record_index *) malloc(sizeof(__record_index));
                        CHECK_NOT_NULL(ptr);

                        ptr->index = get_next_index();
                        ptr->readable = false;
                        ptr->next = nullptr;

                        if (IS_NULL(index_ptr)) {
                            index_ptr = ptr;
                        } else {
                            uint32_t offset = (ptr->index - header->start_index);
                            __record_index *last = (index_ptr + (offset - 1));
                            CHECK_NOT_NULL(last);
                            last->next = ptr;
                        }

                        return ptr;
                    }

                public:
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
                    uint32_t get_free_space() const {
                        CHECK_STATE_AVAILABLE(state);
                        if (in_transaction()) {
                            return (header->block_size - (header->used_bytes + rollback_info->used_bytes));
                        }
                        return (header->block_size - header->used_bytes);
                    }

                    /*!
                     * Get the space currently used by this block.
                     *
                     * @return - Space used (in bytes).
                     */
                    uint32_t get_used_space() const {
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
                    __block_type get_block_type() const {
                        CHECK_STATE_AVAILABLE(state);
                        return header->block_type;
                    }

                    /*!
                     * Start a new write transcation on this block. If block is currently locked for write,
                     * then the call will wait till the block is available.
                     *
                     * @return - Transaction UUID.
                     */
                    string start_transaction() {
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
                     * Start a new write transcation on this block. If block is currently locked for write,
                     * then the call will wait till the block is available or the specified timeout is reached.
                     *
                     * @return - Transaction UUID.
                     */
                    string start_transaction(uint64_t timeout);

                    /*!
                     * Commit the current transcation.
                     *
                     * @param transaction_id - Transaction ID obtained via a start_transaction call.
                     */
                    void commit(string transaction_id);

                    /*!
                     * Rollback the current transcation.
                     *
                     * @param transaction_id - Transaction ID obtained via a start_transaction call.
                     */
                    void rollback(string transaction_id);

                    /*!
                     * Force a transaction rollback.
                     *
                     * Should be used only in exception cases, such as transaction timeout.
                     */
                    void force_rollback();

                    /*!
                     * Create a new instance of a raw data block.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param filename - Backing filename for this data block.
                     * @param block_type - Type of this data block. (PRIMARY or REPLICATED)
                     * @param block_size - Maximum data size allowed for this block.
                     * @param overwrite - Overwrite if data block file already exists?
                     * @return - UUID of the new block created.
                     */
                    string create(uint64_t block_id, string filename, __block_type block_type,
                                  uint64_t block_size, bool overwrite) {
                        string uuid = __create_block(block_id, filename, block_type, __block_record_type::RAW,
                                                     block_size, overwrite);
                        close();

                        POSTCONDITION(!IS_EMPTY(uuid));

                        return uuid;
                    }

                    /*!
                     * Open a new instance of the specified data block.
                     *
                     * @param block_id - Unique block id for this data block.
                     * @param filename - Backing filename for this data block.
                     * @return - Base pointer of the memory-mapped buffer.
                     */
                    void open(uint64_t block_id, string filename) {
                        void *ptr = __open_block(block_id, filename);
                        POSTCONDITION(NOT_NULL(ptr));
                        BYTE_PTR bptr = static_cast<BYTE_PTR>(ptr);
                        bptr = (bptr + sizeof(__block_header));

                        if (header->write_state == __write_state::WRITABLE) {
                            write_ptr = (bptr + header->write_offset);
                        }

                        state.set_state(__state_enum::Available);
                    }


                    /*!
                     * Write a data record to the block. Check should be done to ensure that
                     * enough space is available in the block for the required data length, else
                     * the call will throw an exception.
                     *
                     * @param source - Source buffer to copy data from.
                     * @param length - Length of data to copy.
                     * @param transaction_id - Transaction ID.
                     * @return - Length of data written.
                     */
                    uint32_t write(void *source, uint32_t length, string transaction_id);

                    /*!
                     * Read a set of records from the block starting at the specified record index. If count exceeds the
                     * number of available record, only the set of available records will be returned.
                     *
                     * @param index - Start record index.
                     * @param count - No. of records to read.
                     * @return - Vector of record pointers.
                     */
                    vector<shared_read_ptr> read(uint64_t index, uint32_t count);

                    const __state_enum get_block_state() const {
                        return state.get_state();
                    }

                    static string get_lock_name(uint64_t block_id) {
                        return common_utils::format("block_%lu_lock", block_id);
                    }
                };
            }
        }
    }
}

string
com::wookler::reactfs::core::base_block::__create_block(uint64_t block_id, string filename, __block_type block_type,
                                                        __block_record_type record_type, uint64_t block_size,
                                                        bool overwrite) {
    try {
        Path p(filename);
        if (p.exists()) {
            if (!overwrite) {
                throw FS_BASE_ERROR("File with specified path already exists. [path=%s]", filename.c_str());
            } else {
                if (!p.remove()) {
                    throw FS_BASE_ERROR("Error deleting existing file. [file=%s]", filename.c_str());
                }
            }
        }
        uint64_t ts = sizeof(__block_header) + block_size;

        stream = new fmstream(filename.c_str(), ts);
        CHECK_NOT_NULL(stream);
        base_ptr = stream->data();

        memset(base_ptr, 0, ts);

        header = static_cast<__block_header *>(base_ptr);
        header->block_id = block_id;

        string uuid = common_utils::uuid();
        memcpy(header->block_uid, uuid.c_str(), uuid.length());

        header->block_size = block_size;
        header->create_time = time_utils::now();
        header->update_time = header->create_time;
        header->block_type = block_type;
        header->record_type = record_type;
        header->write_state = __write_state::WRITABLE;
        header->compression.compressed = false;
        header->compression.type = __compression_type::NO_COMPRESSION;
        header->encryption.encrypted = false;
        header->encryption.type = __encryption_type::NO_ENCRYPTION;

        string lock_name = get_lock_name(header->block_id);
        block_lock = new exclusive_lock(&lock_name);
        block_lock->create();
        block_lock->reset();

        state.set_state(__state_enum::Available);

        return uuid;
    } catch (const exception &e) {
        fs_error_base err = FS_BASE_ERROR("Error creating block. [block id=%lu][filename=%s][error=%s]", block_id,
                                          filename.c_str(), e.what());
        state.set_error(&err);
        throw err;
    } catch (...) {
        fs_error_base err = FS_BASE_ERROR("Error creating block. [block id=%lu][filename=%s][error=%s]", block_id,
                                          filename.c_str(), "UNKNOWN ERROR");
        state.set_error(&err);
        throw err;
    }
}

void *com::wookler::reactfs::core::base_block::__open_block(uint64_t block_id, string filename) {
    try {
        Path p(filename);
        if (!p.exists()) {
            throw FS_BASE_ERROR("File not found. [path=%s]", filename.c_str());
        }
        stream = new fmstream(filename.c_str());
        CHECK_NOT_NULL(stream);
        base_ptr = stream->data();

        header = static_cast<__block_header *>(base_ptr);
        PRECONDITION(header->block_id == block_id);

        if (IS_NULL(block_lock)) {
            string lock_name = get_lock_name(header->block_id);
            block_lock = new exclusive_lock(&lock_name);
            block_lock->create();
        }
        this->filename = string(filename);

        if (header->compression.compressed) {
            compression = compression_factory::get_compression_handler(header->compression.type);
            CHECK_NOT_NULL(compression);
        }
        return base_ptr;
    } catch (const exception &e) {
        fs_error_base err = FS_BASE_ERROR("Error creating block. [block id=%lu][filename=%s][error=%s]", block_id,
                                          filename.c_str(), e.what());
        state.set_error(&err);
        throw err;
    } catch (...) {
        fs_error_base err = FS_BASE_ERROR("Error creating block. [block id=%lu][filename=%s][error=%s]", block_id,
                                          filename.c_str(), "UNKNOWN ERROR");
        state.set_error(&err);
        throw err;
    }
}

void com::wookler::reactfs::core::base_block::close() {
    CHECK_AND_DISPOSE(state);

    if (NOT_NULL(stream)) {
        if (stream->is_open()) {
            stream->close();
        }
        delete (stream);
        stream = nullptr;
    }

    if (NOT_NULL(block_lock)) {
        if (block_lock->is_locked()) {
            block_lock->release_lock();
        }
        CHECK_AND_FREE(block_lock);
    }
    header = nullptr;
    write_ptr = nullptr;
    base_ptr = nullptr;
}

__record *
com::wookler::reactfs::core::base_block::__write_record(void *source, uint32_t size, string transaction_id,
                                                        uint32_t uncompressed_size) {
    CHECK_STATE_AVAILABLE(state);
    CHECK_NOT_NULL(source);

    PRECONDITION(header->write_state == __write_state::WRITABLE);
    PRECONDITION(has_space(size));
    PRECONDITION(in_transaction());
    PRECONDITION(!IS_EMPTY(transaction_id) && rollback_info->transaction_id == transaction_id);

    void *w_ptr = get_write_ptr();

    __record_index *i_ptr = get_next_index_ptr();
    CHECK_NOT_NULL(i_ptr);

    __record *record = (__record *) malloc(sizeof(__record));
    CHECK_NOT_NULL(record);

    record->header = reinterpret_cast<__record_header *>(w_ptr);
    record->header->index = i_ptr->index;
    record->header->data_size = size;
    record->header->timestamp = time_utils::now();
    record->header->uncompressed_size = uncompressed_size;

    w_ptr = increment_data_ptr(w_ptr, sizeof(__record_header));
    record->data_ptr = w_ptr;

    memcpy(record->data_ptr, source, size);

    rollback_info->last_index = record->header->index;
    rollback_info->used_bytes += size;
    rollback_info->write_offset += sizeof(__record_header) + size;
    if (IS_NULL(rollback_info->start_index)) {
        rollback_info->start_index = i_ptr;
    }

    i_ptr->read_ptr = record;

    return i_ptr->read_ptr;
}

vector<__record *> com::wookler::reactfs::core::base_block::__read_records(uint64_t index, uint32_t count) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(index >= header->start_index && index <= header->last_index);

    vector<__record *> result(count);
    uint32_t offset = (index - header->start_index);
    for (int ii = 0; ii < count; ii++) {
        if (offset + ii > header->last_index)
            break;
        __record_index *ptr = (index_ptr + (offset + ii));
        CHECK_NOT_NULL(ptr);
        PRECONDITION(ptr->readable);
        result[ii] = ptr->read_ptr;
    }
    return result;
}

string com::wookler::reactfs::core::base_block::start_transaction(uint64_t timeout) {
    CHECK_STATE_AVAILABLE(state);
    string txid = EMPTY_STRING;
    if (timeout <= 0) {
        if (block_lock->wait_lock()) {
            txid = setup_transaction();
        } else {
            throw FS_BASE_ERROR("Error getting wait lock. [lock=%s]", block_lock->get_name()->c_str());
        }
    } else {
        uint64_t startt = time_utils::now();
        while (true) {
            if (block_lock->try_lock()) {
                txid = setup_transaction();
                break;
            }
            START_ALARM(DEFAULT_LOCK_RETRY_INTERVAL);
            if ((time_utils::now() - startt) > timeout) {
                break;
            }
        }
    }
    return txid;
}

uint32_t com::wookler::reactfs::core::base_block::write(void *source, uint32_t length, string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    CHECK_NOT_NULL(source);

    temp_buffer buffer;
    bool use_buffer = false;

    uint32_t uncompressed_size = 0;

    if (is_compressed()) {
        use_buffer = true;
        CHECK_NOT_NULL(compression);

        uncompressed_size = length;
        compression->write_archive_data(source, length, &buffer);
    }
    if (is_encrypted()) {
        use_buffer = true;
        // TODO : Implement encryption handlers.
    }


    void *data_ptr = (use_buffer ? buffer.get_ptr() : source);
    uint32_t size = (use_buffer ? buffer.get_used_size() : length);

    __record *r_ptr = __write_record(data_ptr, size, transaction_id, uncompressed_size);

    return r_ptr->header->data_size;
}

vector<shared_read_ptr> com::wookler::reactfs::core::base_block::read(uint64_t index, uint32_t count) {
    CHECK_STATE_AVAILABLE(state);

    vector<shared_read_ptr> result;
    vector<__record *> r = __read_records(index, count);
    if (is_encrypted() || is_compressed()) {
        temp_buffer *buffer = new temp_buffer();
        temp_buffer *writebuff = nullptr;

        if (r.size() > 0) {
            for (int ii = 0; ii < r.size(); ii++) {
                __record *ptr = r[ii];
                bool use_buffer = false;
                CHECK_NOT_NULL(ptr);
                if (is_encrypted()) {
                    use_buffer = true;
                    // TODO : Implement encryption handlers.
                }
                if (is_compressed()) {
                    CHECK_NOT_NULL(compression);

                    void *data_ptr = (use_buffer ? buffer->get_ptr() : ptr->data_ptr);
                    uint32_t data_size = (use_buffer ? buffer->get_used_size() : ptr->header->data_size);

                    if (use_buffer && IS_NULL(writebuff)) {
                        writebuff = new temp_buffer();
                    }
                    temp_buffer *buff = (use_buffer ? writebuff : buffer);
                    int ret = compression->read_archive_data(data_ptr, ptr->header->uncompressed_size, buff);
                    POSTCONDITION(ret > 0);

                    shared_read_ptr s_ptr = make_shared<__read_ptr>(buff->get_used_size());
                    (*s_ptr).copy(buff->get_ptr());

                    result[ii] = s_ptr;
                }
            }
        }
        CHECK_AND_FREE(buffer);
        CHECK_AND_FREE(writebuff);
    } else {
        for (int ii = 0; ii < r.size(); ii++) {
            __record *ptr = r[ii];
            shared_read_ptr s_ptr = make_shared<__read_ptr>(ptr->header->data_size);
            (*s_ptr).set_data_ptr(ptr->data_ptr);

            result[ii] = s_ptr;
        }
    }
    return result;
}

void com::wookler::reactfs::core::base_block::commit(string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(in_transaction());
    PRECONDITION(!IS_EMPTY(transaction_id) && rollback_info->transaction_id == transaction_id);

    header->last_index = rollback_info->last_index;
    header->used_bytes += rollback_info->used_bytes;
    header->write_offset += rollback_info->write_offset;

    __record_index *iptr = rollback_info->start_index;
    while (NOT_NULL(iptr)) {
        iptr->readable = true;
        iptr = iptr->next;
    }
    FREE_PTR(rollback_info);

    block_lock->release_lock();
}

void com::wookler::reactfs::core::base_block::rollback(string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(in_transaction());
    PRECONDITION(!IS_EMPTY(transaction_id) && rollback_info->transaction_id == transaction_id);

    force_rollback();
}

void com::wookler::reactfs::core::base_block::force_rollback() {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(in_transaction());

    __record_index *iptr = rollback_info->start_index;
    while (NOT_NULL(iptr)) {
        __record_index *ptr = iptr;
        iptr = iptr->next;
        FREE_PTR(ptr);
    }
    FREE_PTR(rollback_info);

    block_lock->release_lock();
}

#endif //REACTFS_BASE_BLOCK_H_H
