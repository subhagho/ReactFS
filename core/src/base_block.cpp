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
// Created by Subhabrata Ghosh on 24/11/16.
//

#include "core/includes/base_block.h"

using namespace com::wookler::reactfs::core;


string
com::wookler::reactfs::core::base_block::__create_block(uint64_t block_id, uint64_t parent_id, string filename,
                                                        __block_usage usage, __block_def type, uint64_t block_size,
                                                        uint64_t start_index, bool overwrite) {
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
        uint64_t b_size = (block_size * 110) / 100;

        uint64_t ts = sizeof(__block_header) + b_size;

        mm_data = new file_mapped_data(p.get_path(), ts, overwrite);
        CHECK_ALLOC(mm_data, TYPE_NAME(file_mapped_data));
        base_ptr = mm_data->get_base_ptr();

        header = static_cast<__block_header *>(base_ptr);
        header->block_id = block_id;
        header->parent_id = parent_id;
        string uuid = common_utils::uuid();
        memset(header->block_uid, 0, SIZE_UUID + 1);
        memcpy(header->block_uid, uuid.c_str(), uuid.length());
        LOG_DEBUG("Creating new block. [ID=%lu][UUID=%s]", header->block_id, header->block_uid);

        header->version.major = version.major;
        header->version.minor = version.minor;
        header->block_size = block_size;
        header->create_time = time_utils::now();
        header->update_time = header->create_time;
        header->usage = usage;
        header->type = type;
        header->write_state = __write_state::WRITABLE;
        header->compression.compressed = false;
        header->compression.type = __compression_type::NO_COMPRESSION;
        header->encryption.encrypted = false;
        header->encryption.type = __encryption_type::NO_ENCRYPTION;
        header->start_index = start_index;
        header->last_index = header->start_index;
        header->commit_sequence = 0;
        header->total_records = 0;
        header->deleted_records = 0;

        string lock_name = get_lock_name(header->block_id, header->block_uid);
        write_lock_client *env = shared_lock_utils::get()->get_w_client(BLOCK_LOCK_GROUP);

        block_lock = env->add_lock(lock_name);
        CHECK_NOT_NULL(block_lock);
        block_lock->reset();

        this->filename = string(filename);

        mm_data->flush();

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

void *com::wookler::reactfs::core::base_block::__open_block(uint64_t block_id, string filename, bool for_update) {
    Path p(filename);
    if (!p.exists()) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_BLOCK_FILE_NOT_FOUND, "File not found. [path=%s]",
                             filename.c_str());
    }
    mm_data = new file_mapped_data(p.get_path());
    CHECK_ALLOC(mm_data, TYPE_NAME(file_mapped_data));
    base_ptr = mm_data->get_base_ptr();

    header = static_cast<__block_header *>(base_ptr);
    if (header->block_id != block_id) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_BLOCK_COPRRUPTED,
                             "Block id read from block file does not match. [read block id=%lu][expected block id=%lu]",
                             header->block_id, block_id);
    }
    if (!version_utils::compatible(header->version, version)) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_BLOCK_DATA_VERSION,
                             "Block data structure version does not match. [read varion=%s][expected version=%s]",
                             version_utils::get_version_string(header->version).c_str(),
                             version_utils::get_version_string(version).c_str());
    }

    if (IS_NULL(block_lock) && for_update) {
        string lock_name = get_lock_name(header->block_id, header->block_uid);
        write_lock_client *env = shared_lock_utils::get()->get_w_client(BLOCK_LOCK_GROUP);

        block_lock = env->add_lock(lock_name);
        CHECK_NOT_NULL(block_lock);
        this->updateable = true;
    }
    this->filename = string(p.get_path());

    if (header->compression.compressed) {
        compression = compression_factory::get_compression_handler(header->compression.type);
    }

    bool r = metrics_utils::create_metric(get_metric_name(BLOCK_METRIC_WRITE_PREFIX), AverageMetric);
    POSTCONDITION(r);
    r = metrics_utils::create_metric(get_metric_name(BLOCK_METRIC_READ_PREFIX), AverageMetric);
    POSTCONDITION(r);

    return base_ptr;
}

