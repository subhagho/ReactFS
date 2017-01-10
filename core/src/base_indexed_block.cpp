//
// Created by Subhabrata Ghosh on 01/12/16.
//

#include "core/includes/base_indexed_block.h"

uint64_t com::wookler::reactfs::core::base_indexed_block::write(void *source, uint32_t length, string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    CHECK_NOT_NULL(source);

    __record *r_ptr = write_record(source, length, transaction_id);

    index_ptr->write_index(r_ptr->header->index, r_ptr->header->offset, r_ptr->header->data_size, transaction_id);

    uint64_t index = r_ptr->header->index;
    CHECK_AND_FREE(r_ptr);

    return index;
}

void com::wookler::reactfs::core::base_indexed_block::close() {
    base_block::close();
    CHECK_AND_FREE(index_ptr);
}

string com::wookler::reactfs::core::base_indexed_block::start_transaction(uint64_t timeout, const string &tid) {
    string txid = base_block::start_transaction(timeout, tid);
    index_ptr->start_transaction(txid);

    return txid;
}

uint32_t
com::wookler::reactfs::core::base_indexed_block::read(uint64_t index, uint32_t count, vector<shared_read_ptr> *data,
                                                      __record_state r_state) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(r_state != __record_state::R_FREE);

    uint64_t current_index = index;
    uint32_t fetched_count = 0;
    temp_buffer *buffer = new temp_buffer();
    CHECK_ALLOC(buffer, TYPE_NAME(temp_buffer));

    temp_buffer *writebuff = nullptr;

    bool r_type = false;
    switch (r_state) {
        case __record_state::R_DELETED:
        case __record_state::R_DIRTY:
        case __record_state::R_ALL:
            r_type = true;
            break;
        default:
            break;
    }

    uint64_t read_bytes = 0;
    nano_timer t;
    t.start();
    while (current_index < header->last_index && fetched_count < count) {
        const __record_index_ptr *iptr = index_ptr->read_index(current_index, r_type);
        if (IS_NULL(iptr)) {
            current_index++;
            continue;
        }

        __record *ptr = __read_record(iptr->index, iptr->offset, iptr->size);
        CHECK_NOT_NULL(ptr);
        read_bytes += iptr->size;
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
        node_client_env *n_env = node_init_client::get_client_env();
        mount_client *m_client = n_env->get_mount_client();
        m_client->update_read_metrics(&filename, read_bytes, t.get_elapsed());
    }
    return data->size();
}

void com::wookler::reactfs::core::base_indexed_block::commit(string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(in_transaction());
    PRECONDITION(!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id));

    vector<uint64_t> deleted = index_ptr->get_deleted_indexes(transaction_id);
    if (!IS_EMPTY(deleted)) {
        for (uint64_t index : deleted) {
            const __record_index_ptr *iptr = index_ptr->read_index(index, true);
            CHECK_NOT_NULL(iptr);
            __record *ptr = __read_record(iptr->index, iptr->offset, iptr->size);
            CHECK_NOT_NULL(ptr);
            ptr->header->state = __record_state::R_DELETED;
            header->deleted_records++;
        }
    }
    index_ptr->commit(transaction_id);
    mm_data->flush();

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

    end_transaction();
}

void com::wookler::reactfs::core::base_indexed_block::rollback(string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(in_transaction());
    PRECONDITION(!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id));
    index_ptr->rollback(transaction_id);

    force_rollback();
}

void com::wookler::reactfs::core::base_indexed_block::open(uint64_t block_id, string filename, bool for_update) {

    void *ptr = __open_block(block_id, filename, for_update);
    POSTCONDITION(NOT_NULL(ptr));
    void *bptr = get_data_ptr();
    CHECK_NOT_NULL(bptr);

    if (header->write_state == __write_state::WRITABLE) {
        write_ptr = bptr;
    } else {
        write_ptr = nullptr;
    }

    index_ptr = new base_block_index();
    CHECK_ALLOC(index_ptr, TYPE_NAME(base_block_index));
    index_ptr->open_index(header->block_id, header->block_uid, this->filename);

    state.set_state(__state_enum::Available);
}

__block_check_record *com::wookler::reactfs::core::base_indexed_block::check_block_sanity() {
    CHECK_STATE_AVAILABLE(state);

    uint32_t total_records = 0;
    uint32_t deleted_records = 0;

    for (uint32_t ii = header->start_index; ii < header->last_index; ii++) {
        const __record_index_ptr *iptr = index_ptr->read_index(ii, true);
        if (IS_NULL(iptr)) {
            throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_BLOCK_SANITY_FAILED,
                                 "[block id=%lu] Missing index record. [index=%lu]", header->block_id, ii);
        }
        if (iptr->readable) {
            __record *ptr = __read_record(iptr->index, iptr->offset, iptr->size);
            CHECK_NOT_NULL(ptr);
        } else {
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
    bcr->block_uuid = string(header->block_uid);
    bcr->commit_sequence = header->commit_sequence;
    bcr->record_index_last = header->last_index;
    bcr->record_index_start = header->start_index;

    return bcr;
}