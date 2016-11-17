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

#ifndef WATERGATE_LOCK_TABLE_H
#define WATERGATE_LOCK_TABLE_H

#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <thread>
#include <mutex>

#include "common/includes/lock_record_def.h"
#include "common/includes/__app.h"
#include "common/includes/timer.h"
#include "common/includes/config.h"
#include "common/includes/log_utils.h"
#include "common/includes/lock_record_def.h"
#include "common/includes/exclusive_lock.h"
#include "common/includes/fmstream.h"
#include "common/includes/__bitset.h"
#include "resource_def.h"

#define CONST_LOCKT_ERROR_PREFIX "Lock Table Error : "
#define WG_LOCK_TABLE_LOCK_PREFIX "t_lock_"
#define WG_LOCK_TABLE_EXT ".lock"
#define WG_LOCK_TABLE_DIR "locks"

#define LOCK_TABLE_ERROR(fmt, ...) lock_table_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define LOCK_TABLE_ERROR_PTR(fmt, ...) new lock_table_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))

#define RELEASE_LOCK_RECORD(rec, priority) do {\
    LOG_DEBUG("[priority=%d] Resetting lock record...", priority); \
    rec->lock.locks[priority].state = __lock_state::None; \
    rec->lock.locks[priority].acquired_time = 0; \
    if (IS_BASE_PRIORITY(priority)) rec->lock.quota_used = 0; \
}while(0)

#define SHARED_LOCK_NAME(pid) common_utils::format("proc_%d", pid)


namespace com {
    namespace wookler {
        namespace watergate {
            namespace core {
                class lock_table_error : public base_error {
                public:
                    lock_table_error(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                                 CONST_LOCKT_ERROR_PREFIX,
                                                                                                 mesg) {
                    }
                };


                class lock_table {
                protected:
                    /// Table instance name. Should be unique for a machine.
                    string name;
                    exclusive_lock *lock = nullptr;
                    /// Memory-mapped file handle.
                    fmstream *stream = nullptr;
                    /// Bitset based index to manage free/used records.
                    com::wookler::reactfs::common::__bitset *bit_index = nullptr;
                    /// Base pointer for the mapped data.
                    void *base_ptr = nullptr;
                    /// Header pointer, pointing to the header in the mapped memory
                    __lock_table_header *header_ptr = nullptr;
                    /// Base lock record pointer.
                    __lock_record *record_ptr = nullptr;

                protected:
                    /// State of this instance of the lock table.
                    __state__ state;

                    /*!
                     * Create a new instance of the lock table. Can be created in server or client mode.
                     *
                     * @param name - Table name.
                     * @param resource - Resouce to be controlled via this lock instance.
                     * @param count - Number of concurrent lockers allowed.
                     * @param server - Is in server mode?
                     * @param overwrite - Overwrite and reset all existing data.
                     */
                    void create(string name, resource_def *resource, uint32_t count, bool server, bool overwrite);


                    __lock_record *get_record(uint32_t index) {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(index >= 0 && index < header_ptr->max_records);

                        return (record_ptr + index);
                    }

                    void remove_record(uint32_t index);

                    __lock_record *create_new_record(string app_name, string app_id, pid_t pid);


                    void reset_all_records() {
                        CHECK_STATE_AVAILABLE(state);
                        for (uint32_t ii = 0; ii < header_ptr->max_records; ii++) {
                            __lock_record *record = get_record(ii);
                            RESET_RECORD(record);
                        }
                    }

                    void reset() {
                        CHECK_STATE_AVAILABLE(state);
                        WAIT_LOCK_P(lock);
                        bit_index->clear();
                        reset_all_records();
                        header_ptr->used_record = 0;
                        RELEASE_LOCK_P(lock);
                    }

                    void __check_expired_locks(uint32_t index, uint64_t expiry_time, uint32_t *counts) {
                        __lock_record *record = get_record(index);
                        if (record->used) {
                            for (uint32_t jj = 0; jj < MAX_PRIORITY_ALLOWED; jj++) {
                                if (record->lock.locks[jj].state == __lock_state::Locked) {
                                    uint64_t at = record->lock.locks[jj].acquired_time + header_ptr->lock_lease_time;
                                    uint64_t now = time_utils::now();
                                    if ((at + expiry_time) <= now) {
                                        counts[jj]++;
                                        record->lock.locks[jj].state = __lock_state::ForceReleased;
                                        record->lock.locks[jj].acquired_time = 0;
                                    }
                                }
                            }
                        }
                    }