void com::wookler::reactfs::core::base_block::close() {
    CHECK_AND_DISPOSE(state);
    flush_write_usage();
    flush_read_usage();

    CHECK_AND_FREE(mm_data);

    if (NOT_NULL(block_lock)) {
        string thread_id = thread_utils::get_current_thread();
        if (block_lock->has_write_lock(thread_id)) {
            block_lock->release_lock();
        }

        write_lock_client *env = shared_lock_utils::get()->get_w_client(BLOCK_LOCK_GROUP);
        env->remove_lock(block_lock->get_name());

        block_lock = nullptr;
    }
    if (NOT_NULL(rollback_info)) {
        CHECK_AND_FREE(rollback_info->transaction_id);
        FREE_PTR(rollback_info);
    }
    rollback_info = nullptr;
    header = nullptr;
    write_ptr = nullptr;
    base_ptr = nullptr;
}

__record *
com::wookler::reactfs::core::base_block::__write_record(void *source, uint64_t size, string transaction_id,
                                                        uint64_t uncompressed_size) {
    CHECK_STATE_AVAILABLE(state);
    CHECK_NOT_NULL(source);
    string m_name = get_metric_name(BLOCK_METRIC_WRITE_PREFIX);
    START_TIMER(m_name, 0);
    PRECONDITION(is_writeable());
    PRECONDITION(has_space(size));
    PRECONDITION(!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id));
    if (!in_transaction()) {
        force_rollback();
        throw FS_BASE_ERROR("Transaction is no longer valid. [transaction id=%s]", transaction_id.c_str());
    }
    void *w_ptr = get_write_ptr();

    nano_timer t;
    t.start();

    __record *record = (__record *) malloc(sizeof(__record));
    CHECK_ALLOC(record, TYPE_NAME(__record));


    record->header = static_cast<__record_header *>(w_ptr);
    memset(record->header, 0, sizeof(__record_header));
    record->header->index = get_next_index();
    record->header->offset = get_write_offset();
    record->header->data_size = size;
    record->header->timestamp = time_utils::now();
    record->header->uncompressed_size = uncompressed_size;
    record->header->state = __record_state::R_DIRTY;

    w_ptr = buffer_utils::increment_data_ptr(w_ptr, sizeof(__record_header));
    record->data_ptr = w_ptr;

    memcpy(record->data_ptr, source, size);

    rollback_info->used_bytes += size;
    rollback_info->write_offset += (sizeof(__record_header) + size);

    t.stop();

    uint64_t write_bytes = (sizeof(__record_header) + size);
    update_write_usage(write_bytes, t.get_elapsed());

    header->update_time = record->header->timestamp;

    END_TIMER(m_name, 0);
    return record;
}

__record *
com::wookler::reactfs::core::base_block::__read_record(uint64_t index, uint64_t offset, uint64_t size) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(index >= header->start_index && index <= header->last_index);
    string m_name = get_metric_name(BLOCK_METRIC_READ_PREFIX);
    START_TIMER(m_name, 0);

    void *ptr = get_data_ptr();
    void *rptr = buffer_utils::increment_data_ptr(ptr, offset);
    __record *record = (__record *) malloc(sizeof(__record));
    CHECK_ALLOC(record, TYPE_NAME(__record));
    record->header = static_cast<__record_header *>(rptr);
    rptr = buffer_utils::increment_data_ptr(rptr, sizeof(__record_header));
    record->data_ptr = rptr;

    POSTCONDITION(record->header->index == index);
    POSTCONDITION(record->header->offset == offset);
    POSTCONDITION(record->header->data_size == size);

    END_TIMER(m_name, 0);
    return record;
}

__record *
com::wookler::reactfs::core::base_block::__read_record(uint64_t index) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(index >= header->start_index && index <= header->last_index);
    string m_name = get_metric_name(BLOCK_METRIC_READ_PREFIX);
    START_TIMER(m_name, 0);

    void *ptr = get_data_ptr();
    uint64_t ii = header->start_index;
    while (ii < index) {
        __record_header *r_header = static_cast<__record_header *>(ptr);
        POSTCONDITION(r_header->index == ii);
        uint64_t offset = r_header->data_size + sizeof(__record_header);
        ptr = buffer_utils::increment_data_ptr(ptr, offset);
        ii++;
    }
    __record *record = (__record *) malloc(sizeof(__record));
    CHECK_ALLOC(record, TYPE_NAME(__record));
    record->header = static_cast<__record_header *>(ptr);
    ptr = buffer_utils::increment_data_ptr(ptr, sizeof(__record_header));
    record->data_ptr = ptr;

    POSTCONDITION(record->header->index == index);

    END_TIMER(m_name, 0);
    return record;
}

