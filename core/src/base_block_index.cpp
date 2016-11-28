//
// Created by Subhabrata Ghosh on 16/11/16.
//

#include "core/includes/base_block_index.h"

string com::wookler::reactfs::core::base_block_index::__create_index(uint64_t block_id, string block_uuid,
                                                                     string filename, uint32_t estimated_records,
                                                                     uint64_t start_index, bool overwrite) {
    try {
        Path *p = get_index_file(filename);

        LOG_DEBUG("Creating new index file. [file=%s]", p->get_path().c_str());
        if (p->exists()) {
            if (!overwrite) {
                throw FS_BASE_ERROR("File with specified path already exists. [path=%s]", p->get_path().c_str());
            } else {
                if (!p->remove()) {
                    throw FS_BASE_ERROR("Error deleting existing file. [file=%s]", p->get_path().c_str());
                }
            }
        }
        uint64_t r_size = (estimated_records * sizeof(__record_index_ptr) * DEFAULT_BLOAT_FACTOR);
        uint64_t ifile_size =
                sizeof(__record_index_header) + r_size;
        LOG_DEBUG("Creating index file with size = %lu", ifile_size);
        mm_data = new file_mapped_data(p->get_path(), ifile_size);
        CHECK_ALLOC(mm_data, TYPE_NAME(file_mapped_data));
        base_ptr = mm_data->get_base_ptr();

        header = static_cast<__record_index_header *>(base_ptr);
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

        state.set_state(__state_enum::Available);

        mm_data->flush();

        string ps = string(p->get_path());
        this->filename = string(ps);
        CHECK_AND_FREE(p);

        return ps;
    } catch (const exception &e) {
        fs_error_base err = FS_BASE_ERROR("Error creating block index. [block id=%lu][filename=%s][error=%s]", block_id,
                                          filename.c_str(), e.what());
        state.set_error(&err);
        throw err;
    } catch (...) {
        fs_error_base err = FS_BASE_ERROR("Error creating block index. [block id=%lu][filename=%s][error=%s]", block_id,
                                          filename.c_str(), "UNKNOWN ERROR");
        state.set_error(&err);
        throw err;
    }
}

void *com::wookler::reactfs::core::base_block_index::__open_index(uint64_t block_id, string block_uuid,
                                                                  string filename) {
    Path *p = get_index_file(filename);

    LOG_DEBUG("Opening new index file. [file=%s]", p->get_path().c_str());

    if (!p->exists()) {
        throw FS_BLOCK_ERROR(
                fs_block_error::ERRCODE_INDEX_FILE_NOUT_FOUND, "File not found. [path=%s]", p->get_path().c_str());
    }
    mm_data = new file_mapped_data(p->get_path());
    CHECK_ALLOC(mm_data, TYPE_NAME(file_mapped_data));

    base_ptr = mm_data->get_base_ptr();

    header = static_cast<__record_index_header *>(base_ptr);
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

    this->filename = string(p->get_path());

    return base_ptr;
}

void com::wookler::reactfs::core::base_block_index::close() {
    CHECK_AND_DISPOSE(state);
    CHECK_AND_FREE(mm_data);

    if (NOT_NULL(rollback_info)) {
        CHECK_AND_FREE(rollback_info->transaction_id);
        FREE_PTR(rollback_info);
    }
    rollback_info = nullptr;
    header = nullptr;
    write_ptr = nullptr;
    base_ptr = nullptr;
}

void com::wookler::reactfs::core::base_block_index::open_index(uint64_t block_id, string block_uuid, string filename) {
    void *ptr = __open_index(block_id, block_uuid, filename);
    CHECK_NOT_NULL(ptr);
    void *bptr = get_data_ptr();
    CHECK_NOT_NULL(bptr);

    if (header->write_state == __write_state::WRITABLE) {
        write_ptr = bptr;
    } else {
        write_ptr = nullptr;
    }
    state.set_state(Available);
}

__record_index_ptr *
com::wookler::reactfs::core::base_block_index::__write_index(uint64_t index, uint64_t offset, uint64_t size,
                                                             string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(header->write_state == __write_state::WRITABLE);
    PRECONDITION(has_space(sizeof(__record_index_ptr)));
    PRECONDITION(in_transaction());
    PRECONDITION(!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id));
    uint64_t last_index = get_last_index();
    PRECONDITION((index == header->start_index) || (index == last_index + 1));
    PRECONDITION(offset >= 0);

    TRACE("[index=%lu] Available index space = %lu", index, get_free_space());

    void *wptr = get_write_ptr();
    __record_index_ptr *iptr = static_cast<__record_index_ptr *>(wptr);
    memset(iptr, 0, sizeof(__record_index_ptr));
    iptr->index = index;
    iptr->offset = offset;
    iptr->size = size;
    iptr->readable = false;

    rollback_info->last_index = iptr->index;
    rollback_info->used_bytes += sizeof(__record_index_ptr);
    rollback_info->write_offset += sizeof(__record_index_ptr);

    return iptr;
}

__record_index_ptr *com::wookler::reactfs::core::base_block_index::__read_index(uint64_t index, bool all) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(index >= header->start_index && index <= header->last_index);
    uint64_t offset = (index - header->start_index) * sizeof(__record_index_ptr);

    void *ptr = get_data_ptr();
    void *rptr = common_utils::increment_data_ptr(ptr, offset);
    __record_index_ptr *iptr = static_cast<__record_index_ptr *>(rptr);
    if (iptr->index != index) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_INDEX_COPRRUPTED,
                             "Block index miss-match. [expected index=%lu][found index=%lu]", index, iptr->index);
    }
    if (!all && !iptr->readable)
        return nullptr;

    return iptr;
}

bool com::wookler::reactfs::core::base_block_index::delete_index(uint64_t index, string transaction_id) {
    CHECK_STATE_AVAILABLE(state);
    PRECONDITION(header->write_state == __write_state::WRITABLE);
    PRECONDITION(in_transaction());
    PRECONDITION(!IS_EMPTY(transaction_id) && (*rollback_info->transaction_id == transaction_id));

    uint64_t offset = (index - header->start_index) * sizeof(__record_index_ptr);
    void *ptr = get_data_ptr();
    void *rptr = common_utils::increment_data_ptr(ptr, offset);
    __record_index_ptr *iptr = static_cast<__record_index_ptr *>(rptr);
    if (iptr->index != index) {
        throw FS_BLOCK_ERROR(fs_block_error::ERRCODE_INDEX_COPRRUPTED,
                             "Block index miss-match. [expected index=%lu][found index=%lu]", index, iptr->index);
    }

    rollback_info_deletes.push_back(index);
    return false;
}