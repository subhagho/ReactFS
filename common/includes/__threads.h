/*
 * Copyright [yyyy] [name of copyright owner]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//
// Created by Subhabrata Ghosh on 23/11/16.
//

#ifndef REACTFS_THREADS_H
#define REACTFS_THREADS_H

#include <mutex>
#include <vector>
#include <queue>

#include "common.h"
#include "common_utils.h"
#include "base_error.h"
#include "__callback.h"
#include "log_utils.h"
#include "__alarm.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                typedef enum __thread_state_enum__ {
                    TS_AVAILABLE = 0, TS_RUNNING, TS_STOPPED, TS_EXCEPTION, TS_UNKNOWN
                } __thread_state_enum;

                class __thread_state {
                private:
                    __thread_state state = __thread_state_enum::TS_UNKNOWN;
                    const exception *error = nullptr;

                public:
                    const __thread_state_enum get_state() const {
                        return state;
                    }

                    string get_state_string() const {
                        switch (state) {
                            case TS_UNKNOWN:
                                return "Unknown";
                            case TS_RUNNING:
                                return "Running";
                            case TS_AVAILABLE:
                                return "Available";
                            case TS_STOPPED:
                                return "Stopped";
                            case TS_EXCEPTION:
                                return "Exception";
                            default:
                                return EMPTY_STRING;
                        }
                    }

                    void set_state(__state_enum s) {
                        state = s;
                    }

                    void set_error(const exception *e) {
                        error = e;
                        state = Exception;
                    }

                    const exception *get_error() const {
                        if (state == __thread_state_enum::TS_EXCEPTION) {
                            return error;
                        }
                        return NULL;
                    }

                    bool has_error() const {
                        return (state == __thread_state_enum::TS_EXCEPTION);
                    }

                    bool is_available() const {
                        return (state == __thread_state_enum::TS_AVAILABLE);
                    }

                    bool is_stopped() const {
                        return (state == __thread_state_enum::TS_STOPPED);
                    }

                    bool is_running() const {
                        return (state == __thread_state_enum::TS_RUNNING);
                    }
                };


                class __thread_stack {
                private:
                    mutex __stack_lock;
                    queue<__callback *> __stack;

                public:
                    ~__thread_stack() {

                    }

                    __callback *next_job() {
                        unique_lock<std::mutex> lock(__stack_lock);
                        if (IS_EMPTY(__stack)) {
                            return __stack.pop();
                        }
                        return nullptr;
                    }
                };

                class __m_thread {
                private:
                    uint64_t __id;
                    thread __runner;
                    __thread_state __state;
                    __thread_stack *queue = nullptr;
                    uint64_t __sleep_time = 0;

                    void run() {
                        try {
                            __alarm alarm(__sleep_time);
                            __state.set_state(__thread_state_enum::TS_AVAILABLE);
                            while (__state != __thread_state_enum::TS_STOPPED) {
                                __callback *task = queue->next_job();
                                if (IS_NULL(task)) {
                                    WAIT_ALARM(alarm);
                                    continue;
                                }
                                try {
                                    task->callback();
                                } catch (const exception &err) {
                                    base_error be = BASE_ERROR("[task=%s] Thread terminated with exception. [error=%s]",
                                                               task->get_uuid(),
                                                               e.what());
                                    LOG_WARN(be.what());
                                    task->error(&be);
                                } catch (...) {
                                    base_error be = BASE_ERROR(
                                            "[task=%s] Thread terminated with exception. [error=UNKNOWN]",
                                            task->get_uuid());
                                    LOG_WARN(be.what());
                                    task->error(&be);
                                }
                            }
                        } catch (const exception &e) {
                            base_error be = BASE_ERROR("Thread terminated with exception. [error=%s]", e.what());
                            LOG_CRITICAL(be.what());
                            __state.set_error(&e);
                            throw e;
                        } catch (...) {
                            base_error be = BASE_ERROR("Thread terminated with exception. [error=UNKNOWN]");
                            LOG_CRITICAL(be.what());
                            __state.set_error(&e);
                            throw e;
                        }
                    }

                public:
                    __m_thread(__thread_stack *stack) {
                        CHECK_NOT_NULL(stack);
                        this->queue = stack;
                    }


                };

            }
        }
    }
}
#endif //REACTFS_THREADS_H