                public:
                    lock_table() {

                    }

                    virtual ~lock_table() {
                        state.set_state(Disposed);
                        CHECK_AND_FREE(lock);
                        if (NOT_NULL(stream)) {
                            if (stream->is_open()) {
                                stream->close();
                            }
                            delete (stream);
                            stream = nullptr;
                        }
                        CHECK_AND_FREE(bit_index);
                    }

                    const string get_name() const {
                        return this->name;
                    }

                    const __state__ get_state() const {
                        return state;
                    }


                    void remove_record(__lock_record *rec) {
                        PRECONDITION(NOT_NULL(rec));
                        remove_record(rec->index);
                    }

                    void set_quota(double quota) {
                        CHECK_STATE_AVAILABLE(state);
                        header_ptr->quota = quota;
                    }

                    double get_quota() {
                        CHECK_STATE_AVAILABLE(state);
                        return header_ptr->quota;
                    }

                    uint64_t get_lock_lease_time() {
                        CHECK_STATE_AVAILABLE(state);
                        return header_ptr->lock_lease_time;
                    }

                    void set_lock_lease_time(long lease_time) {
                        CHECK_STATE_AVAILABLE(state);
                        header_ptr->lock_lease_time = lease_time;
                    }

                    uint32_t get_table_size() {
                        CHECK_STATE_AVAILABLE(state);
                        return header_ptr->max_records;
                    }
                };

                class lock_table_manager : public lock_table {

                public:
                    ~lock_table_manager() {
                        if (NOT_NULL(lock)) {
                            lock->dispose();
                        }
                    }

                    void init(string name, resource_def *resource, uint32_t count, bool overwrite) {
                        create(name, resource, count, true, overwrite);

                        if (overwrite) {
                            lock->reset();
                            reset();

                            long lt = resource->get_lease_time();
                            if (lt > 0) {
                                header_ptr->lock_lease_time = lt;
                            } else {
                                header_ptr->lock_lease_time = DEFAULT_LEASE_TIME;
                            }

                            double quota = resource->get_resource_quota();
                            if (quota > 0.0) {
                                header_ptr->quota = quota;
                            } else {
                                header_ptr->quota = DEFAULT_QUOTA;
                            }
                        }
                    }

                    void check_expired_locks(uint64_t expiry_time, uint32_t *counts) {
                        CHECK_STATE_AVAILABLE(state);
                        CHECK_NOT_NULL(counts);

                        for (int ii = 0; ii < header_ptr->max_records; ii++) {
                            __check_expired_locks(ii, expiry_time, counts);
                        }
                    }

                    void reset_expired_records(uint64_t expiry_time) {
                        CHECK_STATE_AVAILABLE(state);

                        for (uint32_t ii = 0; ii < header_ptr->max_records; ii++) {
                            __lock_record *record = get_record(ii);
                            if (record->used) {
                                uint64_t ut = record->app.last_active_ts;
                                uint64_t now = time_utils::now();
                                if ((ut + expiry_time) <= now) {
                                    LOG_WARN(
                                            "Resetting expired application record. [app name=%s][pid=%d][name=%s][last active=%s]",
                                            record->app.app_name, record->app.proc_id, name.c_str(),
                                            time_utils::get_time_string(ut).c_str());
                                    remove_record(ii);
                                }
                            }
                        }
                    }

                };

                class lock_table_client : public lock_table {
                private:
                    __lock_record *lock_record;

                    bool is_lock_active(int priority) {
                        if (lock_record->lock.locks[priority].state == __lock_state::Locked) {
                            uint64_t now = time_utils::now();
                            if (now < (lock_record->lock.locks[priority].acquired_time + get_lock_lease_time())) {
                                return true;
                            }
                        }
                        return false;
                    }

                public:
                    ~lock_table_client() override {
                        if (state.is_disposed())
                            return;

                        state.set_state(Disposed);

                        if (NOT_NULL(lock_record)) {
                            pid_t pid = getpid();
                            if (lock_record->used && lock_record->app.proc_id == pid) {
                                LOG_WARN("Removing current lock record. [app name=%s][pid=%d][name=%s]",
                                         lock_record->app.app_id,
                                         lock_record->app.proc_id, name.c_str());
                                remove_record(lock_record->index);
                                lock_record = nullptr;
                            }
                        }
                    }

