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

#include "shared_lock.h"

#define DEFAULT_RW_LOCK_EXPIRY 10 * 60 * 1000

REACTFS_NS_COMMON
                typedef __lock_readers_v0 __lock_readers;
                typedef __rw_lock_struct_v0 __rw_lock_struct;

                class rw_lock_table : public base_lock_table {
                private:
                    /// Shared Locks registry pointer.
                    __rw_lock_struct *locks = nullptr;

                public:
                    /*! <constructor
                     * Default constructor.
                     *
                     * @return
                     */
                    rw_lock_table(string group) : base_lock_table(group) {
                        version.major = SHARED_LOCK_MAJ_VERSION;
                        version.minor = SHARED_LOCK_MIN_VERSION;
                    }

                    /*! <destructor
                     *  Instance destructor.
                     */
                    ~rw_lock_table() {
                        locks = nullptr;
                    }

                    /*!
                     * Get the shared lock record at the specified index.
                     *
                     * @param index - Record index.
                     * @return - Lock record at index.
                     */
                    __rw_lock_struct *get_at(uint32_t index) {
                        PRECONDITION(NOT_NULL(locks));
                        PRECONDITION(NOT_NULL(header_ptr));
                        PRECONDITION(index >= 0 && index < header_ptr->max_count);

                        __rw_lock_struct *ptr = locks + index;
                        if (ptr->w_lock_struct.used) {
                            return ptr;
                        }
                        return nullptr;
                    }

                    /*!
                     * Create/Map the shared lock data to this instance.
                     *
                     * @param mode - Shared lock file create mode.
                     * @param is_manager - Is this instance a server instance?
                     * @param reset - Reset the lock table data.
                     */
                    void create(mode_t mode, bool is_manager, bool reset) {
                        __create(MAX_RW_SHARED_LOCKS, sizeof(__rw_lock_struct), mode, is_manager, reset);
                        void *ptr = get_data_ptr();
                        CHECK_NOT_NULL(ptr);
                        if (reset) {
                            memset(ptr, 0, MAX_RW_SHARED_LOCKS * sizeof(__rw_lock_struct));
                        }
                        locks = static_cast<__rw_lock_struct *>(ptr);
                        POSTCONDITION(NOT_NULL(locks));
                    }

                    /*!
                     * Map the shared lock data to this instance in client mode.
                     */
                    void create() {
                        __create(MAX_RW_SHARED_LOCKS, sizeof(__rw_lock_struct), 0, false, false);
                        void *ptr = get_data_ptr();
                        CHECK_NOT_NULL(ptr);

                        locks = static_cast<__rw_lock_struct *>(ptr);
                        POSTCONDITION(NOT_NULL(locks));
                    }

                    /*!
                     * Add a new shared lock. Will allocate the lock record if slot is available.
                     *
                     * @param name - Shared lock name.
                     * @param timeout - Lock add timeout, defaults to DEFAULT_WRITE_LOCK_TIMEOUT
                     * @return - Allocated shared lock record.
                     */
                    __rw_lock_struct *add_lock(string name, uint64_t timeout = DEFAULT_WRITE_LOCK_TIMEOUT);

                    /*!
                     * Remove the shared lock record, already registered.
                     *
                     * @param name - Shared lock name.
                     * @param timeout - Lock add timeout, defaults to DEFAULT_WRITE_LOCK_TIMEOUT
                     * @return - Has been removed?
                     */
                    bool remove_lock(string name, uint64_t timeout = DEFAULT_WRITE_LOCK_TIMEOUT);
                };

                /*!
                 * Class defines a Read/Write locking framework for inter-process locking.
                 *
                 * This implementation is based on a semaphore based exclusive lock and a
                 * SHM backed data structure to define read/write locking semantics.
                 */
                class read_write_lock {
                private:
                    /// State of this instance of the RW lock.
                    __state__ state;

                    /// Shared exclusive lock, used to control SHM updates.
                    exclusive_lock *lock;

                    /// SHM mapped structure for this lock instance.
                    __rw_lock_struct *lock_struct;

                    /// Current transaction ID, if in a locked mode.
                    string txn_id;

                    /// Thread records, to support reentrant locking.
                    unordered_map<string, int> reader_threads;

                    /// Pointer to the SHM mapped table.
                    rw_lock_table *table = nullptr;

                    /// Name of the shared lock.
                    string name;

                    /// NUmber of lock references active.
                    uint32_t reference_count = 0;

                    /*!
                     * Find a free slot to register a new thread.
                     *
                     * @return - Free slot index, or -1 if slots exhausted.
                     */
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


                    /*!
                     * Check if this process has a valid lock record and the record hasn't expired.
                     * This check is needed as the background server thread will release and reuse records
                     * that have expired due to inactivity.
                     */
                    void check_lock_valid() {
                        if (!lock_struct->w_lock_struct.used) {
                            lock_error e = LOCK_ERROR("Lock has been released by manager.");
                            state.set_error(&e);
                        } else if (strncmp(lock_struct->w_lock_struct.name, name.c_str(), name.length()) != 0) {
                            lock_error e = LOCK_ERROR("Lock has been released by manager and reassigned.");
                            state.set_error(&e);
                        }
                        uint64_t delta = (time_utils::now() - lock_struct->w_lock_struct.last_used);
                        if (delta >= DEFAULT_RW_LOCK_EXPIRY) {
                            lock_error e = LOCK_ERROR("Lock has expired.");
                            state.set_error(&e);
                        }
                    }

                    /*!
                     * Release a read lock, check is done to ensure that locks are not already released.
                     *
                     * @param thread_id - Current thread ID of the caller.
                     * @param timeout - Lock timeout, defaults to DEFAULT_LOCK_TIMEOUT
                     * @return - Is lock released?
                     */
                    bool release_read_lock(string thread_id, uint64_t timeout = DEFAULT_LOCK_TIMEOUT) {
                        bool locked = false;
                        TRY_LOCK_WITH_ERROR(lock, 0, timeout);
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
                        lock_struct->w_lock_struct.last_used = time_utils::now();
                        return locked;
                    }


                public:
                    /*!<constructor
                     *
                     * Default constructor.
                     * @return
                     */
                    read_write_lock() {

                    }

                    /*!<destructor
                     * Default destructor.
                     */
                    ~read_write_lock() {
                        state.set_state(__state_enum::Disposed);
                        check_and_clear();
                        lock_struct = nullptr;
                    }

                    /*!
                     * Create and initialize the lock instance.
                     *
                     * @param name - Shared lock name.
                     * @param table - SHM based lock table pointer.
                     */
                    void create(const string *name, rw_lock_table *table) {
                        try {
                            CHECK_NOT_NULL(table);
                            CHECK_NOT_EMPTY_P(name);

                            this->name = string(*name);
                            this->lock = table->get_lock();

                            this->table = table;
                            lock_struct = this->table->add_lock(*name);
                            POSTCONDITION(NOT_NULL(lock_struct));
                            lock_struct->w_lock_struct.last_used = time_utils::now();

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

                    /*!
                     * Check and clear any lock(s) acquired by this instance.
                     */
                    void check_and_clear() {
                        if (NOT_NULL(lock_struct)) {
                            if (lock_struct->w_lock_struct.used) {

                                pid_t pid = getpid();

                                // Check and release, if this instance currently holds a write lock.
                                if (lock_struct->w_lock_struct.write_locked &&
                                    lock_struct->w_lock_struct.owner.process_id == pid) {
                                    release_write_lock();
                                }
                                // Check if this instance has acquired any read locks.
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
                                    // Release all read locks acquired by this instance.
                                    for (vector<string>::iterator it = threads.begin(); it != threads.end(); ++it) {
                                        release_read_lock(*it);
                                    }
                                }
                            }
                        }
                    }

                    /*!
                     * Reset the exclusive lock.
                     *
                     * Note: Should be called only during server startup or
                     * when recovering from error conditions.
                     */
                    void reset() {
                        CHECK_STATE_AVAILABLE(state);
                        lock->reset();
                    }

                    /*!
                     * Get the current transaction ID.
                     * Will be empty if not currently holding a write lock.
                     *
                     * @return - Current transaction ID, empty if no write lock.
                     */
                    string get_txn_id() {
                        CHECK_STATE_AVAILABLE(state);
                        return txn_id;
                    }

                    /*!
                     * Check if this process holds the write lock,
                     * and the current thread is the locker.
                     *
                     * @param thread_id - Calling thread ID.
                     * @return - Has write lock?
                     */
                    bool has_write_lock(string thread_id) {
                        check_lock_valid();
                        CHECK_STATE_AVAILABLE(state);
                        lock_struct->w_lock_struct.last_used = time_utils::now();
                        pid_t pid = getpid();
                        if (lock_struct->w_lock_struct.write_locked &&
                            lock_struct->w_lock_struct.owner.process_id == pid) {
                            if (strncmp(lock_struct->w_lock_struct.owner.txn_id, txn_id.c_str(), txn_id.length()) ==
                                0) {
                                return (strncmp(lock_struct->w_lock_struct.owner.thread_id, thread_id.c_str(),
                                                thread_id.length()) ==
                                        0);
                            }
                        }
                        return false;
                    }

                    /*!
                     * Check if the calling thread already has a read lock.
                     *
                     * @param thread_id - Calling thread ID.
                     * @return - Has read lock?
                     */
                    bool has_thread_lock(string thread_id) {
                        check_lock_valid();
                        CHECK_STATE_AVAILABLE(state);
                        lock_struct->w_lock_struct.last_used = time_utils::now();
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

                    /*!
                     * Get the index of the thread record, if thread has already been registered.
                     *
                     * @param thread_id - Calling thread ID.
                     * @return - Thread record index or -1 if not found.
                     */
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

                    /*!
                     * Acquire a write lock. Will retry for the specified timeout.
                     * Lock calls are thread reentrant and will return true if the calling thread
                     * has already acquired a write lock.
                     *
                     * @param owner - Username of the requesting process owner.
                     * @param timeout - Timeout for retrying.
                     * @return - New transaction ID, or empty string if acquire failed.
                     */
                    string write_lock(string owner, uint64_t timeout) {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(!IS_EMPTY(owner));
                        PRECONDITION(IS_EMPTY(txn_id));

                        uint64_t startt = time_utils::now();
                        string thread_id = thread_utils::get_current_thread();
                        bool locked = false;

                        NEW_ALARM(DEFAULT_RW_LOCK_RETRY, 0);
                        while (true) {
                            bool ret = false;
                            TRY_LOCK(lock, 0, timeout, ret);
                            if (ret) {
                                if (has_write_lock(thread_id)) {
                                    locked = true;
                                } else if (!lock_struct->w_lock_struct.write_locked && lock_struct->reader_count == 0) {
                                    lock_struct->w_lock_struct.write_locked = true;
                                    lock_struct->w_lock_struct.owner.lock_timestamp = time_utils::now();
                                    memset(lock_struct->w_lock_struct.owner.owner, 0, SIZE_USER_NAME);
                                    strncpy(lock_struct->w_lock_struct.owner.owner, owner.c_str(), owner.length());
                                    lock_struct->w_lock_struct.owner.process_id = getpid();
                                    memset(lock_struct->w_lock_struct.owner.txn_id, 0, SIZE_UUID);
                                    txn_id = common_utils::uuid();
                                    strncpy(lock_struct->w_lock_struct.owner.txn_id, txn_id.c_str(), txn_id.length());
                                    memset(lock_struct->w_lock_struct.owner.thread_id, 0, SIZE_THREAD_ID);
                                    strncpy(lock_struct->w_lock_struct.owner.thread_id, thread_id.c_str(),
                                            thread_id.length());

                                    locked = true;
                                }
                            }
                            if (locked) {
                                break;
                            }
                            if ((time_utils::now() - startt) > timeout) {
                                break;
                            }
                            START_ALARM(0);
                        }
                        return txn_id;
                    }

                    /*!
                     * Get a write lock, without waiting.
                     * Will return immediately if lock is not acquired.
                     *
                     * @param owner - Username of the requesting process owner.
                     * @return - New transaction ID, or empty string if acquire failed.
                     */
                    string write_lock(string owner) {
                        return write_lock(owner, 0);
                    }

                    /*!
                     * Get a read lock on behalf of the calling thread.
                     * Lock calls are thread reentrant and will return true if the calling thread
                     * has already acquired a read lock.
                     *
                     * @param timeout - Retry timeout.
                     * @return - Is read lock acquired?
                     */
                    bool read_lock(uint64_t timeout) {
                        CHECK_STATE_AVAILABLE(state);

                        string thread_id = thread_utils::get_current_thread();

                        uint64_t startt = time_utils::now();
                        bool locked = false;
                        NEW_ALARM(DEFAULT_RW_LOCK_RETRY, 0);
                        while (true) {
                            bool ret = false;
                            TRY_LOCK(lock, 0, timeout, ret);
                            if (ret) {
                                if (!lock_struct->w_lock_struct.write_locked) {
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
                                                                 lock_struct->w_lock_struct.name);
                                            }
                                        } else {
                                            locked = false;
                                        }
                                    } else {
                                        locked = true;
                                    }
                                }
                            }
                            if (locked) {
                                break;
                            }
                            uint64_t now = time_utils::now();
                            if ((now - startt) > timeout) {
                                break;
                            }
                            START_ALARM(0);
                        }
                        return locked;
                    }

                    /*!
                    * Get a read lock on behalf of the calling thread.
                    * Lock calls are thread reentrant and will return true if the calling thread
                    * has already acquired a read lock.
                    *
                    * @return - Is read lock acquired?
                    */
                    bool read_lock() {
                        return read_lock(0);
                    }

                    /*!
                     * Release the write lock currently held by this process/thread.
                     * Is thread safe and will not release if lock has been acquired by a different thread.
                     *
                     * @return - Has been released?
                     */
                    bool release_write_lock() {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(!IS_EMPTY(txn_id));

                        TRY_LOCK_WITH_ERROR(lock, 0, DEFAULT_LOCK_TIMEOUT);
                        bool locked = false;
                        if (lock_struct->w_lock_struct.write_locked) {
                            string thread_id = thread_utils::get_current_thread();
                            if (has_write_lock(thread_id)) {
                                lock_struct->w_lock_struct.owner.process_id = -1;
                                lock_struct->w_lock_struct.owner.lock_timestamp = 0;
                                memset(lock_struct->w_lock_struct.owner.txn_id, 0, SIZE_UUID);
                                memset(lock_struct->w_lock_struct.owner.owner, 0, SIZE_USER_NAME);
                                memset(lock_struct->w_lock_struct.owner.thread_id, 0, SIZE_THREAD_ID);
                                lock_struct->w_lock_struct.write_locked = false;
                                txn_id = common_consts::EMPTY_STRING;
                                locked = true;
                            }
                        }
                        return locked;
                    }

                    /*!
                    * Release the read lock currently held by this process/thread.
                    * Is thread safe and will not release if lock has been acquired by a different thread.
                    *
                    * @return - Has been released?
                    */
                    bool release_read_lock() {
                        CHECK_STATE_AVAILABLE(state);

                        string thread_id = thread_utils::get_current_thread();
                        bool locked = false;
                        return release_read_lock(thread_id);
                    }

                    /*!
                     * Get the name of this shared read/write lock.
                     *
                     * @return - Lock name.
                     */
                    string get_name() {
                        return this->name;
                    }


                    /*!
                     * Increment the reference count for this lock instance.
                     */
                    uint32_t increment_ref_count() {
                        return ++reference_count;
                    }

                    /*!
                     * Decrement the reference count for this lock instance.
                     */
                    uint32_t decrement_ref_count() {
                        if (reference_count > 0)
                            return --reference_count;
                        TRACE("[name=%s] Current lock reference count = %lu", this->name.c_str(), reference_count);
                        return reference_count;
                    }

                    /*!
                     * Get the reference count for this lock instance.
                     *
                     * @return - Reference count.
                     */
                    uint32_t get_reference_count() {
                        return reference_count;
                    }
                };
REACTFS_NS_COMMON_END
#endif //REACTFS_READ_WRITE_LOCK_H
