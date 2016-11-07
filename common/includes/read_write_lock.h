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

#include "exclusive_lock.h"
#include "timer.h"
#include "__alarm.h"
#include "process_utils.h"
#include "unordered_map"

#define DEFAULT_RW_LOCK_RETRY 10
#define SIZE_LOCK_NAME 64
#define MAX_READER_LOCKS 64

using namespace com::watergate::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                typedef struct __owner__ {
                    char owner[SIZE_USER_NAME];
                    char txn_id[SIZE_UUID];
                    pid_t process_id;
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
                    bool write_locked = false;
                    __owner owner;
                    uint64_t reader_count;
                    __lock_readers readers[MAX_READER_LOCKS];
                } __lock_struct;

                class read_write_lock {
                private:
                    exclusive_lock *lock;
                    __lock_struct *lock_struct;
                    string txn_id;
                    unordered_map<string, int> reader_threads;

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
                    }

                    bool release_read_lock(string thread_id) {
                        bool locked = false;
                        while (true) {
                            if (lock->try_lock()) {
                                if (lock_struct->reader_count > 0) {
                                    int index = get_reader_index(thread_id);
                                    if (index >= 0) {
                                        lock_struct->reader_count--;
                                        lock_struct->readers[index].used = false;

                                        reader_threads.erase(thread_id);
                                        locked = true;
                                    }
                                }
                                lock->release_lock();
                                break;
                            }
                            START_ALARM(DEFAULT_RW_LOCK_RETRY);
                        }
                        return locked;
                    }

                public:
                    read_write_lock(const string *name) {
                        lock = new exclusive_lock(name);
                    }

                    ~read_write_lock() {
                        check_and_clear();

                        CHECK_AND_FREE(lock);
                        lock_struct = nullptr;
                    }

                    string get_txn_id() {
                        CHECK_NOT_NULL(lock);

                        return txn_id;
                    }

                    bool has_write_lock() {
                        PRECONDITION(NOT_NULL(lock));
                        PRECONDITION(NOT_NULL(lock_struct));
                        pid_t pid = getpid();
                        if (lock_struct->write_locked && lock_struct->owner.process_id == pid) {
                            return (strncmp(lock_struct->owner.txn_id, txn_id.c_str(), txn_id.length()) == 0);
                        }
                        return false;
                    }

                    bool has_thread_lock(string thread_id) {
                        unordered_map<std::string, int>::const_iterator iter = reader_threads.find(thread_id);
                        if (iter != reader_threads.end()) {
                            return true;
                        }
                        return false;
                    }

                    int get_reader_index(string thread_id) {
                        unordered_map<std::string, int>::const_iterator iter = reader_threads.find(thread_id);
                        if (iter != reader_threads.end()) {
                            return iter->second;
                        }
                        return -1;
                    }

                    string write_lock(string owner, uint64_t timeout) {
                        CHECK_NOT_NULL(lock);
                        PRECONDITION(!IS_EMPTY(owner));
                        PRECONDITION(IS_EMPTY(txn_id));

                        uint64_t startt = time_utils::now();
                        bool locked = false;
                        while (true) {
                            if (lock->try_lock()) {
                                if (!lock_struct->write_locked && lock_struct->reader_count == 0) {
                                    lock_struct->write_locked = true;

                                    lock_struct->owner.lock_timestamp = time_utils::now();
                                    memset(lock_struct->owner.owner, SIZE_USER_NAME, 0);
                                    strncpy(lock_struct->owner.owner, owner.c_str(), owner.length());
                                    lock_struct->owner.process_id = getpid();
                                    memset(lock_struct->owner.txn_id, 0, SIZE_UUID);
                                    txn_id = common_utils::uuid();
                                    strncpy(lock_struct->owner.txn_id, txn_id, txn_id.length());

                                    locked = true;
                                }
                                lock->release_lock();
                            }
                            if (locked) {
                                break;
                            }
                            uint64_t now = time_utils::now();
                            if (startt - now > timeout) {
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
                        CHECK_NOT_NULL(lock);

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
                            if (startt - now > timeout) {
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
                        CHECK_NOT_NULL(lock);
                        PRECONDITION(!IS_EMPTY(txn_id));

                        bool locked = false;
                        while (true) {
                            if (lock->try_lock()) {
                                if (lock_struct->write_locked) {
                                    pid_t pid = getpid();
                                    if (lock_struct->owner.process_id == pid &&
                                        strncmp(lock_struct->owner.txn_id, txn_id.c_str(), txn_id.length()) == 0) {
                                        lock_struct->owner.process_id = -1;
                                        lock_struct->owner.lock_timestamp = 0;
                                        memset(lock_struct->owner.txn_id, 0, SIZE_UUID);
                                        memset(lock_struct->owner.owner, 0, SIZE_USER_NAME);

                                        lock_struct->write_locked = false;

                                        txn_id = EMPTY_STRING;
                                        locked = true;
                                    }
                                }
                                lock->release_lock();
                                break;
                            }
                            START_ALARM(DEFAULT_RW_LOCK_RETRY);
                        }
                        return locked;
                    }

                    bool release_read_lock() {
                        CHECK_NOT_NULL(lock);

                        string thread_id = thread_utils::get_current_thread();
                        bool locked = false;
                        return release_read_lock(thread_id);
                    }
                };
            }
        }
    }
}
#endif //REACTFS_READ_WRITE_LOCK_H
