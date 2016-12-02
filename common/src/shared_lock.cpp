//
// Created by Subhabrata Ghosh on 02/12/16.
//

#include "common/includes/shared_lock.h"

void
com::wookler::reactfs::common::base_lock_table::__create(uint32_t max_count, uint32_t struct_size, mode_t mode,
                                                         bool manager, bool reset) {
    LOG_DEBUG("Creating shared memory. [group=%s]", group.c_str());
    string table_name = get_table_name();
    string lock_name = get_lock_name();

    if (mode > 0) {
        CREATE_LOCK_P(table_lock, &lock_name, mode);
    } else {
        INIT_LOCK_P(table_lock, &lock_name);
    }
    if (reset)
        table_lock->reset();

    WAIT_LOCK_GUARD(table_lock, 0);
    try {
        uint64_t l_size = max_count * struct_size;
        uint64_t h_size = sizeof(__shared_lock_data);
        uint64_t t_size = (l_size + h_size);
        mm_data = new shm_mapped_data(table_name, t_size, manager);
        CHECK_ALLOC(mm_data, TYPE_NAME(file_mapped_data));
        header_ptr = reinterpret_cast<__shared_lock_data *>(mm_data->get_base_ptr());
        if (manager) {
            header_ptr->max_count = max_count;
            header_ptr->used_count = 0;
            header_ptr->version.major = version.major;
            header_ptr->version.minor = version.minor;
        } else {
            PRECONDITION(version_utils::compatible(header_ptr->version, version));
        }
        LOG_DEBUG("[%] Initialized shared lock table.", table_name.c_str());
    } catch (const exception &e) {
        throw LOCK_ERROR("Error creating lock table instance. [error=%s]", e.what());
    } catch (...) {
        throw LOCK_ERROR("Error creating lock table instance. [error=Unknown]");
    }
}