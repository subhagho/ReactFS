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

com::wookler::reactfs::common::__rw_lock_struct *com::wookler::reactfs::common::rw_lock_table::add_lock(
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
        __rw_lock_struct *ptr = locks;
        int found_index = -1;
        int free_index = -1;
        for (int ii = 0; ii < header_ptr->max_count; ii++) {
            if (ptr->w_lock_struct.used) {
                if (strlen(ptr->w_lock_struct.name) == name.length() &&
                    strncmp(ptr->w_lock_struct.name, name.c_str(), name.length()) == 0) {
                    found_index = ii;
                    break;
                }
            } else if (free_index < 0) {
                free_index = ii;
            }
        }
        __rw_lock_struct *r_ptr = nullptr;
        if (found_index >= 0) {
            r_ptr = locks + found_index;
            POSTCONDITION(NOT_NULL(r_ptr));
            POSTCONDITION(r_ptr->w_lock_struct.used);
        }
        if (free_index >= 0) {
            r_ptr = locks + free_index;
            POSTCONDITION(NOT_NULL(r_ptr));
            POSTCONDITION(!r_ptr->w_lock_struct.used);

            memset(r_ptr, 0, sizeof(__rw_lock_struct));
            r_ptr->w_lock_struct.used = true;
            memset(r_ptr->w_lock_struct.name, 0, SIZE_LOCK_NAME);
            strncpy(r_ptr->w_lock_struct.name, name.c_str(), name.length());
            header_ptr->used_count++;
        }

        return r_ptr;
    } catch (const exception &e) {
        throw LOCK_ERROR("Error creating lock table instance. [error=%s]", e.what());
    } catch (...) {
        throw LOCK_ERROR("Error creating lock table instance. [error=Unknown]");
    }
}

bool com::wookler::reactfs::common::rw_lock_table::remove_lock(string name, uint64_t timeout) {
    PRECONDITION(NOT_NULL(locks));
    PRECONDITION(!IS_EMPTY(name));
    PRECONDITION(name.length() < SIZE_LOCK_NAME);

    bool ret = false;
    TRY_LOCK_WITH_ERROR(table_lock, 0, timeout);
    try {
        __rw_lock_struct *ptr = locks;
        int found_index = -1;
        for (int ii = 0; ii < header_ptr->max_count; ii++) {
            if (ptr->w_lock_struct.used) {
                if (strlen(ptr->w_lock_struct.name) == name.length() &&
                    strncmp(ptr->w_lock_struct.name, name.c_str(), name.length()) == 0) {
                    found_index = ii;
                    break;
                }
            }
        }

        if (found_index >= 0) {
            __rw_lock_struct *r_ptr = locks + found_index;
            POSTCONDITION(NOT_NULL(r_ptr));
            POSTCONDITION(r_ptr->w_lock_struct.used);

            r_ptr->w_lock_struct.ref_count--;
            if (r_ptr->w_lock_struct.ref_count <= 0) {
                memset(r_ptr, 0, sizeof(__rw_lock_struct));
                r_ptr->w_lock_struct.used = false;
                r_ptr->reader_count = 0;
                r_ptr->w_lock_struct.write_locked = false;

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

