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
// Created by Subhabrata Ghosh on 04/11/16.
//

#ifndef REACTFS_READ_WRITE_LOCK_H
#define REACTFS_READ_WRITE_LOCK_H

#include <mutex>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <thread>
#include <unordered_map>

#include "exclusive_lock.h"
#include "timer.h"
#include "__alarm.h"
#include "process_utils.h"
#include "unordered_map"
#include "__bitset.h"
#include "mapped_data.h"

#define DEFAULT_RW_LOCK_RETRY 10
#define SIZE_LOCK_NAME 32
#define MAX_READER_LOCKS 256
#define MAX_SHARED_LOCKS 4096

#define DEFAULT_LOCK_MGR_SLEEP 30 * 1000
#define DEFAULT_READ_LOCK_TIMEOUT 5 * 60 * 1000
#define DEFAULT_WRITE_LOCK_TIMEOUT  60 * 1000
#define DEFAULT_RW_LOCK_EXPIRY 10 * 60 * 1000

#define SHARED_LOCK_TABLE_NAME "LOCK_shared_table"
#define SHARED_LOCK_NAME "LOCK_shared_lock_table"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                typedef struct __owner__ {
                    char owner[SIZE_USER_NAME];
                    char txn_id[SIZE_UUID];
                    pid_t process_id;
                    char thread_id[SIZE_THREAD_ID];
                    uint64_t lock_timestamp = 0;
                } __owner;

                typedef struct __lock_readers__ {
                    bool used = false;
                    pid_t process_id;
                    char thread_id[SIZE_THREAD_ID];
                    uint64_t lock_timestamp = 0;
                } __lock_readers;

                typedef struct __lock_struct__ {
                    bool used = false;
                    char name[SIZE_LOCK_NAME];
                    uint64_t last_used = 0;
                    bool write_locked = false;
                    uint32_t ref_count = 0;
                    __owner owner;
                    uint64_t reader_count;
                    __lock_readers readers[MAX_READER_LOCKS];
                } __lock_struct;

                typedef struct __shared_lock_data__ {
                    uint32_t max_count = 0;
                    uint32_t used_count = 0;
                } __shared_lock_data;

                class shared_lock_table {
                private:
                    /// Memory-mapped file handle.
                    shm_mapped_data *mm_data = nullptr;
                    exclusive_lock *table_lock = nullptr;
                    __lock_struct *locks = nullptr;
                    __shared_lock_data *header_ptr = nullptr;

                    void __create(mode_t mode, bool manager = false);

                public:
                    ~shared_lock_table() {
                        CHECK_AND_FREE(table_lock);
                        CHECK_AND_FREE(mm_data);
                        locks = nullptr;
                        header_ptr = nullptr;
                    }

                    uint32_t get_max_size() {
                        PRECONDITION(NOT_NULL(header_ptr));
                        return header_ptr->max_count;
                    }

                    uint32_t get_used_size() {
                        PRECONDITION(NOT_NULL(header_ptr));
                        return header_ptr->used_count;
                    }

                    __lock_struct *get_at(uint32_t index) {
                        PRECONDITION(NOT_NULL(locks));
                        PRECONDITION(NOT_NULL(header_ptr));
                        PRECONDITION(index >= 0 && index < header_ptr->max_count);

                        __lock_struct *ptr = locks + index;
                        if (ptr->used) {
                            return ptr;
                        }
                        return nullptr;
                    }

                    void create(mode_t mode, bool is_manager) {
                        __create(mode, is_manager);
                    }

                    void create() {
                        __create(0, false);
                    }

                    __lock_struct *add_lock(string name);

                    bool remove_lock(string name);
                };

                class read_write_lock {
                private:
                    __state__ state;
                    exclusive_lock *lock;
                    __lock_struct *lock_struct;
                    string txn_id;
                    unordered_map<string, int> reader_threads;
                    shared_lock_table *table = nullptr;
                    string name;
                    uint32_t reference_count = 0;

                    int find_free_reader() {
                        __lock_readers *ptr = lock_struct->readers;
                        for (int ii = 0; ii < MAX_READER_LOCKS; ii++) {
                            if (!ptr[ii].used) {
                                ptr[ii].used = true;
                                return ii;
                            }
                        }
                        return -1;
                    }

                    void check_and_clear() {
                        PRECONDITION(NOT_NULL(lock_struct));
                        PRECONDITION(lock_struct->used);

                        pid_t pid = getpid();
                        if (lock_struct->write_locked && lock_struct->owner.process_id == pid) {
                            release_write_lock();
                        }
                        if (!reader_threads.empty()) {
                            vector<string> threads;
                            for (auto it = reader_threads.begin(); it != reader_threads.end(); ++it) {
                                int index = it->second;
                                if (index >= 0) {
                                    __lock_readers r = lock_struct->readers[index];
                                    if (r.used) {
                                        threads.push_back(string(r.thread_id));
                                    }
                                }
                            }
                            for (vector<string>::iterator it = threads.begin(); it != threads.end(); ++it) {
                                release_read_lock(*it);
                            }
                        }
                        table->remove_lock(name);
                    }

                    void check_lock_valid() {
                        if (!lock_struct->used) {
                            lock_error e = LOCK_ERROR("Lock has been released by manager.");
                            state.set_error(&e);
                        } else if (strncmp(lock_struct->name, name.c_str(), name.length()) != 0) {
                            lock_error e = LOCK_ERROR("Lock has been released by manager and reassigned.");
                            state.set_error(&e);
                        }
                        uint64_t delta = (time_utils::now() - lock_struct->last_used);
                        if (delta >= DEFAULT_RW_LOCK_EXPIRY) {
                            lock_error e = LOCK_ERROR("Lock has expired.");
                            state.set_error(&e);
                        }
                    }

                    bool release_read_lock(string thread_id) {
                        bool locked = false;
                        if (lock->wait_lock()) {
                            if (has_thread_lock(thread_id)) {
                                if (lock_struct->reader_count > 0) {
                                    int index = get_reader_index(thread_id);
                                    if (index >= 0) {
                                        lock_struct->reader_count--;
                                        lock_struct->readers[index].used = false;

                                        reader_threads.erase(thread_id);
                                        locked = true;
                                    }
                                } else {
                                    throw LOCK_ERROR("Error getting lock. [name=%s]", lock->get_name()->c_str());
                                }
                            }
                            lock_struct->last_used = time_utils::now();
                            lock->release_lock();
                        }
                        return locked;
                    }

                public:
                    read_write_lock() {

                    }

                    ~read_write_lock() {
                        state.set_state(__state_enum::Disposed);
                        check_and_clear();

                        CHECK_AND_FREE(lock);
                        lock_struct = nullptr;
                    }

                    void create(const string *name, shared_lock_table *table) {
                        try {
                            CHECK_NOT_NULL(table);
                            CHECK_NOT_EMPTY_P(name);

                            this->name = string(*name);
                            lock = new exclusive_lock(name);
                            lock->create();

                            this->table = table;
                            lock_struct = this->table->add_lock(*name);
                            POSTCONDITION(NOT_NULL(lock_struct));
                            lock_struct->last_used = time_utils::now();

                            state.set_state(__state_enum::Available);
                        } catch (const exception &e) {
                            lock_error le = LOCK_ERROR("Error creating lock instance. [error=%s]", e.what());
                            state.set_error(&le);
                            throw le;
                        } catch (...) {
                            lock_error le = LOCK_ERROR("Error creating lock instance. [error=Unknown]");
                            state.set_error(&le);
                            throw le;
                        }
                    }

                    void reset() {
                        CHECK_STATE_AVAILABLE(state);
                        lock->reset();
                    }

                    string get_txn_id() {
                        CHECK_STATE_AVAILABLE(state);
                        return txn_id;
                    }

                    bool has_write_lock(string thread_id) {
                        check_lock_valid();
                        CHECK_STATE_AVAILABLE(state);
                        lock_struct->last_used = time_utils::now();
                        pid_t pid = getpid();
                        if (lock_struct->write_locked && lock_struct->owner.process_id == pid) {
                            if (strncmp(lock_struct->owner.txn_id, txn_id.c_str(), txn_id.length()) == 0) {
                                return (strncmp(lock_struct->owner.thread_id, thread_id.c_str(), thread_id.length()) ==
                                        0);
                            }
                        }
                        return false;
                    }

                    bool has_thread_lock(string thread_id) {
                        check_lock_valid();
                        CHECK_STATE_AVAILABLE(state);
                        lock_struct->last_used = time_utils::now();
                        unordered_map<std::string, int>::const_iterator iter = reader_threads.find(thread_id);
                        if (iter != reader_threads.end()) {
                            int index = iter->second;
                            if (index >= 0) {
                                if (lock_struct->readers[index].used)
                                    return true;
                            }
                        }
                        return false;
                    }

                    int get_reader_index(string thread_id) {
                        CHECK_STATE_AVAILABLE(state);
                        unordered_map<std::string, int>::const_iterator iter = reader_threads.find(thread_id);
                        if (iter != reader_threads.end()) {
                            int index = iter->second;
                            if (index >= 0) {
                                if (lock_struct->readers[index].used)
                                    return index;
                            }
                        }
                        return -1;
                    }

                    string write_lock(string owner, uint64_t timeout) {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(!IS_EMPTY(owner));
                        PRECONDITION(IS_EMPTY(txn_id));

                        uint64_t startt = time_utils::now();
                        string thread_id = thread_utils::get_current_thread();
                        bool locked = false;
                        while (true) {
                            if (lock->try_lock()) {
                                if (has_write_lock(thread_id)) {
                                    locked = true;
                                } else if (!lock_struct->write_locked && lock_struct->reader_count == 0) {
                                    lock_struct->write_locked = true;
                                    lock_struct->owner.lock_timestamp = time_utils::now();
                                    memset(lock_struct->owner.owner, 0, SIZE_USER_NAME);
                                    strncpy(lock_struct->owner.owner, owner.c_str(), owner.length());
                                    lock_struct->owner.process_id = getpid();
                                    memset(lock_struct->owner.txn_id, 0, SIZE_UUID);
                                    txn_id = common_utils::uuid();
                                    strncpy(lock_struct->owner.txn_id, txn_id.c_str(), txn_id.length());
                                    memset(lock_struct->owner.thread_id, 0, SIZE_THREAD_ID);
                                    strncpy(lock_struct->owner.thread_id, thread_id.c_str(), thread_id.length());

                                    locked = true;
                                }
                                lock->release_lock();
                            }
                            if (locked) {
                                break;
                            }
                            uint64_t now = time_utils::now();
                            if ((now - startt) > timeout) {
                                break;
                            }
                            START_ALARM(DEFAULT_RW_LOCK_RETRY);
                        }
                        return txn_id;
                    }

                    string write_lock(string owner) {
                        return write_lock(owner, 0);
                    }

                    bool read_lock(uint64_t timeout) {
                        CHECK_STATE_AVAILABLE(state);

                        uint64_t startt = time_utils::now();
                        bool locked = false;
                        while (true) {
                            if (lock->try_lock()) {
                                if (!lock_struct->write_locked) {
                                    string thread_id = thread_utils::get_current_thread();
                                    if (!has_thread_lock(thread_id)) {
                                        if (lock_struct->reader_count < MAX_READER_LOCKS) {
                                            int index = find_free_reader();
                                            if (index >= 0) {
                                                lock_struct->readers[index].lock_timestamp = time_utils::now();
                                                lock_struct->readers[index].process_id = getpid();
                                                memset(lock_struct->readers[index].thread_id, 0, SIZE_THREAD_ID);
                                                strncpy(lock_struct->readers[index].thread_id, thread_id.c_str(),
                                                        thread_id.length());
                                                lock_struct->readers[index].used = true;
                                                reader_threads[thread_id] = index;

                                                lock_struct->reader_count++;

                                                locked = true;
                                            } else {
                                                throw BASE_ERROR("Error getting free read lock pointer. [name=%s]",
                                                                 lock_struct->name);
                                            }
                                        } else {
                                            locked = false;
                                        }
                                    } else {
                                        locked = true;
                                    }
                                }
                                lock->release_lock();
                            }
                            if (locked) {
                                break;
                            }
                            uint64_t now = time_utils::now();
                            if ((now - startt) > timeout) {
                                break;
                            }
                            START_ALARM(DEFAULT_RW_LOCK_RETRY);
                        }
                        return locked;
                    }

                    bool read_lock() {
                        return read_lock(0);
                    }

                    bool release_write_lock() {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(!IS_EMPTY(txn_id));

                        bool locked = false;
                        if (lock->wait_lock()) {
                            if (lock_struct->write_locked) {
                                string thread_id = thread_utils::get_current_thread();
                                if (has_write_lock(thread_id)) {
                                    lock_struct->owner.process_id = -1;
                                    lock_struct->owner.lock_timestamp = 0;
                                    memset(lock_struct->owner.txn_id, 0, SIZE_UUID);
                                    memset(lock_struct->owner.owner, 0, SIZE_USER_NAME);
                                    memset(lock_struct->owner.thread_id, 0, SIZE_THREAD_ID);
                                    lock_struct->write_locked = false;
                                    txn_id = EMPTY_STRING;
                                    locked = true;
                                }
                            }
                            lock->release_lock();
                        } else {
                            throw LOCK_ERROR("Error getting lock. [name=%s]", lock->get_name()->c_str());
                        }
                        return locked;
                    }

                    bool release_read_lock() {
                        CHECK_STATE_AVAILABLE(state);

                        string thread_id = thread_utils::get_current_thread();
                        bool locked = false;
                        return release_read_lock(thread_id);
                    }

                    string get_name() {
                        return this->name;
                    }

                    void increment_ref_count() {
                        reference_count++;
                    }

                    void decrement_ref_count() {
                        reference_count--;
                    }

                    uint32_t get_reference_count() {
                        return reference_count;
                    }
                };

                class lock_env {
                protected:
                    __state__ state;
                    mutex thread_mutex;
                    shared_lock_table *table = nullptr;
                    unordered_map<string, read_write_lock *> locks;

                    void create(mode_t mode, bool is_manager = false);

                public:
                    ~lock_env() {
                        std::lock_guard<std::mutex> guard(thread_mutex);
                        state.set_state(__state_enum::Disposed);
                        if (!locks.empty()) {
                            unordered_map<string, read_write_lock *>::iterator iter;
                            for (iter = locks.begin(); iter != locks.end(); iter++) {
                                read_write_lock *lock = iter->second;
                                if (NOT_NULL(lock)) {
                                    delete (lock);
                                }
                            }
                        }
                        CHECK_AND_FREE(table);
                    }

                    shared_lock_table *get_lock_table() {
                        CHECK_STATE_AVAILABLE(state);
                        return table;
                    }

                    void create();

                    read_write_lock *add_lock(string name);

                    bool remove_lock(string name);

                };

                class lock_manager : public lock_env {
                private:
                    thread *manager_thread = nullptr;

                    void check_lock_states();

                    static void run(lock_manager *manager);

                public:
                    ~lock_manager() {
                        state.set_state(__state_enum::Disposed);
                        if (NOT_NULL(manager_thread)) {
                            manager_thread->join();
                            delete (manager_thread);
                            manager_thread = nullptr;
                        }
                    }

                    void init(mode_t mode);

                    void reset();
                };

                class lock_env_utils {
                private:
                    static mutex env_mutex;
                    static bool is_manager;
                    static lock_env *env;
                public:
                    static lock_env *create_manager(mode_t mode) {
                        std::lock_guard<std::mutex> guard(env_mutex);
                        if (NOT_NULL(env) && !is_manager) {
                            throw LOCK_ERROR("Lock environment already initialized in client mode.");
                        }
                        if (is_manager && NOT_NULL(env)) {
                            return env;
                        }
                        is_manager = true;

                        lock_manager *manager = new lock_manager();
                        manager->init(mode);

                        env = manager;
                        return env;
                    }

                    static lock_env *create_client() {
                        std::lock_guard<std::mutex> guard(env_mutex);
                        if (NOT_NULL(env)) {
                            return env;
                        }
                        is_manager = false;
                        env = new lock_env();
                        env->create();

                        return env;
                    }

                    static lock_env *get() {
                        CHECK_NOT_NULL(env);
                        return env;
                    }

                    static lock_manager *get_manager() {
                        CHECK_NOT_NULL(env);
                        PRECONDITION(is_manager);

                        return static_cast<lock_manager *>(env);
                    }

                    static void dispose() {
                        std::lock_guard<std::mutex> guard(env_mutex);
                        CHECK_AND_FREE(env);
                    }
                };
            }
        }
    }
}
#endif //REACTFS_READ_WRITE_LOCK_H