                    void init(const __app *app, string name, resource_def *resrouce, uint32_t count) {
                        if (NOT_NULL(lock_record)) {
                            pid_t pid = getpid();
                            if (lock_record->app.proc_id != pid) {
                                throw new LOCK_TABLE_ERROR("Invalid lock record. [current pid=%d][app pid=%d]", pid,
                                                           lock_record->app.proc_id);
                            }
                        }
                        create(name, resrouce, count, false, false);
                        pid_t pid = getpid();

                        lock_record = create_new_record(app->get_name(), app->get_id(), pid);
                        CHECK_NOT_NULL(lock_record);
                    }

                    __lock_record *new_record(string app_name, string app_id, pid_t pid) {
                        CHECK_STATE_AVAILABLE(state);

                        if (NOT_NULL(lock_record)) {
                            pid_t pid = getpid();
                            if (lock_record->app.proc_id != pid) {
                                throw LOCK_TABLE_ERROR("Invalid lock record. [current pid=%d][app pid=%d]", pid,
                                                       lock_record->app.proc_id);
                            }
                        } else {
                            throw LOCK_TABLE_ERROR("Invalid lock table client state. Process record handle is null.");
                        }

                        return lock_record;
                    }

                    __lock_record *new_record_test(string app_name, string app_id, pid_t pid) {
                        CHECK_STATE_AVAILABLE(state);
                        return create_new_record(app_name, app_id, pid);
                    }

                    __lock_state has_valid_lock(int priority, double quota) {
                        CHECK_STATE_AVAILABLE(state);


                        __lock_state r = None;
                        pid_t pid = getpid();
                        if (lock_record->app.proc_id != pid) {
                            lock_table_error te = LOCK_TABLE_ERROR(
                                    "Lock record has been reset. [current owner=%d][proc id=%d]",
                                    lock_record->app.proc_id,
                                    pid);
                        }
                        string thread_id = thread_utils::get_current_thread();

                        lock_record->app.last_active_ts = time_utils::now();
                        if (lock_record->lock.locks[priority].state == __lock_state::Locked) {
                            if (!is_lock_active(priority)) {
                                TRACE("[pid=%d][thread=%s][name=%s][priority=%d][acquired=%lu][timeout=%lu] Lock has expired.",
                                      pid,
                                      thread_id.c_str(),
                                      name.c_str(), priority, lock_record->lock.locks[priority].acquired_time,
                                      get_lock_lease_time());
                                r = Expired;
                            } else if (quota > 0) {
                                double q = get_quota();
                                if (q > 0) {
                                    double aq = q - lock_record->lock.quota_used;
                                    if (aq < quota) {
                                        TRACE("[pid=%d][thread=%s][name=%s][priority=%d]  Quota reached. Release lock.",
                                              pid,
                                              thread_id.c_str(),
                                              name.c_str(), priority);
                                        return ReleaseLock;
                                    }
                                }
                                r = Locked;
                            } else {
                                r = Locked;
                            }
                        } else if (lock_record->lock.locks[priority].state == __lock_state::ForceReleased) {
                            TRACE("[pid=%d][thread=%s][name=%s][priority=%d]  Lock force released by server.", pid,
                                  thread_id.c_str(),
                                  name.c_str(), priority);
                            lock_record->lock.locks[priority].state = __lock_state::None;
                            r = ForceReleased;
                        } else if (lock_record->lock.locks[priority].state == __lock_state::QuotaReached) {
                            uint64_t now = time_utils::now();
                            if (now > (lock_record->lock.locks[priority].acquired_time + get_lock_lease_time())) {
                                TRACE("[pid=%d][thread=%s][name=%s][priority=%d]  Quota timeout reached.", pid,
                                      thread_id.c_str(),
                                      name.c_str(), priority);
                                lock_record->lock.locks[priority].state = __lock_state::None;
                                lock_record->lock.quota_used = 0;
                                r = None;
                            } else {
                                TRACE("[pid=%d][thread=%s][name=%s][priority=%d]  Quota reached.", pid,
                                      thread_id.c_str(),
                                      name.c_str(), priority);
                                r = QuotaReached;
                            }
                        }
                        TRACE("[pid=%d][thread=%s][name=%s][priority=%d]  Return value = %s. [state=%s]", pid,
                              thread_id.c_str(),
                              name.c_str(), priority, record_utils::get_lock_acquire_enum_string(r).c_str(),
                              record_utils::get_lock_acquire_enum_string(
                                      lock_record->lock.locks[priority].state).c_str());
                        return r;
                    }


