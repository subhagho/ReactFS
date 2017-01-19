//
// Created by Subhabrata Ghosh on 21/12/16.
//

#include <core/includes/typed_index_base.h>
#include "core/includes/typed_hash_index.h"

string
com::wookler::reactfs::core::typed_hash_index::create_index(const string &name, uint64_t block_id, string block_uuid,
                                                            string filename,
                                                            uint32_t estimated_records, uint64_t start_index,
                                                            record_index *index_def,
                                                            bool overwrite) {
    try {
        CHECK_NOT_NULL(index_def);
        PRECONDITION(estimated_records > 0);

        Path *ip = get_index_file(filename, block_id, name);
        CHECK_NOT_NULL(ip);
        if (ip->exists()) {
            LOG_INFO("Deleting index file. [file=%s]", ip->get_path().c_str());
            ip->remove();
        }
        LOG_INFO("Creating index : [name=%s][file=%s]", name.c_str(), ip->get_path().c_str());

        uint64_t r_size = estimate_file_size(estimated_records, index_def);
        uint64_t ifile_size =
                sizeof(__typed_index_header) + sizeof(__hash_index_header) + r_size;
        LOG_DEBUG("Creating index file with size = %lu", ifile_size);
        mm_data = new file_mapped_data(ip->get_path(), ifile_size);
        CHECK_ALLOC(mm_data, TYPE_NAME(file_mapped_data));
        base_ptr = mm_data->get_base_ptr();
        header_offset = 0;

        header = static_cast<__typed_index_header *>(base_ptr);
        header->block_id = block_id;
        header->version.major = version.major;
        header->version.minor = version.minor;
        memset(header->block_uid, 0, SIZE_UUID);
        memcpy(header->block_uid, block_uuid.c_str(), block_uuid.length());
        header->create_time = time_utils::now();
        header->update_time = header->create_time;
        header->start_index = start_index;
        header->last_index = header->start_index;
        header->total_size = r_size;
        header->used_size = 0;
        header->write_offset = 0;
        header->write_state = __write_state::WRITABLE;
        header->type = __index_type_enum::HASH_INDEX;
        header_offset += sizeof(__typed_index_header);

        void *ptr = buffer_utils::increment_data_ptr(base_ptr, sizeof(__typed_index_header));
        hash_header = static_cast<__hash_index_header *>(ptr);
        hash_header->bucket_prime = compute_bucket_prime(estimated_records);
        hash_header->bucket_count = compute_bucket_size(estimated_records, hash_header->bucket_prime);
        hash_header->key_size = index_def->compute_index_record_size();
        hash_header->overflow_write_index = 0;

        srand(time(NULL));
        hash_header->block_seed = rand();

        header_offset += sizeof(__hash_index_header);

        ptr = buffer_utils::increment_data_ptr(ptr, sizeof(__hash_index_header));
        uint64_t d_size = index_def->write(ptr, 0);
        POSTCONDITION(d_size > 0);
        header_offset += d_size;

        void *bptr = get_data_ptr();
        CHECK_NOT_NULL(bptr);

        uint64_t w_size = 0;
        for (uint32_t bucket = 0; bucket < hash_header->bucket_count; bucket++) {
            for (uint16_t offset = 0; offset < hash_header->bucket_prime; offset++) {
                void *ptr = get_index_ptr(bucket, offset);
                CHECK_NOT_NULL(ptr);
                w_size += __typed_index_bucket::init_index_record(ptr, 0, offset, HASH_COLLISION_ESTIMATE,
                                                                  hash_header->key_size);
            }
        }

        uint64_t available_size = header->total_size - w_size;
        hash_header->overflow_offset = header_offset + w_size;
        uint16_t ir_size = compute_record_size();
        __pos pos;
        pos.offset = hash_header->overflow_offset;
        pos.size = 0;
        void *overflow_ptr = get_base_overflow_ptr();
        while (available_size > ir_size) {
            uint32_t size = __typed_index_bucket::init_index_record(overflow_ptr, pos.offset,
                                                                    hash_header->overflow_count,
                                                                    HASH_COLLISION_ESTIMATE,
                                                                    hash_header->key_size);
            pos.offset += size;
            pos.size += size;
            available_size -= size;
            hash_header->overflow_count++;
        }
        state.set_state(__state_enum::Available);

        mm_data->flush();

        string ps = string(ip->get_path());
        this->filename = string(ps);
        CHECK_AND_FREE(ip);

        return ps;

    } catch (const exception &e) {
        fs_error_base err = FS_BASE_ERROR("Error creating hash block index. [block id=%lu][filename=%s][error=%s]",
                                          block_id,
                                          filename.c_str(), e.what());
        state.set_error(&err);
        throw err;
    } catch (...) {
        fs_error_base err = FS_BASE_ERROR("Error creating hash block index. [block id=%lu][filename=%s][error=%s]",
                                          block_id,
                                          filename.c_str(), "UNKNOWN ERROR");
        state.set_error(&err);
        throw err;
    }
}

