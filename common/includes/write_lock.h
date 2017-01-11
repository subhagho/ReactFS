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

#ifndef REACTFS_WRITE_LOCK_H
#define REACTFS_WRITE_LOCK_H

#include <mutex>

#include "shared_lock.h"

#define DEFAULT_W_LOCK_EXPIRY 10 * 60 * 1000
#define CHECK_LOCK_AVAILABLE (state.get_state() != __lock_state_enum::LOCK_UNKNOWN && state.get_state() != __lock_state_enum::LOCK_ERROR)
#define CHECK_LOCK_FREE (state.get_state() == __lock_state_enum::LOCK_FREE)
#define CHECK_LOCK_LOCKED (state.get_state() == __lock_state_enum::LOCK_LOCKED)

REACTFS_NS_COMMON
                typedef __lock_readers_v0 __lock_readers;
                typedef __rw_lock_struct_v0 __rw_lock_struct;

                class w_lock_table : public base_lock_table {
                private:
                    /// Shared Locks registry pointer.
                    __w_lock_struct *locks = nullptr;

                public:
                    /*! <constructor
                     * Default constructor.
                     *
                     * @return
                     */
                    w_lock_table(string group) : base_lock_table(group) {
                        version.major = SHARED_LOCK_MAJ_VERSION;
                        version.minor = SHARED_LOCK_MIN_VERSION;
                    }

                    /*! <destructor
                     *  Instance destructor.
                     */
                    ~w_lock_table() {
                        locks = nullptr;
                    }

                    /*!
                     * Get the shared lock record at the specified index.
                     *
                     * @param index - Record index.
                     * @return - Lock record at index.
                     */
                    __w_lock_struct *get_at(uint32_t index) {
                        PRECONDITION(NOT_NULL(locks));
                        PRECONDITION(NOT_NULL(header_ptr));
                        PRECONDITION(index >= 0 && index < header_ptr->max_count);

                        __w_lock_struct *ptr = locks + index;
                        if (ptr->used) {
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
                        __create(MAX_W_SHARED_LOCKS, sizeof(__w_lock_struct), mode, is_manager, reset);
                        void *ptr = get_data_ptr();
                        CHECK_NOT_NULL(ptr);
                        if (reset) {
                            memset(ptr, 0, MAX_W_SHARED_LOCKS * sizeof(__w_lock_struct));
                        }
                        locks = static_cast<__w_lock_struct *>(ptr);
                        POSTCONDITION(NOT_NULL(locks));
                    }

                    /*!
                     * Map the shared lock data to this instance in client mode.
                     */
                    void create() {
                        __create(MAX_W_SHARED_LOCKS, sizeof(__w_lock_struct), 0, false, false);
                        void *ptr = get_data_ptr();
                        CHECK_NOT_NULL(ptr);

                        locks = static_cast<__w_lock_struct *>(ptr);
                        POSTCONDITION(NOT_NULL(locks));
                    }

                    /*!
                     * Add a new shared lock. Will allocate the lock record if slot is available.
                     *
                     * @param name - Shared lock name.
                     * @param timeout - Lock add timeout, defaults to DEFAULT_WRITE_LOCK_TIMEOUT
                     * @return - Allocated shared lock record.
                     */
                    __w_lock_struct *add_lock(string name, uint64_t timeout = DEFAULT_WRITE_LOCK_TIMEOUT);

                    /*!
                     * Remove the shared lock record, already registered.
                     *
                     * @param name - Shared lock name.
                     * @param timeout - Lock add timeout, defaults to DEFAULT_WRITE_LOCK_TIMEOUT
                     * @return - Has been removed?
                     */
                    bool remove_lock(string name, uint64_t timeout = DEFAULT_WRITE_LOCK_TIMEOUT);
                };


                class __w_lock_state {
                private:
                    __lock_state_enum state = __lock_state_enum::LOCK_UNKNOWN;
                    const exception *error = nullptr;

                public:
                    void set_state(__lock_state_enum state) {
                        this->state = state;
                    }

                    __lock_state_enum get_state() const {
                        return this->state;
                    }

                    void set_error(const exception *error) {
                        this->state = __lock_state_enum::LOCK_ERROR;
                        this->error = error;
                    }

                    const exception *get_error() {
                        return this->error;
                    }

                    bool is_available() const {
                        return (state == __lock_state_enum::LOCK_FREE);
                    }

                    bool is_locked() const {
                        return (state == __lock_state_enum::LOCK_LOCKED);
                    }

                    bool has_error() const {
                        return (state == __lock_state_enum::LOCK_ERROR);
                    }
                };

                /*!
                 * Class defines a Write locking framework for inter-process locking.
                 *
                 * This implementation is based on a semaphore based exclusive lock and a
                 * SHM backed data structure to define write locking semantics.
                 */
                class write_lock {
                protected:
                    /// Mutex to control lock state changes.
                    mutex __thread_lock;

                    /// State of this instance of the RW lock.
                    __w_lock_state state;

                    /// Shared exclusive lock, used to control SHM updates.
                    exclusive_lock *lock;

                    /// SHM mapped structure for this lock instance.
                    __w_lock_struct *lock_struct;

                    /// Current transaction ID, if in a locked mode.
                    string txn_id;

                    /// Pointer to the SHM mapped table.
                    w_lock_table *table = nullptr;

                    /// Name of the shared lock.
                    string name;

                    /// NUmber of lock references active.
                    uint32_t reference_count = 0;


                    /*!
                     * Check if this process has a valid lock record and the record hasn't expired.
                     * This check is needed as the background server thread will release and reuse records
                     * that have expired due to inactivity.
                     */
                    virtual bool check_lock_valid() {
                        bool r = true;
                        if (!lock_struct->used) {
                            clear_lock_reset();
                            r = false;
                        } else if (strncmp(lock_struct->name, name.c_str(), name.length()) != 0) {
                            clear_lock_reset();
                            r = false;
                        }
                        uint64_t delta = (time_utils::now() - lock_struct->last_used);
                        if (delta >= DEFAULT_W_LOCK_EXPIRY) {
                            std::lock_guard<std::mutex> lock(__thread_lock);
                            state.set_state(__lock_state_enum::LOCK_EXPIRED);
                            r = false;
                        }
                        return r;
                    }

                    /*!
                     * Check and clear any lock(s) acquired by this instance.
                     */
                    virtual void check_and_clear() {
                        if (NOT_NULL(lock_struct)) {
                            if (lock_struct->used) {

                                pid_t pid = getpid();

                                // Check and release, if this instance currently holds a write lock.
                                if (lock_struct->write_locked &&
                                    lock_struct->owner.process_id == pid) {
                                    release_lock();
                                }
                            }
                        }
                    }

                    /*!
                     * Set the lock state to indicate that it has been reset.
                     */
                    void clear_lock_reset() {
                        std::lock_guard<std::mutex> lock(__thread_lock);
                        state.set_state(__lock_state_enum::LOCK_RESET);
                        this->lock_struct = nullptr;
                    }

                    /*!
                     * Register this lock in the global lock table.
                     */
                    void register_lock() {
                        lock_struct = this->table->add_lock(this->name);
                        POSTCONDITION(NOT_NULL(lock_struct));
                        lock_struct->last_used = time_utils::now();
                    }

                public:
                    /*!<constructor
                     *
                     * Default constructor.
                     * @return
                     */
                    write_lock() {
                    }

                    /*!<destructor
                     * Default destructor.
                     */
                    virtual ~write_lock() {
                        state.set_state(__lock_state_enum::LOCK_UNKNOWN);
                        check_and_clear();
                        lock_struct = nullptr;
                    }

                    /*!
                     * Create and initialize the lock instance.
                     *
                     * @param name - Shared lock name.
                     * @param table - SHM based lock table pointer.
                     */
                    void create(const string *name, w_lock_table *table) {
                        std::lock_guard<std::mutex> lock(__thread_lock);
                        if (CHECK_LOCK_AVAILABLE) {
                            return;
                        }
                        try {
                            CHECK_NOT_NULL(table);
                            CHECK_NOT_EMPTY_P(name);

                            this->name = string(*name);
                            this->lock = table->get_lock();

                            this->table = table;
                            register_lock();
                            state.set_state(__lock_state_enum::LOCK_FREE);
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
                     * Register this lock again.
                     *
                     * Should be called when the lock has been reset due to inactivity.
                     */
                    void check_and_register() {
                        PRECONDITION(CHECK_LOCK_AVAILABLE);
                        std::lock_guard<std::mutex> lock(__thread_lock);
                        if (CHECK_LOCK_FREE || CHECK_LOCK_LOCKED) {
                            return;
                        }
                        if (state.get_state() == __lock_state_enum::LOCK_RESET) {
                            try {
                                register_lock();
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
                    }

                    /*!
                     * Check if the lock is free to be acquired.
                     *
                     * @return - Is free?
                     */
                    bool is_free() {
                        return CHECK_LOCK_FREE;
                    }

                    /*!
                     * Has this lock instance been reset due to inactivity?
                     *
                     * @return - Has been reset?
                     */
                    bool is_lock_reset() {
                        PRECONDITION(CHECK_LOCK_AVAILABLE);
                        return (state.get_state() == __lock_state_enum::LOCK_RESET);
                    }

                    /*!
                     * Reset the exclusive lock.
                     *
                     * Note: Should be called only during server startup or
                     * when recovering from error conditions.
                     */
                    void reset() {
                        PRECONDITION(CHECK_LOCK_AVAILABLE);
                        lock->reset();
                    }

                    /*!
                     * Get the current transaction ID.
                     * Will be empty if not currently holding a write lock.
                     *
                     * @return - Current transaction ID, empty if no write lock.
                     */
                    string get_txn_id() {
                        PRECONDITION(CHECK_LOCK_AVAILABLE);
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
                        PRECONDITION(CHECK_LOCK_AVAILABLE);
                        if (!check_lock_valid()) {
                            TRACE("Lock is no longer valid.");
                            return false;
                        }
                        bool ret = false;
                        lock_struct->last_used = time_utils::now();
                        pid_t pid = getpid();
                        if (lock_struct->write_locked) {
                            if (lock_struct->owner.process_id == pid) {
                                if (strncmp(lock_struct->owner.txn_id, txn_id.c_str(), txn_id.length()) ==
                                    0) {
                                    int r = strncmp(lock_struct->owner.thread_id, thread_id.c_str(),
                                                    thread_id.length());
                                    if (r == 0) {
                                        lock_struct->owner.last_updated = lock_struct->last_used;
                                        ret = true;
                                    } else {
                                        TRACE("Thread owner doesn't match. [owner=%s][current=%s]", thread_id.c_str(),
                                              lock_struct->owner.thread_id);
                                    }
                                } else {
                                    TRACE("TXN ID doesn't match. [txid=%s][current=%s]", txn_id.c_str(),
                                          lock_struct->owner.txn_id);
                                }
                            } else {
                                TRACE("PID doesn't match. [pid=%d][current=%d]", pid, lock_struct->owner.process_id);
                            }
                        } else {
                            TRACE("Write lock reset due to expiry.");
                        }
                        return ret;
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
                    string get_lock(string owner, uint64_t timeout, const string &tid = common_consts::EMPTY_STRING) {
                        PRECONDITION(CHECK_LOCK_AVAILABLE);
                        PRECONDITION(!IS_EMPTY(owner));

                        uint64_t startt = time_utils::now();
                        string thread_id = thread_utils::get_current_thread();
                        bool locked = false;

                        NEW_ALARM(DEFAULT_RW_LOCK_RETRY, 0);
                        while (true) {
                            bool ret = false;
                            TRY_LOCK(lock, 0, timeout, ret);
                            if (ret) {
                                if (has_write_lock(thread_id)) {
                                    if (IS_EMPTY(tid)) {
                                        locked = true;
                                    } else if (tid == txn_id) {
                                        locked = true;
                                    } else {
                                        break;
                                    }
                                } else if (!lock_struct->write_locked) {
                                    lock_struct->write_locked = true;
                                    memset(lock_struct->owner.owner, 0, SIZE_MAX_NAME);
                                    strncpy(lock_struct->owner.owner, owner.c_str(), owner.length());
                                    lock_struct->owner.process_id = getpid();
                                    memset(lock_struct->owner.txn_id, 0, SIZE_UUID);
                                    if (IS_EMPTY(tid))
                                        txn_id = common_utils::uuid();
                                    else
                                        txn_id = string(tid);
                                    strncpy(lock_struct->owner.txn_id, txn_id.c_str(), txn_id.length());
                                    memset(lock_struct->owner.thread_id, 0, SIZE_THREAD_ID);
                                    strncpy(lock_struct->owner.thread_id, thread_id.c_str(),
                                            thread_id.length());
                                    lock_struct->owner.lock_timestamp = time_utils::now();
                                    lock_struct->owner.last_updated = lock_struct->owner.lock_timestamp;
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
                    string get_lock(string owner) {
                        return get_lock(owner, 0);
                    }

                    /*!
                     * Release the write lock currently held by this process/thread.
                     * Is thread safe and will not release if lock has been acquired by a different thread.
                     *
                     * @return - Has been released?
                     */
                    bool release_lock() {
                        PRECONDITION(CHECK_LOCK_AVAILABLE);
                        PRECONDITION(!IS_EMPTY(txn_id));

                        TRY_LOCK_WITH_ERROR(lock, 0, DEFAULT_LOCK_TIMEOUT);
                        bool locked = false;
                        if (lock_struct->write_locked) {
                            string thread_id = thread_utils::get_current_thread();
                            if (has_write_lock(thread_id)) {
                                lock_struct->owner.process_id = -1;
                                lock_struct->owner.lock_timestamp = 0;
                                memset(lock_struct->owner.txn_id, 0, SIZE_UUID);
                                memset(lock_struct->owner.owner, 0, SIZE_MAX_NAME);
                                memset(lock_struct->owner.thread_id, 0, SIZE_THREAD_ID);
                                lock_struct->write_locked = false;
                                txn_id = common_consts::EMPTY_STRING;
                                locked = true;
                            }
                        }
                        return locked;
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
                            reference_count--;
                        TRACE("[name=%s] Updated lock reference count = %d", this->name.c_str(), reference_count);
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
#endif //REACTFS_WRITE_LOCK_H