                    __lock_state check_and_lock(double quota) {
                        CHECK_STATE_AVAILABLE(state);

                        __lock_state ls = has_valid_lock(BASE_PRIORITY, quota);
                        if (ls == Expired) {
                            release_lock(ls, BASE_PRIORITY);
                        } else if (ls == Locked) {
                            lock_record->lock.quota_used += quota;
                            lock_record->lock.quota_total += quota;
                        }
                        return ls;
                    }

                    __lock_state quota_available(double quota) {
                        CHECK_STATE_AVAILABLE(state);

                        double q = get_quota();
                        if (q > 0) {
                            double aq = q - lock_record->lock.quota_used;
                            if (aq < quota) {
                                LOG_DEBUG("Quota Reached %f", lock_record->lock.quota_used);
                                return QuotaReached;
                            }
                        }

                        return QuotaAvailable;
                    }

                    void update_lock(int priority) {
                        CHECK_STATE_AVAILABLE(state);

                        lock_record->lock.locks[priority].state = __lock_state::Locked;
                        lock_record->lock.locks[priority].acquired_time = time_utils::now();
                    }

                    void update_quota(double quota, int priority) {
                        CHECK_STATE_AVAILABLE(state);
                        lock_record->lock.quota_used += quota;
                        lock_record->lock.quota_total += quota;
                        LOG_DEBUG("[priority=%d] Current quota used = %f", priority,
                                  lock_record->lock.quota_used);
                    }

                    void reset_quota() {
                        CHECK_STATE_AVAILABLE(state);
                        lock_record->lock.quota_used = 0;
                    }

                    void release_lock(__lock_state lock_state, int priority) {
                        CHECK_STATE_AVAILABLE(state);
                        if (lock_state == __lock_state::Expired || lock_state == __lock_state::Locked) {
                            if (lock_record->lock.locks[priority].state == __lock_state::Locked) {
                                RELEASE_LOCK_RECORD(lock_record, priority);
                            }
                        } else if (lock_state == ReleaseLock) {
                            if (lock_record->lock.locks[priority].state == __lock_state::Locked) {
                                lock_record->lock.locks[priority].state = QuotaReached;
                            }
                        }
                    }

                    void dump() {
                        CHECK_STATE_AVAILABLE(state);

                        LOG_DEBUG("**************[LOCK TABLE:%s:%d]**************", get_name().c_str(), getpid());
                        LOG_DEBUG("\tquota=%f", get_quota());
                        LOG_DEBUG("\tlease time=%lu", get_lock_lease_time());

                        if (NOT_NULL(lock_record)) {
                            LOG_DEBUG("\t\tin use=%s", (lock_record->used ? "used" : "unused"));
                            LOG_DEBUG("\t\tindex=%d", lock_record->index);
                            LOG_DEBUG("\t\tapp name=%s", lock_record->app.app_name);
                            LOG_DEBUG("\t\tapp id=%s", lock_record->app.app_id);
                            LOG_DEBUG("\t\tproc id=%d", lock_record->app.proc_id);
                            LOG_DEBUG("\t\tregister time=%s",
                                      time_utils::get_time_string(lock_record->app.register_time).c_str());
                            LOG_DEBUG("\t\tlast active=%s",
                                      time_utils::get_time_string(lock_record->app.last_active_ts).c_str());
                            for (int ii = 0; ii < MAX_PRIORITY_ALLOWED; ii++) {
                                LOG_DEBUG("\t\t[%d] has lock=%s", ii,
                                          ((lock_record->lock.locks[ii].state == Locked) ? "true" : "false"));
                                LOG_DEBUG("\t\t[%d] acquired time=%s", ii,
                                          (lock_record->lock.locks[ii].acquired_time > 0 ? time_utils::get_time_string(
                                                  lock_record->lock.locks[ii].acquired_time).c_str() : "N/A"));
                            }
                            LOG_DEBUG("\t\tused quota=%f", lock_record->lock.quota_used);
                            LOG_DEBUG("\t\ttotal quota=%f", lock_record->lock.quota_total);
                        }

                        LOG_DEBUG("**************[LOCK TABLE:%s:%d]**************", get_name().c_str(), getpid());
                    }
                };
            }
        }
    }
}
#endif //WATERGATE_LOCK_TABLE_H
