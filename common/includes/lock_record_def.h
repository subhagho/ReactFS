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
// Created by Subhabrata Ghosh on 22/09/16.
//

#ifndef WATERGATE_LOCK_RECORD_DEF_H
#define WATERGATE_LOCK_RECORD_DEF_H

#include <unistd.h>
#include <thread>
#include <sstream>
#include <unordered_map>
#include <semaphore.h>

#include "common/includes/log_utils.h"
#include "common/includes/common.h"
#include "common/includes/base_error.h"
#include "common/includes/process_utils.h"

#define MAX_PRIORITY_ALLOWED 8
#define DEFAULT_MAX_RECORDS 1024
#define MAX_STRING_SIZE 64
#define FREE_INDEX_USED -999

#ifndef PSEMNAMLEN // For cases where the POSIX sem header is not visible.
#define PSEMNAMLEN 31
#endif //PSEMNAMLEN

#define BASE_PRIORITY 0

#define IS_BASE_PRIORITY(p) (p == BASE_PRIORITY)

typedef enum {
    Locked = 0,
    QuotaReached = 1,
    ForceReleased = 2,
    Expired = 3,
    None = 4,
    Error = 5,
    Timeout = 6,
    QuotaAvailable = 7,
    ReleaseLock = 8,
    Retry = 9
} __lock_state;

typedef struct __app_handle__ {
    char app_name[MAX_STRING_SIZE];
    char app_id[MAX_STRING_SIZE];
    pid_t proc_id;
    uint64_t register_time = 0;
    uint64_t last_active_ts = 0;
} __app_handle;

typedef struct __priority_lock__ {
    __lock_state state = __lock_state::None;
    uint64_t acquired_time = 0;
} __priority_lock;

typedef struct __lock_handle__ {
    double quota_used;
    double quota_total;
    __priority_lock locks[MAX_PRIORITY_ALLOWED];
} __lock_handle;

typedef struct __lock_record__ {
    bool used = false;
    uint32_t index = 0;
    __app_handle app;
    __lock_handle lock;
} __lock_record;

typedef struct __lock_table_header__ {
    uint64_t lock_lease_time;
    double quota = 0;
    uint16_t used_record = 0;
    uint16_t max_records = 0;
} __lock_table_header;

typedef struct __lock_metrics__ {
    int base_priority = -1;
    uint64_t total_wait_time = 0;
    uint64_t max_wait_time = 0;
    uint64_t tries = 0;
} __lock_metrics;

typedef struct __lock_id__ {
    uint64_t id = -1;
    uint64_t acquired_time = 0;
} __lock_id;

using namespace std;


namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {

                struct thread_lock_ptr {
                    string thread_id;
                    __lock_id **priority_lock_index = nullptr;
                    int lock_priority = -1;
                    __lock_metrics metrics;
                };

                class thread_lock_record {
                private:
                    std::string thread_id;
                    int *p_counts = nullptr;
                    int priorities = 0;
                    thread_lock_ptr *thread_ptr;

                public:
                    thread_lock_record(thread_lock_ptr *thread_ptr, int priorities) {
                        this->thread_id = thread_ptr->thread_id;
                        this->p_counts = (int *) malloc(priorities * sizeof(int));
                        this->priorities = priorities;
                        for (int ii = 0; ii < this->priorities; ii++) {
                            p_counts[ii] = 0;
                        }
                        this->thread_ptr = thread_ptr;
                    }

                    ~thread_lock_record() {
                        if (NOT_NULL(p_counts)) {
                            free(p_counts);
                            p_counts = nullptr;
                        }
                        if (NOT_NULL(thread_ptr)) {
                            if (NOT_NULL(thread_ptr->priority_lock_index)) {
                                for (int ii = 0; ii < priorities; ii++) {
                                    if (NOT_NULL(thread_ptr->priority_lock_index[ii])) {
                                        free(thread_ptr->priority_lock_index[ii]);
                                    }
                                }
                                free(thread_ptr->priority_lock_index);
                            }
                            delete (thread_ptr);
                        }
                    }

                    thread_lock_ptr *get_thread_ptr() {
                        return thread_ptr;
                    }

                    int increment(int priority) {
                        _assert(priority >= 0 && priority < priorities);

                        p_counts[priority]++;

                        return p_counts[priority];
                    }

                    int decremet(int priority) {
                        _assert(priority >= 0 && priority < priorities);

                        p_counts[priority]--;

                        return p_counts[priority];
                    }

                    void reset(int priority) {
                        p_counts[priority] = 0;
                        thread_ptr->priority_lock_index[priority]->acquired_time = 0;
                        thread_ptr->priority_lock_index[priority]->id = -1;
                    }

                    void update_metrics(int priority, uint64_t lock_time) {
                        _assert(NOT_NULL(thread_ptr));

                        if (thread_ptr->metrics.base_priority < 0) {
                            thread_ptr->metrics.base_priority = priority;
                        }
                        thread_ptr->metrics.total_wait_time += lock_time;
                        thread_ptr->metrics.tries++;
                        if (thread_ptr->metrics.max_wait_time < lock_time) {
                            thread_ptr->metrics.max_wait_time = lock_time;
                        }
                    }

                    void dump() {
                        _assert(NOT_NULL(thread_ptr));
                        if (NOT_NULL(p_counts)) {
                            LOG_DEBUG("**************[THREAD:%s:%d]**************", thread_id.c_str(), getpid());
                            for (int ii = 0; ii < this->priorities; ii++) {
                                LOG_DEBUG("[priority=%d] count=%d", ii, p_counts[ii]);
                                double avg = ((double) thread_ptr->metrics.total_wait_time) / thread_ptr->metrics.tries;
                                LOG_DEBUG(
                                        "METRICS : [base priority=%d][average wait time=%f][max wait time=%lu][tries=%lu]",
                                        thread_ptr->metrics.base_priority, avg, thread_ptr->metrics.max_wait_time,
                                        thread_ptr->metrics.tries);
                            }
                            LOG_DEBUG("**************[THREAD:%s:%d]**************", thread_id.c_str(), getpid());
                        }
                    }

                    void test_assert() {
                        _assert(NOT_NULL(thread_ptr));
                        if (NOT_NULL(p_counts)) {
                            for (int ii = 0; ii < this->priorities; ii++) {
                                _assert(p_counts[ii] == 0);
                            }
                        }
                    }

                    static thread_lock_ptr *create_new_ptr(int max_priority) {
                        thread_lock_ptr *ptr = new thread_lock_ptr();
                        ptr->thread_id = thread_utils::get_current_thread();
                        ptr->priority_lock_index = (__lock_id **) malloc(max_priority * sizeof(__lock_id *));
                        for (int ii = 0; ii < max_priority; ii++) {
                            ptr->priority_lock_index[ii] = (__lock_id *) malloc(sizeof(__lock_id));
                            ptr->priority_lock_index[ii]->acquired_time = 0;
                            ptr->priority_lock_index[ii]->id = -1;
                        }
                        return ptr;
                    }
                };

                class record_utils {
                public:
                    static string get_lock_acquire_enum_string(__lock_state value) {
                        switch (value) {
                            case __lock_state::Locked:
                                return string("LOCKED");
                            case __lock_state::None:
                                return string("NONE");
                            case __lock_state::Error:
                                return string("ERROR");
                            case __lock_state::Expired:
                                return string("EXPIRED");
                            case __lock_state::QuotaReached:
                                return string("QUOTA REACHED");
                            case __lock_state::Timeout:
                                return string("TIMEOUT");
                            case __lock_state::ForceReleased:
                                return string("FORCE RELEASED");
                            case __lock_state::QuotaAvailable:
                                return string("QUOTA AVAILABLE");
                            case __lock_state::ReleaseLock:
                                return string("RELEASE LOCK");
                            case __lock_state::Retry:
                                return string("RETRY");
                            default:
                                return string("UNKNOWN");
                        }
                    }

                    static int get_lock_acquire_enum_int(__lock_state value) {
                        switch (value) {
                            case __lock_state::Locked:
                                return 0;
                            case __lock_state::None:
                                return 4;
                            case __lock_state::Error:
                                return 5;
                            case __lock_state::Expired:
                                return 3;
                            case __lock_state::QuotaReached:
                                return 1;
                            case __lock_state::QuotaAvailable:
                                return 7;
                            case __lock_state::Timeout:
                                return 6;
                            case __lock_state::ForceReleased:
                                return 2;
                            case __lock_state::ReleaseLock:
                                return 8;
                            case __lock_state::Retry:
                                return 9;
                            default:
                                return 4;
                        }
                    }

                    static void reset_record(__lock_record *record) {
                        PRECONDITION(NOT_NULL(record));

                        record->used = false;

                        // Reset App handle
                        record->app.proc_id = 0;
                        memset(record->app.app_id, 0, MAX_STRING_SIZE);
                        memset(record->app.app_name, 0, MAX_STRING_SIZE);
                        record->app.last_active_ts = 0;
                        record->app.register_time = 0;

                        // Reset Lock handles.
                        record->lock.quota_total = 0;
                        record->lock.quota_used = 0;

                        for (int ii = 0; ii < MAX_PRIORITY_ALLOWED; ii++) {
                            record->lock.locks[ii].acquired_time = 0;
                            record->lock.locks[ii].state = __lock_state::None;
                        }
                    }
                };
            }
        }
    }
}

#define RESET_RECORD(rec) com::wookler::reactfs::common::record_utils::reset_record(rec)

#endif //WATERGATE_LOCK_RECORD_DEF_H