string com::wookler::reactfs::core::base_block::start_transaction(uint64_t timeout, const string &tid) {
    CHECK_STATE_AVAILABLE(state);
    string txid;
    char *user_name = getenv("USER");
    if (IS_NULL(user_name)) {
        user_name = getenv("USERNAME");
    }
    PRECONDITION(NOT_NULL(user_name));
    string uname(user_name);

    txid = block_lock->get_lock(uname, timeout, tid);
    if (!IS_EMPTY(txid)) {
        setup_transaction(txid);
    } else {
        throw FS_BASE_ERROR("Error getting wait lock. [lock=%s]", block_lock->get_name().c_str());
    }

    return txid;
}

__record *com::wookler::reactfs::core::base_block::write_record(void *source, uint32_t length, string transaction_id) {
    temp_buffer buffer;
    bool use_buffer = false;

    uint64_t uncompressed_size = 0;

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
    uint64_t size = (use_buffer ? buffer.get_used_size() : length);

    return __write_record(data_ptr, size, transaction_id, uncompressed_size);
}

uint64_t com::wookler::reactfs::core::base_block::write(void *source, uint32_t length, string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    CHECK_NOT_NULL(source);

    __record *r_ptr = write_record(source, length, transaction_id);
    uint64_t index = r_ptr->header->index;
    CHECK_AND_FREE(r_ptr);

    return index;
}

void com::wookler::reactfs::core::base_block::process_record_data(__record *ptr, temp_buffer *buffer,
                                                                  temp_buffer *writebuff,
                                                                  vector<shared_read_ptr> *data) {
    if (is_encrypted() || is_compressed()) {
        bool use_buffer = false;
        CHECK_NOT_NULL(buffer);
        CHECK_NOT_NULL(writebuff);
        CHECK_NOT_NULL(ptr);
        if (is_encrypted()) {
            use_buffer = true;
            // TODO : Implement encryption handlers.
        }
        if (is_compressed()) {
            CHECK_NOT_NULL(compression);

            void *data_ptr = (use_buffer ? buffer->get_ptr() : ptr->data_ptr);

            temp_buffer *buff = (use_buffer ? writebuff : buffer);
            int ret = compression->read_archive_data(data_ptr, ptr->header->uncompressed_size, buff);
            POSTCONDITION(ret > 0);

            shared_read_ptr s_ptr = make_shared<__read_ptr>(buff->get_used_size());
            (*s_ptr).copy(buff->get_ptr());

            data->push_back(s_ptr);
        }
    } else {
        shared_read_ptr s_ptr = make_shared<__read_ptr>(ptr->header->data_size);
        (*s_ptr).set_data_ptr(ptr->data_ptr, ptr->header->data_size);

        data->push_back(s_ptr);
    }
}

uint32_t com::wookler::reactfs::core::base_block::read(uint64_t index, uint32_t count, vector<shared_read_ptr> *data,
                                                       __record_state r_state) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(r_state != __record_state::R_FREE);

    uint64_t current_index = index;
    uint32_t fetched_count = 0;
    temp_buffer *buffer = new temp_buffer();
    CHECK_ALLOC(buffer, TYPE_NAME(temp_buffer));

    temp_buffer *writebuff = new temp_buffer();
    CHECK_ALLOC(writebuff, TYPE_NAME(temp_buffer));

    uint64_t read_bytes = 0;
    nano_timer t;
    t.start();
    while (current_index < header->last_index && fetched_count < count) {
        __record *ptr = __read_record(current_index);
        CHECK_NOT_NULL(ptr);
        read_bytes += ptr->header->data_size;
        switch (r_state) {
            case __record_state::R_DELETED:
            case __record_state::R_DIRTY:
                if (ptr->header->state != r_state) {
                    current_index++;
                    continue;
                }
                break;
            default:
                break;
        }
        process_record_data(ptr, buffer, writebuff, data);
        current_index++;
        fetched_count++;
    }
    t.stop();

    if (read_bytes > 0) {
        update_read_usage(read_bytes, t.get_elapsed());
    }
    return data->size();
}

