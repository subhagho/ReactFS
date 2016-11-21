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
    LOG_DEBUG("Creating shared memory. [name=%s]", SHARED_LOCK_NAME);

    string name_l(SHARED_LOCK_NAME);
    if (mode > 0) {
        table_lock = new exclusive_lock(&name_l, mode);
    } else {
        table_lock = new exclusive_lock(&name_l);
    }
    table_lock->create();
    if (mode > 0)
        table_lock->reset();

    WAIT_LOCK_P(table_lock);
    try {
        uint64_t l_size = MAX_SHARED_LOCKS * sizeof(__lock_struct);
        uint64_t h_size = sizeof(__shared_lock_data);
        uint64_t t_size = (l_size + h_size);
        mm_data = new shm_mapped_data(SHARED_LOCK_TABLE_NAME, t_size, manager);
        header_ptr = reinterpret_cast<__shared_lock_data *>(mm_data->get_base_ptr());
        header_ptr->max_count = MAX_SHARED_LOCKS;
        header_ptr->used_count = 0;

        void *ptr = common_utils::increment_data_ptr(mm_data->get_base_ptr(), h_size);

        locks = reinterpret_cast<__lock_struct *>(ptr);
        POSTCONDITION(NOT_NULL(locks));

        LOG_DEBUG("Initialized shared lock table.");
        RELEASE_LOCK_P(table_lock);
    } catch (const exception &e) {
        RELEASE_LOCK_P(table_lock);
        throw LOCK_ERROR("Error creating lock table instance. [error=%s]", e.what());
    } catch (...) {
        RELEASE_LOCK_P(table_lock);
        throw LOCK_ERROR("Error creating lock table instance. [error=Unknown]");
    }
}

com::wookler::reactfs::common::__lock_struct *com::wookler::reactfs::common::shared_lock_table::add_lock(
        string name) {
    PRECONDITION(NOT_NULL(locks));
    PRECONDITION(!IS_EMPTY(name));
    PRECONDITION(name.length() < SIZE_LOCK_NAME);

    table_lock->wait_lock();
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
        table_lock->release_lock();

        return r_ptr;
    } catch (const exception &e) {
        table_lock->release_lock();
        throw LOCK_ERROR("Error creating lock table instance. [error=%s]", e.what());
    } catch (...) {
        table_lock->release_lock();
        throw LOCK_ERROR("Error creating lock table instance. [error=Unknown]");
    }
}

bool com::wookler::reactfs::common::shared_lock_table::remove_lock(string name) {
    PRECONDITION(NOT_NULL(locks));
    PRECONDITION(!IS_EMPTY(name));
    PRECONDITION(name.length() < SIZE_LOCK_NAME);

    bool ret = false;
    table_lock->wait_lock();
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
        table_lock->release_lock();
    } catch (const exception &e) {
        table_lock->release_lock();
        throw LOCK_ERROR("Error creating lock table instance. [error=%s]", e.what());
    } catch (...) {
        table_lock->release_lock();
        throw LOCK_ERROR("Error creating lock table instance. [error=Unknown]");
    }
    return ret;
}

void com::wookler::reactfs::common::lock_env::create(mode_t mode, bool is_manager) {
    try {
        table = new shared_lock_table();
        table->create(mode, is_manager);

        state.set_state(__state_enum::Available);

    } catch (const exception &e) {
        lock_error le = LOCK_ERROR("Error creating lock manager instance. [error=%s]", e.what());
        state.set_error(&le);
        throw le;
    } catch (...) {
        lock_error le = LOCK_ERROR("Error creating lock manager instance. [error=Unknown]");
        state.set_error(&le);
        throw le;
    }
}

void com::wookler::reactfs::common::lock_env::create() {
    std::lock_guard<std::mutex> guard(thread_mutex);
    try {
        table = new shared_lock_table();
        table->create();

        state.set_state(__state_enum::Available);

    } catch (const exception &e) {
        lock_error le = LOCK_ERROR("Error creating lock manager instance. [error=%s]", e.what());
        state.set_error(&le);
        throw le;
    } catch (...) {
        lock_error le = LOCK_ERROR("Error creating lock manager instance. [error=Unknown]");
        state.set_error(&le);
        throw le;
    }
}

void com::wookler::reactfs::common::lock_manager::init(mode_t mode) {
    try {
        create(mode, true);
        reset();

        manager_thread = new thread(lock_manager::run, this);

    } catch (const exception &e) {
        lock_error le = LOCK_ERROR("Error creating lock manager instance. [error=%s]", e.what());
        state.set_error(&le);
        throw le;
    } catch (...) {
        lock_error le = LOCK_ERROR("Error creating lock manager instance. [error=Unknown]");
        state.set_error(&le);
        throw le;
    }
}

void com::wookler::reactfs::common::lock_manager::run(lock_manager *manager) {
    PRECONDITION(NOT_NULL(manager));
    try {
        LOG_INFO("Starting lock manager thread...");
        while (manager->state.get_state() == __state_enum::Available) {
            PRECONDITION(NOT_NULL(manager));
            manager->check_lock_states();

            START_ALARM(DEFAULT_LOCK_MGR_SLEEP);
        }
        LOG_INFO("Lock manager thread terminated...");
    } catch (const exception &e) {
        LOG_ERROR("Lock manager thread terminated with error. [error=%s]", e.what());
        manager->state.set_error(&e);
    } catch (...) {
        lock_error le = LOCK_ERROR("Lock manager thread terminated with unhandled exception.");
        manager->state.set_error(&le);
    }
}

