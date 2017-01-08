//
// Created by Subhabrata Ghosh on 02/12/16.
//

#include "common/includes/write_lock.h"

using namespace com::wookler::reactfs::common;

com::wookler::reactfs::common::__w_lock_struct *com::wookler::reactfs::common::w_lock_table::add_lock(
        string name, uint64_t timeout, __lock_isolation_mode mode) {
    PRECONDITION(NOT_NULL(locks));
    PRECONDITION(!IS_EMPTY(name));
    PRECONDITION(name.length() < SIZE_LOCK_NAME);

    TRY_LOCK_WITH_ERROR(table_lock, 0, timeout);
    try {
        if (header_ptr->used_count >= header_ptr->max_count) {
            throw LOCK_ERROR("Error adding new lock instance. Max records exhausted. [count=%d]",
                             header_ptr->used_count);
        }
        __w_lock_struct *ptr = locks;
        int found_index = -1;
        int free_index = -1;
        for (uint32_t ii = 0; ii < header_ptr->max_count; ii++) {
            if (ptr->used) {
                if (strlen(ptr->name) == name.length() &&
                    strncmp(ptr->name, name.c_str(), name.length()) == 0) {
                    found_index = ii;
                    break;
                }
            }
            if (free_index < 0 && !ptr->used) {
                free_index = ii;
            }
            ptr++;
        }
        __w_lock_struct *r_ptr = nullptr;
        if (found_index >= 0) {
            r_ptr = locks + found_index;
            POSTCONDITION(NOT_NULL(r_ptr));
            POSTCONDITION(r_ptr->used);
            POSTCONDITION(r_ptr->mode == mode);
        }
        if (free_index >= 0) {
            r_ptr = locks + free_index;
            POSTCONDITION(NOT_NULL(r_ptr));
            POSTCONDITION(!r_ptr->used);

            memset(r_ptr, 0, sizeof(__w_lock_struct));
            r_ptr->used = true;
            memcpy(r_ptr, &mode, sizeof(__lock_isolation_mode));
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

bool com::wookler::reactfs::common::w_lock_table::remove_lock(string name, uint64_t timeout) {
    PRECONDITION(NOT_NULL(locks));
    PRECONDITION(!IS_EMPTY(name));
    PRECONDITION(name.length() < SIZE_LOCK_NAME);

    bool ret = false;
    TRY_LOCK_WITH_ERROR(table_lock, 0, timeout);
    try {
        __w_lock_struct *ptr = locks;
        int found_index = -1;
        for (uint32_t ii = 0; ii < header_ptr->max_count; ii++) {
            if (ptr->used) {
                if (strlen(ptr->name) == name.length() &&
                    strncmp(ptr->name, name.c_str(), name.length()) == 0) {
                    found_index = ii;
                    break;
                }
            }
            ptr++;
        }

        if (found_index >= 0) {
            __w_lock_struct *r_ptr = locks + found_index;
            POSTCONDITION(NOT_NULL(r_ptr));
            POSTCONDITION(r_ptr->used);

            if (r_ptr->ref_count > 0)
                r_ptr->ref_count--;
            if (r_ptr->ref_count == 0) {
                TRACE("Releasing lock record. [name=%s][count=%lu]", r_ptr->name,
                      r_ptr->ref_count);
                memset(r_ptr, 0, sizeof(__w_lock_struct));
                r_ptr->used = false;
                r_ptr->write_locked = false;
                header_ptr->used_count--;
                ret = true;
            } else {
                TRACE("Lock record not released due to reference(s). [name=%s][count=%lu]", r_ptr->name,
                      r_ptr->ref_count);
            }
        } else {
            LOG_WARN("No lock record found for name. [name=%s]", name.c_str());
        }

    } catch (const exception &e) {
        throw LOCK_ERROR("Error creating lock table instance. [error=%s]", e.what());
    } catch (...) {
        throw LOCK_ERROR("Error creating lock table instance. [error=Unknown]");
    }
    return ret;
}

