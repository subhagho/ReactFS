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

void com::wookler::reactfs::common::shared_lock_table::create(mode_t mode, uint32_t count) {
    LOG_DEBUG("Creating shared memory. [name=%s]", SHARED_LOCK_NAME);

    string name_l(SHARED_LOCK_NAME);
    table_lock = new exclusive_lock(&name_l, mode);
    table_lock->create();

    shm_fd = shm_open(SHARED_LOCK_TABLE_NAME, O_CREAT | O_RDWR, mode);
    if (shm_fd < 0) {
        lock_error te = LOCK_ERROR(
                "Error creating shared memory handle. [name=%s][error=%s]",
                SHARED_LOCK_TABLE_NAME, strerror(errno));
        LOG_ERROR(te.what());
        throw te;
    }

    uint32_t m_count = (count > 0 ? count : MAX_SHARED_LOCKS);
    uint32_t size = sizeof(__shared_lock_data) + (m_count * sizeof(__lock_struct));

    ftruncate(shm_fd, size);
    LOG_DEBUG("Sizing shared memory segment. [size=%d]", size);

    /* now map the shared memory segment in the address space of the process */
    mem_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (mem_ptr == MAP_FAILED) {
        lock_error te = LOCK_ERROR(
                "Error mapping shared memory segment. [name=%s][error=%s]",
                SHARED_LOCK_TABLE_NAME, strerror(errno));
        LOG_ERROR(te.what());
        throw te;
    }

    header_ptr = static_cast<__shared_lock_data *>(mem_ptr);
    POSTCONDITION(NOT_NULL(header_ptr));
    header_ptr->max_count = m_count;
    header_ptr->used_count = 0;

    BYTE_PTR ptr = static_cast<BYTE_PTR >(mem_ptr);
    ptr += sizeof(__shared_lock_data);

    locks = reinterpret_cast<__lock_struct *>(ptr);
    POSTCONDITION(NOT_NULL(locks));

    LOG_DEBUG("Initialized shared lock table.");
}

void com::wookler::reactfs::common::shared_lock_table::create(uint32_t count) {
    LOG_DEBUG("Creating shared memory. [name=%s]", SHARED_LOCK_NAME);

    string name_l(SHARED_LOCK_NAME);
    table_lock = new exclusive_lock(&name_l);
    table_lock->create();

    table_lock->wait_lock();
    try {
        shm_fd = shm_open(SHARED_LOCK_TABLE_NAME, O_RDWR);
        if (shm_fd < 0) {
            lock_error te = LOCK_ERROR(
                    "Error creating shared memory handle. [name=%s][error=%s]",
                    SHARED_LOCK_TABLE_NAME, strerror(errno));
            LOG_ERROR(te.what());
            throw te;
        }

        uint32_t m_count = (count > 0 ? count : MAX_SHARED_LOCKS);
        uint32_t size = sizeof(__shared_lock_data) + (m_count * sizeof(__lock_struct));

        ftruncate(shm_fd, size);
        LOG_DEBUG("Sizing shared memory segment. [size=%d]", size);

        /* now map the shared memory segment in the address space of the process */
        mem_ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (mem_ptr == MAP_FAILED) {
            lock_error te = LOCK_ERROR(
                    "Error mapping shared memory segment. [name=%s][error=%s]",
                    SHARED_LOCK_TABLE_NAME, strerror(errno));
            LOG_ERROR(te.what());
            throw te;
        }

        header_ptr = static_cast<__shared_lock_data *>(mem_ptr);
        POSTCONDITION(NOT_NULL(header_ptr));
        header_ptr->max_count = m_count;
        header_ptr->used_count = 0;

        BYTE_PTR ptr = static_cast<BYTE_PTR >(mem_ptr);
        ptr += sizeof(__shared_lock_data);

        locks = reinterpret_cast<__lock_struct *>(ptr);
        POSTCONDITION(NOT_NULL(locks));

        LOG_DEBUG("Initialized shared lock table.");
        table_lock->release_lock();
    } catch (const exception &e) {
        table_lock->release_lock();
        throw LOCK_ERROR("Error creating lock table instance. [error=%s]", e.what());
    } catch (...) {
        table_lock->release_lock();
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
            memset(r_ptr, 0, sizeof(__lock_struct));
            r_ptr->used = false;

            header_ptr->used_count--;
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

void com::wookler::reactfs::common::lock_manager::create(mode_t mode, uint32_t count) {
    try {
        table = new shared_lock_table();
        table->create(mode, count);

        state.set_state(__state_enum::Available);

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
            }
        }
    }
}

com::wookler::reactfs::common::__lock_struct *com::wookler::reactfs::common::lock_manager::add_lock(string name) {
    CHECK_STATE_AVAILABLE(state);

    return table->add_lock(name);
}

bool com::wookler::reactfs::common::lock_manager::remove_lock(string name) {
    CHECK_STATE_AVAILABLE(state);

    return table->remove_lock(name);
}