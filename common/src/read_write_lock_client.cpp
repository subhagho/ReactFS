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
// Created by Subhabrata Ghosh on 25/11/16.
//

#include "common/includes/read_write_lock_client.h"

void
com::wookler::reactfs::common::read_write_lock_client::create(mode_t mode, bool is_manager, bool reset) {
    try {
        table = new rw_lock_table(group);
        CHECK_ALLOC(table, TYPE_NAME(rw_lock_table));

        table->create(mode, is_manager, reset);

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

void com::wookler::reactfs::common::read_write_lock_client::create() {
    std::lock_guard<std::mutex> guard(thread_mutex);
    try {
        table = new rw_lock_table(group);
        CHECK_ALLOC(table, TYPE_NAME(rw_lock_table));

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


read_write_lock *com::wookler::reactfs::common::read_write_lock_client::add_lock(string name) {
    CHECK_STATE_AVAILABLE(state);
    std::lock_guard<std::mutex> guard(thread_mutex);

    read_write_lock *lock = nullptr;
    unordered_map<std::string, read_write_lock *>::const_iterator iter = locks.find(name);
    if (iter != locks.end()) {
        lock = iter->second;
        CHECK_NOT_NULL(lock);
    } else {
        lock = new read_write_lock();
        CHECK_ALLOC(lock, TYPE_NAME(read_write_lock));
        lock->create(&name, table);
        locks[name] = lock;
    }
    lock->increment_ref_count();

    return lock;
}

bool com::wookler::reactfs::common::read_write_lock_client::remove_lock(string name) {
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
