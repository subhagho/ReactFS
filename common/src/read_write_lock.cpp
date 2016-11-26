/*
 * Copyright [2016] [Subhabrata Ghosh]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *       limitations under the License.
 *
 */
//
// Created by Subhabrata Ghosh on 09/11/16.
//

#include "common/includes/read_write_lock.h"
#include "common/includes/__env.h"
#include "common/includes/init_utils.h"

using namespace com::wookler::reactfs::common;

void com::wookler::reactfs::common::shared_lock_table::__create(mode_t mode, bool manager) {
    LOG_DEBUG("Creating shared memory. [name=%s]", RW_SHARED_LOCK_NAME);

    string name_l(RW_SHARED_LOCK_NAME);
    if (mode > 0) {
        CREATE_LOCK_P(table_lock, &name_l, mode);
    } else {
        INIT_LOCK_P(table_lock, &name_l);
    }
    if (mode > 0)
        table_lock->reset();

    WAIT_LOCK_GUARD(table_lock, 0);
    try {
        uint64_t l_size = MAX_SHARED_LOCKS * sizeof(__lock_struct);
        uint64_t h_size = sizeof(__shared_lock_data);
        uint64_t t_size = (l_size + h_size);
        mm_data = new shm_mapped_data(RW_SHARED_LOCK_TABLE_NAME, t_size, manager);
        header_ptr = reinterpret_cast<__shared_lock_data *>(mm_data->get_base_ptr());
        if (manager) {
            header_ptr->max_count = MAX_SHARED_LOCKS;
            header_ptr->used_count = 0;
            header_ptr->version.major = version.major;
            header_ptr->version.minor = version.minor;
        } else {
            PRECONDITION(version_utils::compatible(header_ptr->version, version));
        }
        void *ptr = common_utils::increment_data_ptr(mm_data->get_base_ptr(), h_size);

        locks = reinterpret_cast<__lock_struct *>(ptr);
        POSTCONDITION(NOT_NULL(locks));

        LOG_DEBUG("Initialized shared lock table.");
    } catch (const exception &e) {
        throw LOCK_ERROR("Error creating lock table instance. [error=%s]", e.what());
    } catch (...) {
        throw LOCK_ERROR("Error creating lock table instance. [error=Unknown]");
    }
}

com::wookler::reactfs::common::__lock_struct *com::wookler::reactfs::common::shared_lock_table::add_lock(
        string name, uint64_t timeout) {
    PRECONDITION(NOT_NULL(locks));
    PRECONDITION(!IS_EMPTY(name));
    PRECONDITION(name.length() < SIZE_LOCK_NAME);

    TRY_LOCK_WITH_ERROR(table_lock, 0, timeout);
    try {
        if (header_ptr->used_count >= header_ptr->max_count) {
            throw LOCK_ERROR("Error adding new lock instance. Max records exhausted. [count=%d]",
                             header_ptr->used_count);
        }
        __lock_struct *ptr = locks;
        int found_index = -1;
        int free_index = -1;
        for (int ii = 0; ii < header_ptr->max_count; ii++) {
            if (ptr->used) {
                if (strlen(ptr->name) == name.length() && strncmp(ptr->name, name.c_str(), name.length()) == 0) {
                    found_index = ii;
                    break;
                }
            } else if (free_index < 0) {
                free_index = ii;
            }
        }
        __lock_struct *r_ptr = nullptr;
        if (found_index >= 0) {
            r_ptr = locks + found_index;
            POSTCONDITION(NOT_NULL(r_ptr));
            POSTCONDITION(r_ptr->used);
        }
        if (free_index >= 0) {
            r_ptr = locks + free_index;
            POSTCONDITION(NOT_NULL(r_ptr));
            POSTCONDITION(!r_ptr->used);

            memset(r_ptr, 0, sizeof(__lock_struct));
            r_ptr->used = true;
            memset(r_ptr->name, 0, SIZE_LOCK_NAME);
            strncpy(r_ptr->name, name.c_str(), name.length());
            header_ptr->used_count++;
        }

        return r_ptr;
    } catch (const exception &e) {
        throw LOCK_ERROR("Error creating lock table instance. [error=%s]", e.what());
    } catch (...) {
        throw LOCK_ERROR("Error creating lock table instance. [error=Unknown]");
    }
}

bool com::wookler::reactfs::common::shared_lock_table::remove_lock(string name, uint64_t timeout) {
    PRECONDITION(NOT_NULL(locks));
    PRECONDITION(!IS_EMPTY(name));
    PRECONDITION(name.length() < SIZE_LOCK_NAME);

    bool ret = false;
    TRY_LOCK_WITH_ERROR(table_lock, 0, timeout);
    try {
        __lock_struct *ptr = locks;
        int found_index = -1;
        for (int ii = 0; ii < header_ptr->max_count; ii++) {
            if (ptr->used) {
                if (strlen(ptr->name) == name.length() && strncmp(ptr->name, name.c_str(), name.length()) == 0) {
                    found_index = ii;
                    break;
                }
            }
        }

        if (found_index >= 0) {
            __lock_struct *r_ptr = locks + found_index;
            POSTCONDITION(NOT_NULL(r_ptr));
            POSTCONDITION(r_ptr->used);

            r_ptr->ref_count--;
            uint64_t delta = (time_utils::now() - r_ptr->last_used);
            if (r_ptr->ref_count <= 0 && delta >= DEFAULT_RW_LOCK_EXPIRY) {
                memset(r_ptr, 0, sizeof(__lock_struct));
                r_ptr->used = false;
                r_ptr->reader_count = 0;
                r_ptr->write_locked = false;

                header_ptr->used_count--;
            }
        }
    } catch (const exception &e) {
        throw LOCK_ERROR("Error creating lock table instance. [error=%s]", e.what());
    } catch (...) {
        throw LOCK_ERROR("Error creating lock table instance. [error=Unknown]");
    }
    return ret;
}

