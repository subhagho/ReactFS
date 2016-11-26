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
// Created by Subhabrata Ghosh on 21/09/16.
//

#ifndef WATERGATE_EXCLUSIVE_LOCK_H
#define WATERGATE_EXCLUSIVE_LOCK_H

#include <semaphore.h>

#include "common/includes/common.h"
#include "common/includes/base_error.h"
#include "common/includes/common_utils.h"
#include "common/includes/log_utils.h"
#include "common/includes/process_utils.h"
#include "common/includes/__alarm.h"
#include "common/includes/timer.h"

#define DEFAULT_LOCK_MODE 0760 /// Default lock create mode.
#define DEFAULT_LOCK_RETRY_TIME 500
#define DEFAULT_LOCK_TIMEOUT 30 * 1000 /// Default lock timeout is 30 secs.
#define EXCLUSIVE_LOCK_PREFIX "/ExLk"

#define CONST_LOCK_ERROR_PREFIX "Lock Error : "

#define LOCK_ERROR(fmt, ...) lock_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define LOCK_ERROR_PTR(fmt, ...) new lock_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))


#define CHECK_SEMAPHORE_PTR(s, name) do { \
    if (IS_NULL(s) || s == SEM_FAILED) { \
        throw LOCK_ERROR("Semaphore is not valid. [name=%s]", name->c_str()); \
    } \
} while(0);

#define WAIT_LOCK_GUARD(lock, i) com::wookler::reactfs::common::exclusive_lock_guard __guard_##i(lock, true);

#define RELEASE_LOCK_GUARD(i) __guard_##i.release();

#define TRY_LOCK_WITH_ERROR(lock, i, t) \
    com::wookler::reactfs::common::exclusive_lock_guard __guard_##i(lock, false); \
    if (!__guard_##i.get_lock(t)) { \
        throw com::wookler::reactfs::common::lock_timeout_error(__FILE__, __LINE__, __guard_##i.get_lock_name().c_str()); \
    }
#define TRY_LOCK(lock, i, t, b) \
    com::wookler::reactfs::common::exclusive_lock_guard __guard_##i(lock, false); \
    b = __guard_##i.get_lock(t);

#define INIT_LOCK_P(var, name) do {\
    var = new com::wookler::reactfs::common::exclusive_lock(name); \
    var->__create(); \
    CHECK_NOT_NULL(var); \
} while(0);