void com::wookler::reactfs::common::lock_manager::check_lock_states() {
    if (state.get_state() == __state_enum::Available) {
        uint32_t count = table->get_max_size();
        for (uint32_t ii = 0; ii < count; ii++) {
            __lock_struct *ptr = table->get_at(ii);
            if (NOT_NULL(ptr)) {
                if (ptr->write_locked) {
                    uint64_t now = time_utils::now();
                    if ((now - ptr->owner.lock_timestamp) > DEFAULT_WRITE_LOCK_TIMEOUT) {
                        ptr->owner.lock_timestamp = 0;
                        ptr->owner.process_id = -1;
                        memset(ptr->owner.txn_id, 0, SIZE_UUID);
                        memset(ptr->owner.owner, 0, SIZE_USER_NAME);
                        ptr->write_locked = false;
                    }
                }
                __lock_readers *r_ptr = ptr->readers;
                for (int ii = 0; ii < MAX_READER_LOCKS; ii++) {
                    if (r_ptr[ii].used) {
                        uint64_t now = time_utils::now();
                        if ((now - r_ptr->lock_timestamp) > DEFAULT_READ_LOCK_TIMEOUT) {
                            r_ptr[ii].used = false;
                            ptr->reader_count--;
                        }
                    }
                }
                uint64_t delta = (time_utils::now() - ptr->last_used);
                if (ptr->ref_count <= 0 && delta >= DEFAULT_RW_LOCK_EXPIRY) {
                    table->remove_lock(ptr->name);
                }
            }
        }
    }
}

read_write_lock *com::wookler::reactfs::common::lock_env::add_lock(string name) {
    CHECK_STATE_AVAILABLE(state);
    std::lock_guard<std::mutex> guard(thread_mutex);

    read_write_lock *lock = nullptr;
    unordered_map<std::string, read_write_lock *>::const_iterator iter = locks.find(name);
    if (iter != locks.end()) {
        lock = iter->second;
        CHECK_NOT_NULL(lock);
    } else {
        lock = new read_write_lock();
        lock->create(&name, table);
        locks[name] = lock;
    }
    lock->increment_ref_count();

    return lock;
}

bool com::wookler::reactfs::common::lock_env::remove_lock(string name) {
    CHECK_STATE_AVAILABLE(state);
    std::lock_guard<std::mutex> guard(thread_mutex);

    read_write_lock *lock = nullptr;
    unordered_map<std::string, read_write_lock *>::const_iterator iter = locks.find(name);
    if (iter != locks.end()) {
        lock = iter->second;
        CHECK_NOT_NULL(lock);
    }
    if (NOT_NULL(lock)) {
        string thread_id = thread_utils::get_current_thread();
        if (lock->has_write_lock(thread_id)) {
            lock->release_write_lock();
        }
        if (lock->has_thread_lock(thread_id)) {
            lock->release_read_lock();
        }
        lock->decrement_ref_count();
        if (lock->get_reference_count() <= 0) {
            table->remove_lock(lock->get_name());
            locks.erase(lock->get_name());
            delete (lock);
        }
    }
    return false;
}

void com::wookler::reactfs::common::lock_manager::reset() {
    CHECK_STATE_AVAILABLE(state);
    std::lock_guard<std::mutex> guard(thread_mutex);
    uint32_t count = table->get_max_size();
    for (uint32_t ii = 0; ii < count; ii++) {
        __lock_struct *ptr = table->get_at(ii);
        if (NOT_NULL(ptr)) {
            if (ptr->write_locked) {
                pid_t pid = ptr->owner.process_id;
                if (!process_utils::check_process(pid)) {
                    ptr->owner.lock_timestamp = 0;
                    ptr->owner.process_id = -1;
                    memset(ptr->owner.txn_id, 0, SIZE_UUID);
                    memset(ptr->owner.owner, 0, SIZE_USER_NAME);
                    ptr->write_locked = false;
                } else {
                    uint64_t now = time_utils::now();
                    if ((now - ptr->owner.lock_timestamp) > DEFAULT_WRITE_LOCK_TIMEOUT) {
                        ptr->owner.lock_timestamp = 0;
                        ptr->owner.process_id = -1;
                        memset(ptr->owner.txn_id, 0, SIZE_UUID);
                        memset(ptr->owner.owner, 0, SIZE_USER_NAME);
                        ptr->write_locked = false;
                    }
                }
            }
            __lock_readers *r_ptr = ptr->readers;
            int reader_count = 0;
            for (int ii = 0; ii < MAX_READER_LOCKS; ii++) {
                if (r_ptr[ii].used) {
                    bool used = true;
                    pid_t pid = r_ptr[ii].process_id;
                    if (!process_utils::check_process(pid)) {
                        r_ptr[ii].used = false;
                        used = false;
                    } else {
                        uint64_t now = time_utils::now();
                        if ((now - r_ptr->lock_timestamp) > DEFAULT_READ_LOCK_TIMEOUT) {
                            r_ptr[ii].used = false;
                            used = false;
                        }
                    }
                    if (used) {
                        reader_count++;
                    }
                }
            }
            ptr->reader_count = reader_count;
        }
    }
}

bool com::wookler::reactfs::common::lock_env_utils::is_manager = false;
lock_env *com::wookler::reactfs::common::lock_env_utils::env = nullptr;
mutex com::wookler::reactfs::common::lock_env_utils::env_mutex;