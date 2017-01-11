//
// Created by Subhabrata Ghosh on 12/12/16.
//

#include "core/includes/typed_block.h"

string com::wookler::reactfs::core::typed_block::create(uint64_t block_id, uint64_t parent_id, string filename,
                                                        __block_usage usage, uint64_t block_size, uint64_t start_index,
                                                        uint32_t est_record_size, bool overwrite) {
    CHECK_NOT_NULL(this->datetype);

    string uuid = __create_block(block_id, parent_id, filename, usage, __block_def::INDEXED,
                                 block_size, start_index, overwrite);
    PRECONDITION(est_record_size > 0);
    uint32_t estimated_records = (block_size / est_record_size);
    this->estimated_record_size = est_record_size;

    index_ptr = new base_block_index();
    CHECK_ALLOC(index_ptr, TYPE_NAME(base_block_index));

    index_ptr->create_index(header->block_id, header->block_uid, this->filename, estimated_records,
                            header->start_index, overwrite);

    void *ptr = buffer_utils::increment_data_ptr(base_ptr, sizeof(__block_header));
    type_header_size = static_cast<uint64_t *>(ptr);
    *type_header_size = 0;

    ptr = buffer_utils::increment_data_ptr(ptr, sizeof(uint64_t));
    *type_header_size = this->datetype->write(ptr, 0);

    ptr = buffer_utils::increment_data_ptr(ptr, *type_header_size);
    index_count = static_cast<uint8_t *>(ptr);
    *index_count = 0;

    ptr = buffer_utils::increment_data_ptr(ptr, sizeof(uint8_t));
    index_ptrs = static_cast<__index_type *>(ptr);

    close();

    POSTCONDITION(!IS_EMPTY(uuid));
    return uuid;
}

void com::wookler::reactfs::core::typed_block::open(uint64_t block_id, string filename, bool for_update) {
    void *ptr = __open_block(block_id, filename, for_update);
    POSTCONDITION(NOT_NULL(ptr));

    index_ptr = new base_block_index();
    CHECK_ALLOC(index_ptr, TYPE_NAME(base_block_index));
    index_ptr->open_index(header->block_id, header->block_uid, this->filename);

    ptr = buffer_utils::increment_data_ptr(base_ptr, sizeof(__block_header));
    type_header_size = static_cast<uint64_t *>(ptr);

    ptr = buffer_utils::increment_data_ptr(ptr, sizeof(uint64_t));
    this->datetype = new __complex_type(nullptr);
    this->datetype->read(ptr, 0);
    this->datetype->print();
    ptr = buffer_utils::increment_data_ptr(ptr, *type_header_size);

    index_count = static_cast<uint8_t *>(ptr);
    ptr = buffer_utils::increment_data_ptr(ptr, sizeof(uint8_t));
    index_ptrs = static_cast<__index_type *>(ptr);
    ptr = buffer_utils::increment_data_ptr(ptr, (sizeof(__index_type) * BLOCK_MAX_INDEXES));

    this->read_ptr = ptr;
    void *bptr = get_data_ptr();
    CHECK_NOT_NULL(bptr);

    if (header->write_state == __write_state::WRITABLE) {
        write_ptr = buffer_utils::increment_data_ptr(bptr, header->write_offset);
    } else {
        write_ptr = nullptr;
    }

    bool r = metrics_utils::create_metric(get_metric_name(BLOCK_TYPED_METRIC_READ_PREFIX), AverageMetric, false);
    POSTCONDITION(r);
    r = metrics_utils::create_metric(get_metric_name(BLOCK_TYPED_METRIC_WRITE_PREFIX), AverageMetric, false);
    POSTCONDITION(r);

    state.set_state(__state_enum::Available);
}

__record *com::wookler::reactfs::core::typed_block::__write_record(mutable_record_struct *source,
                                                                   string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    CHECK_NOT_NULL(source);
    string m_name = get_metric_name(BLOCK_METRIC_WRITE_PREFIX);
    START_TIMER(m_name, 0);
    PRECONDITION(is_writeable());
    PRECONDITION(in_transaction());
    PRECONDITION((!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id)));

    void *w_ptr = get_write_ptr();

    nano_timer t;
    t.start();

    __record *record = (__record *) malloc(sizeof(__record));
    CHECK_ALLOC(record, TYPE_NAME(__record));


    string tm_name = get_metric_name(BLOCK_TYPED_METRIC_WRITE_PREFIX);
    START_TIMER(tm_name, 1);
    record->header = static_cast<__record_header *>(w_ptr);
    memset(record->header, 0, sizeof(__record_header));
    record->header->index = get_next_index();
    record->header->offset = get_write_offset();
    record->header->timestamp = time_utils::now();
    record->header->state = __record_state::R_DIRTY;

    w_ptr = buffer_utils::increment_data_ptr(w_ptr, sizeof(__record_header));
    record->data_ptr = w_ptr;


    __base_datatype_io *handler = this->datetype->get_type_handler(__record_mode::RM_WRITE);
    CHECK_NOT_NULL(handler);
    __dt_struct *dt_struct = dynamic_cast<__dt_struct *>(handler);
    CHECK_CAST(dt_struct, TYPE_NAME(__base_datatype_io), TYPE_NAME(__dt_struct));

    uint64_t free_s = this->get_free_space();
    free_s -= sizeof(__record_header);
    POSTCONDITION(free_s > 0);

    uint64_t w_size = dt_struct->write(record->data_ptr, source, 0, free_s);

    record->header->data_size = w_size;
    record->header->uncompressed_size = w_size;
    END_TIMER(tm_name, 1);

    rollback_info->used_bytes += w_size;
    rollback_info->write_offset += (sizeof(__record_header) + w_size);

    t.stop();
    END_TIMER(m_name, 0);

    uint64_t write_bytes = (sizeof(__record_header) + w_size);
    update_write_usage(write_bytes, t.get_elapsed());

    header->update_time = record->header->timestamp;

    return record;
}

