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

#ifndef WATERGATE_CONTROL_MANAGER_H
#define WATERGATE_CONTROL_MANAGER_H

#include <thread>

#include "common/includes/__callback.h"
#include "common/includes/__threads.h"
#include "control_def.h"

#define DEFAULT_LOCK_RESET_TIME "60s"
#define DEFAULT_RECORD_RESET_TIME "10m"

#define CONTROL_MANAGER_TASK_NAME "control-manager-gc-task"
#define CONST_CM_CONFIG_LOCK_RESET_TIME "lock.reset.time"
#define CONST_CM_CONFIG_RECORD_RESET_TIME "record.reset.time"

#define DEFAULT_CONTROL_THREAD_SLEEP 5 * 1000 // 10 seconds

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace watergate {
            namespace core {
                class control_manager : public control_def {
                private:
                    uint64_t lock_timeout;
                    uint64_t record_timeout;
                    thread *control_thread;
                    bool reset_lock_table = false;
                    bool start_lock_thread = true;

                    void start() {
                        control_thread = new thread(control_manager::run, this);
                    }


                    void join() {
                        PRECONDITION(!state.is_available());

                        if (NOT_NULL(control_thread)) {
                            control_thread->join();
                        }
                    }

                    static void run(control_manager *owner);

                public:
                    control_manager(bool reset_lock_table = false, bool start_lock_thread = true) {
                        this->reset_lock_table = reset_lock_table;
                        this->start_lock_thread = start_lock_thread;
                    }

                    ~control_manager() override {
                        state.set_state(Disposed);
                        LOG_INFO("Disposing control manager. [state=%s]", state.get_state_string().c_str());
                        join();
                    }

                    void init(const __app *app, const ConfigValue *config) override;

                    void clear_locks() {
                        if (!IS_EMPTY(semaphores)) {
                            unordered_map<string, _semaphore *>::iterator iter;
                            for (iter = semaphores.begin(); iter != semaphores.end(); iter++) {
                                _semaphore *sem = iter->second;
                                if (NOT_NULL(sem)) {
                                    _semaphore_owner *c = static_cast<_semaphore_owner *>(sem);
                                    c->reset();
                                }
                            }
                        }
                    }

                    uint64_t get_record_timeout() {
                        return record_timeout;
                    }

                    uint64_t get_lock_timeout() {
                        return lock_timeout;
                    }

                    void run_checkup() {
                        if (state.is_available()) {
                            if (!IS_EMPTY(semaphores)) {
                                unordered_map<string, _semaphore *>::iterator iter;
                                for (iter = semaphores.begin(); iter != semaphores.end(); iter++) {
                                    _semaphore *sem = iter->second;
                                    if (NOT_NULL(sem)) {
                                        _semaphore_owner *c = static_cast<_semaphore_owner *>(sem);
                                        c->check_expired_locks(lock_timeout);
                                        c->check_expired_records(record_timeout);
                                    }
                                }
                            }
                        }
                    }
                };

                class control_manager_callback : public __runnable_callback {
                private:
                    uint64_t last_run_time;
                    control_manager *owner = nullptr;

                public:
                    control_manager_callback(control_manager *owner) : __runnable_callback(CONTROL_MANAGER_TASK_NAME) {
                        this->owner = owner;
                    }


                    void callback() override {
                        CHECK_NOT_NULL(owner);

                        try {
                            LOG_INFO("Running control manager clean-up task...");
                            if (NOT_NULL(owner)) {
                                owner->run_checkup();
                            }
                        } catch (const exception &e) {
                            control_error ce = CONTROL_ERROR(
                                    "[%d][name=control_manager] Control thread exited with error. [error=%s]",
                                    getpid(),
                                    e.what());
                            throw ce;
                        } catch (...) {
                            control_error ce = CONTROL_ERROR(
                                    "[%d][name=control_manager] Control thread exited with error. [error=UNKNOWN ERROR]",
                                    getpid());
                            throw ce;
                        }
                    }


                    void error() override {
                        LOG_ERROR("[name=%s] Unknown error occurred...");
                    }


                    void error(exception *err) override {
                        LOG_ERROR(err->what());
                    }

                    bool can_run() override {
                        uint64_t now = time_utils::now();
                        return ((now - last_run_time) >= DEFAULT_CONTROL_THREAD_SLEEP);
                    }
                };
            }
        }
    }
}
#endif //WATERGATE_CONTROL_MANAGER_H