void com::wookler::reactfs::core::typed_hash_index::open_index(const string &name, uint64_t block_id, string block_uuid,
                                                               string filename, bool for_update) {
    Path *p = new Path(filename);

    LOG_DEBUG("Opening new index file. [file=%s]", p->get_path().c_str());

    if (!p->exists()) {
        throw FS_BLOCK_ERROR(
                fs_block_error::ERRCODE_INDEX_FILE_NOUT_FOUND, "File not found. [path=%s]", p->get_path().c_str());
    }
    mm_data = new file_mapped_data(p->get_path());
    CHECK_ALLOC(mm_data, TYPE_NAME(file_mapped_data));

    base_ptr = mm_data->get_base_ptr();

    header_offset = 0;
    header = static_cast<__typed_index_header *>(base_ptr);
    header_offset += sizeof(__typed_index_header);

    if (header->block_id != block_id) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_INDEX_COPRRUPTED,
                             "Block id read from index file does not match. [read block id=%lu][expected block id=%lu]",
                             header->block_id, block_id);
    }
    if (strncmp(header->block_uid, block_uuid.c_str(), block_uuid.length()) != 0) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_INDEX_COPRRUPTED,
                             "Block uid read from index file does not match. [read block uid=%s][expected block uid=%s]",
                             header->block_uid, block_uuid.c_str());
    }
    if (!version_utils::compatible(header->version, version)) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_INDEX_DATA_VERSION,
                             "Block index structure version does not match. [read varion=%s][expected version=%s]",
                             version_utils::get_version_string(header->version).c_str(),
                             version_utils::get_version_string(version).c_str());
    }

    void *ptr = buffer_utils::increment_data_ptr(base_ptr, header_offset);
    hash_header = static_cast<__hash_index_header *>(ptr);
    header_offset += sizeof(__hash_index_header);

    ptr = buffer_utils::increment_data_ptr(ptr, sizeof(__hash_index_header));
    index_def = new record_index();
    uint64_t d_size = index_def->read(ptr, 0, this->datatype);
    header_offset += d_size;

    void *bptr = get_data_ptr();
    CHECK_NOT_NULL(bptr);

    if (header->write_state == __write_state::WRITABLE || for_update) {
        write_ptr = bptr;
        overflow = get_base_overflow_ptr();
    } else {
        write_ptr = nullptr;
    }

    this->filename = string(p->get_path());

    bool r = metrics_utils::create_metric(get_metric_name(HASH_INDEX_METRIC_READ_PREFIX), AverageMetric);
    POSTCONDITION(r);
    r = metrics_utils::create_metric(get_metric_name(HASH_INDEX_METRIC_WRITE_PREFIX), AverageMetric);
    POSTCONDITION(r);

    CHECK_AND_FREE(p);
    state.set_state(Available);
}

void com::wookler::reactfs::core::typed_hash_index::close() {
    CHECK_AND_DISPOSE(state);
    CHECK_AND_FREE(mm_data);

    index_def = nullptr;
    header = nullptr;
    hash_header = nullptr;
    write_ptr = nullptr;
    base_ptr = nullptr;
}

void com::wookler::reactfs::core::typed_hash_index::commit(string txid) {

}

void com::wookler::reactfs::core::typed_hash_index::force_rollback() {

}

void com::wookler::reactfs::core::typed_hash_index::rollback(string txid) {

}

const uint64_t com::wookler::reactfs::core::typed_hash_index::get_free_space() const {
    return 0;
}

const uint64_t com::wookler::reactfs::core::typed_hash_index::get_used_space() const {
    return 0;
}

const __record_index_ptr *com::wookler::reactfs::core::typed_hash_index::read_index(__index_key_set *index, bool all) {
    return nullptr;
}

__typed_index_record *
com::wookler::reactfs::core::typed_hash_index::__write_index(uint32_t hash, __index_key_set *index,
                                                             uint64_t offset, uint64_t size, uint8_t *error) {
    uint32_t bucket = hash % hash_header->bucket_count;
    uint32_t boff = hash % hash_header->bucket_prime;
    uint64_t b_index = (bucket * hash_header->bucket_count) + boff;

    __typed_index_record *rec = nullptr;
    void *b_ptr = get_index_ptr(bucket, boff);
    CHECK_NOT_NULL(b_ptr);

    __typed_index_bucket ib(this->index_def);
    while (true) {
        ib.read(b_ptr, 0, b_index, hash_header->key_size);
        if (!ib.can_write_index()) {
            if (ib.has_overflow()) {
                uint64_t n_offset = ib.get_overflow_offset();
                void *ptr = get_base_overflow_ptr();
                b_ptr = buffer_utils::increment_data_ptr(ptr, n_offset);
                continue;
            } else {
                if (has_overflow_space()) {
                    uint64_t n_offset = get_next_overflow_offset();
                    ib.set_overflow_offset(n_offset);

                    void *ptr = get_base_overflow_ptr();
                    b_ptr = buffer_utils::increment_data_ptr(ptr, n_offset);
                    continue;
                } else {
                    *error = HASH_INDEX_ERROR_NO_SPACE;
                    break;
                }
            }
        }
        ib.write(hash, index, offset, size);
    }
    return rec;
}

const __record_index_ptr *com::wookler::reactfs::core::typed_hash_index::write_index(__index_key_set *index,
                                                                                     uint64_t offset, uint64_t size,
                                                                                     string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(header->write_state == __write_state::WRITABLE);
    CHECK_NOT_NULL(index);
    CHECK_NOT_NULL(index->key_data);
    PRECONDITION(*index->key_size > 0);

    uint32_t hash = 0;
    MurmurHash3_x86_32(index->key_data, *index->key_size, hash_header->block_seed, &hash);

    uint8_t error = 0;
    __typed_index_record *rec = __write_index(hash, index, offset, size, &error);
    if (IS_NULL(rec)) {
        if (error == HASH_INDEX_ERROR_NO_SPACE) {
            throw FS_SPACE_ERROR("Hash index block out of free space.");
        } else {
            throw FS_BASE_ERROR("Error writing hash index. [error=%d]", error);
        }
    }

    return nullptr;
}

bool com::wookler::reactfs::core::typed_hash_index::delete_index(__index_key_set *index, string transaction_id) {
    return false;
}

void com::wookler::reactfs::core::typed_hash_index::sync(bool recreate) {

}