#define CREATE_LOCK_P(var, name, mode) do {\
    var = new com::wookler::reactfs::common::exclusive_lock(name, mode); \
    var->__create(); \
    CHECK_NOT_NULL(var); \
} while(0);

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                /*!
                 * Exception type raised due to lock related errors.
                 */
                class lock_error : public base_error {
                public:
                    /*!<constructor
                     *
                     * @param file - Source file name
                     * @param line - Source line number.
                     * @param mesg - Error Message
                     * @return
                     */
                    lock_error(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                           CONST_LOCK_ERROR_PREFIX,
                                                                                           mesg) {
                    }
                };

                class lock_timeout_error : public base_error {
                public:
                    /*!<constructor
                     * Exception raised when a lock acquire failed.
                     *
                     * @param file - Source file name
                     * @param line - Source line number.
                     * @param lock_name - Name of the lock which was being acquired.
                     * @return
                     */
                    lock_timeout_error(char const *file, const int line, const string &lock_name)
                            : base_error(file,
                                         line,
                                         CONST_LOCK_ERROR_PREFIX,
                                         common_utils::format("Timeout while acquiring lock. [lock=%s]",
                                                              lock_name.c_str())) {
                    }
                };

                /*!
                 * Semaphore base exclusive lock implementation. Only one process/thread can hold this
                 * lock at any given point of time.
                 *
                 * The locks are reentrant, hence the same thread calling lock/release will not cause problems.
                 */
                class exclusive_lock {
                private:
                    /// Unique name associated with this lock. Since the locks are shared between
                    /// processes, this name should be unique per machine.
                    string *name = nullptr;
                    /// More with which this lock should be created.
                    mode_t mode = DEFAULT_LOCK_MODE;
                    /// Semaphore handle backing this lock.
                    sem_t *semaphore = nullptr;
                    /// Thread ID of the thread that has obtained this lock.
                    string lock_owner;
                    /// Is this lock currently acquired.
                    bool locked = false;

                    /*!
                     * Set this lock has been successfully acquired by the current thread.
                     */
                    void set_locked() {
                        locked = true;
                        lock_owner = thread_utils::get_current_thread();
                    }

                    /*!
                     * Reset the lock owner info.
                     */
                    void reset_locked() {
                        locked = false;
                        lock_owner = EMPTY_STRING;
                    }

                public:
                    /*!<constructor
                     *
                     * Create a new instance of a shared lock with the specified name.
                     *
                     * @param name - Unique lock name.
                     * @return
                     */
                    exclusive_lock(const string *name) {
                        string ss = common_utils::get_normalized_name(*name);
                        PRECONDITION(!IS_EMPTY(ss));
                        this->name = new string(EXCLUSIVE_LOCK_PREFIX);
                        this->name->append(ss);
                    }

                    /*!<constructor
                     *
                     * Create a new instance of a shared lock with the specified name.
                     *
                     * @param name - Unique lock name.
                     * @param mode - Lock create mode.
                     * @return
                     */
                    exclusive_lock(const string *name, mode_t mode) {
                        string ss = common_utils::get_normalized_name(*name);
                        PRECONDITION(!IS_EMPTY(ss));
                        this->name = new string(EXCLUSIVE_LOCK_PREFIX);
                        this->name->append(ss);
                        this->mode = mode;
                    }

                    /*!<destructor
                     * Dispose this instance of the shared lock.
                     */
                    ~exclusive_lock() {
                        if (locked) {
                            __release_lock();
                        }
                        if (NOT_NULL(semaphore) && semaphore != SEM_FAILED) {
                            if (sem_close(semaphore) != 0) {
                                LOG_ERROR("Error closing semaphore. [name=%s][errno=%s]", name, strerror(errno));
                            }
                            semaphore = nullptr;
                        }
                        CHECK_AND_FREE(name);
                    }

                    /*!
                     * Check if the lock has been acquired and owned by the current thread.
                     *
                     * @return - Is acquired and owned?
                     */
                    bool is_locked() {
                        if (locked) {
                            string tid = thread_utils::get_current_thread();
                            return (tid == lock_owner);
                        }
                        return false;
                    }

                    /*!
                     * Reset the backing semaphore. Should only be used during system startup or in case of recovering
                     * from error state.
                     */
                    void reset() {
                        CHECK_SEMAPHORE_PTR(semaphore, name);
                        if (sem_trywait(semaphore) == 0) {
                            if (sem_post(semaphore) != 0) {
                                lock_error e = LOCK_ERROR("Error releaseing semaphore lock. [name=%s][errno=%s]",
                                                          name->c_str(), strerror(errno));
                                LOG_ERROR(e.what());
                                throw e;
                            }
                        } else {
                            if (sem_post(semaphore) != 0) {
                                lock_error e = LOCK_ERROR("Error releaseing semaphore lock. [name=%s][errno=%s]",
                                                          name->c_str(), strerror(errno));
                                LOG_ERROR(e.what());
                                throw e;
                            }
                        }
                        reset_locked();
                    }

                    /*!
                     * Create/Initialize this instance of the shared lock.
                     */
                    void __create() {
                        semaphore = sem_open(name->c_str(), O_CREAT, mode, 1);
                        if (IS_NULL(semaphore) || semaphore == SEM_FAILED) {
                            lock_error e = LOCK_ERROR("Error creating lock. [name=%s][errno=%s]", name->c_str(),
                                                      strerror(errno));
                            LOG_ERROR(e.what());
                            throw e;
                        }
                        reset_locked();
                        LOG_DEBUG("Created exclusive lock. [name=%s]", name->c_str());
                    }

                    /*!
                     * Get the name of this shared lock.
                     *
                     * @return - Lock name.
                     */
                    const string *get_name() const {
                        return name;
                    }

                    /*!
                     * Try to acquire this lock. Will return immediately if acquire fails.
                     *
                     * @return - Is acquired?
                     */
                    bool __try_lock() {
                        if (is_locked()) {
                            return true;
                        }
                        CHECK_SEMAPHORE_PTR(semaphore, name);
                        if (sem_trywait(semaphore) == 0) {
                            set_locked();
                            return true;
                        } else {
                            return false;
                        }
                    }

                    /*!
                     * Try to acquire this lock. Will retry till the timeout is exhausted.
                     *
                     * @param timeout - Time for which to retry.
                     * @return - Is acquired?
                     */
                    bool __try_lock(uint64_t timeout) {
                        NEW_ALARM(DEFAULT_LOCK_RETRY_TIME, 0);
                        long rem = timeout;
                        uint64_t s_time = time_utils::now();
                        while (rem >= 0) {
                            if (__try_lock()) {
                                return true;
                            }
                            START_ALARM(0);
                            uint64_t delta = (time_utils::now() - s_time);
                            rem = timeout - delta;
                        }
                        return false;
                    }

                    /*!
                     * Acquire this lock, wait till the lock is acquired.
                     */
                    bool __wait_lock() {
                        if (is_locked()) {
                            return true;
                        }
                        CHECK_SEMAPHORE_PTR(semaphore, name);
                        if (sem_wait(semaphore) == 0) {
                            set_locked();
                            return true;
                        } else {
                            throw LOCK_ERROR("Error while getting lock. [name=%s][error=%s]",
                                             this->name->c_str(), strerror(errno));
                        }
                    }

                    /*!
                     * Release this lock, if acquired.
                     * @return - Has been released.
                     */
                    bool __release_lock() {
                        if (!is_locked()) {
                            return false;
                        }
                        CHECK_SEMAPHORE_PTR(semaphore, name);
                        if (sem_post(semaphore) == 0) {
                            reset_locked();
                            return true;
                        } else {
                            throw LOCK_ERROR("Error while getting lock. [name=%s][error=%s]",
                                             this->name->c_str(), strerror(errno));
                        }
                    }

                    /*!
                     * Dispose this instance of the lock. Lock will be recreated if subsequently invoked.
                     *
                     * @return
                     */
                    bool dispose() {
                        CHECK_SEMAPHORE_PTR(semaphore, name);
                        if (sem_unlink(name->c_str()) != 0) {
                            LOG_ERROR("Error closing semaphore. [name=%s][error=%s]", name->c_str(), strerror(errno));
                            return false;
                        } else {
                            return true;
                        }
                    }
                };

                /*!
                 * Exception/return safe lock/unlock guard. Should never be instantiated as a pointer.
                 */
                class exclusive_lock_guard {
                private:
                    /// Lock handle backing this guard.
                    exclusive_lock *lock;
                public:
                    /*!
                     * Create a new guard instance. Will acquire the lock in wait mode if wait is true.
                     *
                     * @param lock - Lock instance handle.
                     * @param wait - Acquire wait lock.
                     * @return
                     */
                    exclusive_lock_guard(exclusive_lock *lock, bool wait = true) {
                        CHECK_NOT_NULL(lock);
                        this->lock = lock;
                        if (wait) {
                            this->lock->__wait_lock();
                        }
                    }

                    /*!
                     * Dispose the guard, will release the lock if acquired.
                     */
                    ~exclusive_lock_guard() {
                        if (NOT_NULL(lock)) {
                            if (lock->is_locked()) {
                                lock->__release_lock();
                            }
                        }
                    }

                    /*!
                     * Acquire this lock, will retry till timeout is exhausted.
                     *
                     * @param timeout - Acquire timeout.
                     * @return - Is acquired.
                     */
                    bool get_lock(uint64_t timeout = 0) {
                        if (timeout <= 0) {
                            return this->lock->__try_lock();
                        } else {
                            return this->lock->__try_lock(timeout);
                        }
                    }

                    /*!
                     * Release the lock, if acquired.
                     *
                     * @return
                     */
                    bool release() {
                        return this->lock->__release_lock();
                    }

                    /*!
                     * Get the name of the backing lock.
                     *
                     * @return - Lock name.
                     */
                    string get_lock_name() {
                        return *(lock->get_name());
                    }
                };
            }
        }
    }
}
#endif //WATERGATE_EXCLUSIVE_LOCK_H