uint64_t com::wookler::reactfs::core::typed_block::write(void *source, uint32_t length, string transaction_id) {
    CHECK_NOT_NULL(source);
    mutable_record_struct *data = static_cast<mutable_record_struct *>(source);
    CHECK_CAST(data, TYPE_NAME(void), TYPE_NAME(__struct_datatype__));
    __record *r_ptr = __write_record(data, transaction_id);
    LOG_DEBUG("Written typed record of size %d.", r_ptr->header->data_size);
    index_ptr->write_index(r_ptr->header->index, r_ptr->header->offset, r_ptr->header->data_size, transaction_id);

    uint64_t index = r_ptr->header->index;
    CHECK_AND_FREE(r_ptr);

    return index;
}


__record *
com::wookler::reactfs::core::typed_block::__read_record(uint64_t index, uint64_t offset, uint64_t size) {
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

    string tm_name = get_metric_name(BLOCK_TYPED_METRIC_READ_PREFIX);
    START_TIMER(tm_name, 1);

    __base_datatype_io *handler = this->datetype->get_type_handler(__record_mode::RM_READ);
    CHECK_NOT_NULL(handler);
    __dt_struct *dt_struct = dynamic_cast<__dt_struct *>(handler);
    CHECK_CAST(dt_struct, TYPE_NAME(__base_datatype_io), TYPE_NAME(__dt_struct));

    record_struct *st_data = nullptr;

    dt_struct->read(rptr, &st_data, 0, record->header->data_size);
    CHECK_NOT_NULL(st_data);
    END_TIMER(tm_name, 1);

    record->data_ptr = st_data;

    POSTCONDITION(record->header->index == index);
    POSTCONDITION(record->header->offset == offset);
    POSTCONDITION(record->header->data_size == size);

    END_TIMER(m_name, 0);
    return record;
}

uint32_t
com::wookler::reactfs::core::typed_block::read_struct(uint64_t index, uint32_t count, vector<record_struct *> *data,
                                                      __record_state r_state) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(r_state != __record_state::R_FREE);

    uint64_t current_index = index;
    uint32_t fetched_count = 0;

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

        record_struct *st = static_cast<record_struct *>(ptr->data_ptr);
        CHECK_CAST(st, TYPE_NAME(void * ), TYPE_NAME(record_struct));
        data->push_back(st);

        current_index++;
        fetched_count++;
    }
    t.stop();

    if (read_bytes > 0) {
        update_read_usage(read_bytes, t.get_elapsed());
    }
    return data->size();
}

void com::wookler::reactfs::core::typed_block::add_index(record_index *index) {
    CHECK_NOT_NULL(index);
    PRECONDITION(*this->index_count < BLOCK_MAX_INDEXES);
    PRECONDITION(is_writeable());

    string txid = start_transaction();
    CHECK_NOT_EMPTY(txid);
    try {
        __index_type *iptr = find_free_index_ptr();
        CHECK_NOT_NULL(iptr);
        POSTCONDITION(!iptr->used);
        typed_index_base *bi = create_index_instance(index->get_type());
        CHECK_NOT_NULL(bi);
        uint32_t est_records = (header->block_size / estimated_record_size);
        string if_name = bi->create_index(header->block_id, header->block_uid, this->filename, est_records,
                                          header->start_index, true);
        CHECK_NOT_EMPTY(if_name);
        size_t nl = strlen(index->get_name());
        PRECONDITION(nl <= SIZE_MAX_NAME);

        iptr->used = true;
        memset(iptr->name, 0, SIZE_MAX_NAME + 1);
        strncpy(iptr->name, index->get_name(), nl);

        size_t pl = if_name.length();
        PRECONDITION(pl <= SIZE_MAX_PATH);
        memset(iptr->block_path, 0, SIZE_MAX_PATH + 1);
        strncpy(iptr->block_path, if_name.c_str(), pl);

        commit(txid);

        bi->sync(true);
        bi->close();

        CHECK_AND_FREE(bi);
    } catch (const exception &e) {
        rollback(txid);
        throw FS_BASE_ERROR("Error occurred while adding index. [error=%s]", e.what());
    } catch (...) {
        rollback(txid);
        throw FS_BASE_ERROR("Unknown error occurred while adding index. [index name=%s]", index->get_name());
    }

}

void com::wookler::reactfs::core::typed_block::open_index(__index_type *def) {
    CHECK_NOT_NULL(def);
}