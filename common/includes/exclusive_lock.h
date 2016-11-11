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

#define DEFAULT_LOCK_MODE 0660

#define CONST_LOCK_ERROR_PREFIX "Lock Error : "

#define LOCK_ERROR(fmt, ...) lock_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define LOCK_ERROR_PTR(fmt, ...) new lock_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define EXCLUSIVE_LOCK_PREFIX "/EL::"

#define CHECK_SEMAPHORE_PTR(s, name) do { \
    if (IS_NULL(s) || s == SEM_FAILED) { \
        throw LOCK_ERROR("Semaphore is not valid. [name=%s]", name->c_str()); \
    } \
} while(0)

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                class lock_error : public base_error {
                public:
                    lock_error(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                           CONST_LOCK_ERROR_PREFIX,
                                                                                           mesg) {
                    }
                };

                class exclusive_lock {
                private:
                    string *name = nullptr;
                    mode_t mode = DEFAULT_LOCK_MODE;
                    sem_t *semaphore = nullptr;
                    bool locked = false;

                public:
                    exclusive_lock(const string *name) {
                        string ss = common_utils::get_normalized_name(*name);
                        assert(!IS_EMPTY(ss));
                        this->name = new string(EXCLUSIVE_LOCK_PREFIX);
                        this->name->append(ss);
                    }

                    exclusive_lock(const string *name, mode_t mode) {
                        string ss = common_utils::get_normalized_name(*name);
                        assert(!IS_EMPTY(ss));
                        this->name = new string(EXCLUSIVE_LOCK_PREFIX);
                        this->name->append(ss);
                        this->mode = mode;
                    }

                    ~exclusive_lock() {
                        if (NOT_NULL(semaphore) && semaphore != SEM_FAILED) {
                            if (sem_close(semaphore) != 0) {
                                LOG_ERROR("Error closing semaphore. [name=%s][errno=%s]", name, strerror(errno));
                            }
                            semaphore = nullptr;
                        }
                        CHECK_AND_FREE(name);
                    }

                    bool is_locked() {
                        return locked;
                    }

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
                        locked = false;
                    }

                    void create() {
                        semaphore = sem_open(name->c_str(), O_CREAT, mode, 1);
                        if (IS_NULL(semaphore) || semaphore == SEM_FAILED) {
                            lock_error e = LOCK_ERROR("Error creating lock. [name=%s][errno=%s]", name->c_str(),
                                                      strerror(errno));
                            LOG_ERROR(e.what());
                            throw e;
                        }
                        LOG_DEBUG("Created exclusive lock. [name=%s]", name->c_str());
                        locked = false;
                    }

                    const string *get_name() const {
                        return name;
                    }

                    bool try_lock() {
                        CHECK_SEMAPHORE_PTR(semaphore, name);
                        if (sem_trywait(semaphore) == 0) {
                            locked = true;
                            return true;
                        } else {
                            return false;
                        }
                    }

                    bool wait_lock() {
                        CHECK_SEMAPHORE_PTR(semaphore, name);
                        if (sem_wait(semaphore) == 0) {
                            locked = true;
                            return true;
                        } else {
                            return false;
                        }
                    }

                    bool release_lock() {
                        CHECK_SEMAPHORE_PTR(semaphore, name);
                        if (sem_post(semaphore) == 0) {
                            locked = false;
                            return true;
                        } else {
                            return false;
                        }
                    }

                    bool dispose() {
                        CHECK_SEMAPHORE_PTR(semaphore, name);
                        if (sem_unlink(name->c_str()) != 0) {
                            LOG_ERROR("Error closing semaphore. [name=%s]", name);
                            return false;
                        } else {
                            return true;
                        }
                    }
                };
            }
        }
    }
}
#endif //WATERGATE_EXCLUSIVE_LOCK_H