void com::wookler::reactfs::core::base_block::commit(string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(in_transaction());
    PRECONDITION(!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id));

    if (!IS_EMPTY(rollback_info_deletes)) {
        for (uint64_t index : rollback_info_deletes) {
            __record *record = __read_record(index);
            CHECK_NOT_NULL(record);
            record->header->state = __record_state::R_DELETED;
            CHECK_AND_FREE(record);
            header->deleted_records++;
        }
        rollback_info_deletes.clear();
    }
    void *d_ptr = get_data_ptr();
    uint64_t offset = header->write_offset;
    uint32_t added = 0;
    while (offset < rollback_info->write_offset) {
        void *ptr = buffer_utils::increment_data_ptr(d_ptr, offset);
        __record_header *header = static_cast<__record_header *>(ptr);
        header->state = __record_state::R_READABLE;
        offset += (sizeof(__record_header) + header->data_size);
        added++;
    }
    header->last_index = rollback_info->last_index;
    header->used_bytes += rollback_info->used_bytes;
    header->write_offset = rollback_info->write_offset;
    header->commit_sequence++;
    header->total_records += added;

    mm_data->flush();
    end_transaction();
}

void com::wookler::reactfs::core::base_block::rollback(string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(in_transaction());
    PRECONDITION(!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id));
    force_rollback();
}

void com::wookler::reactfs::core::base_block::force_rollback() {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(in_transaction());
    rollback_info_deletes.clear();
    end_transaction();
}

void com::wookler::reactfs::core::base_block::open(uint64_t block_id, string filename, bool for_update) {

    void *ptr = __open_block(block_id, filename, for_update);
    POSTCONDITION(NOT_NULL(ptr));
    void *bptr = get_data_ptr();
    CHECK_NOT_NULL(bptr);

    if (header->write_state == __write_state::WRITABLE) {
        write_ptr = bptr;
    } else {
        write_ptr = nullptr;
    }

    state.set_state(__state_enum::Available);
}

__block_check_record *com::wookler::reactfs::core::base_block::check_block_sanity() {
    CHECK_STATE_AVAILABLE(state);

    uint32_t total_records = 0;
    uint32_t deleted_records = 0;

    for (uint32_t ii = header->start_index; ii < header->last_index; ii++) {
        __record *ptr = __read_record(ii);
        CHECK_NOT_NULL(ptr);
        if (ptr->header->state != __record_state::R_READABLE) {
            deleted_records++;
        }
        total_records++;
    }
    if (total_records != header->total_records) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_BLOCK_SANITY_FAILED,
                             "[block id=%lu] Block record count mismatch. [expected=%lu][computed=%lu]",
                             header->block_id, header->total_records, total_records);
    }
    TRACE("[block id=%lu] Block record count. [expected=%lu][computed=%lu]",
          header->block_id, header->total_records, total_records);
    if (deleted_records != header->deleted_records) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_BLOCK_SANITY_FAILED,
                             "[block id=%lu] Block deleted record count mismatch. [expected=%lu][computed=%lu]",
                             header->block_id, header->deleted_records, deleted_records);
    }
    TRACE("[block id=%lu] Block deleted record count. [expected=%lu][computed=%lu]",
          header->block_id, header->deleted_records, deleted_records);


    __block_check_record *bcr = (__block_check_record *) malloc(sizeof(__block_check_record));
    CHECK_ALLOC(bcr, TYPE_NAME(__block_check_record));
    bcr->block_id = header->block_id;
    bcr->block_uuid = new string(header->block_uid);
    CHECK_ALLOC(bcr->block_uuid, TYPE_NAME(string));
    bcr->commit_sequence = header->commit_sequence;
    bcr->record_index_last = header->last_index;
    bcr->record_index_start = header->start_index;

    return bcr;
}