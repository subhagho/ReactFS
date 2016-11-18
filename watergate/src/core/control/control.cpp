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
// Created by Subhabrata Ghosh on 16/09/16.
//

#include "common/includes/lock_record_def.h"
#include "watergate/includes/control.h"
#include "common/includes/__alarm.h"
#include "watergate/includes/resource_factory.h"


void com::wookler::watergate::core::_semaphore::create(const __app *app, const ConfigValue *config, bool server) {
    CHECK_NOT_NULL(app);
    CHECK_NOT_NULL(config);

    const BasicConfigValue *cn = Config::get_value(CONST_SEM_CONFIG_PARAM_RESOURCE_NAME, config);
    if (IS_NULL(cn)) {
        throw ERROR_MISSING_CONFIG(CONST_SEM_CONFIG_PARAM_RESOURCE_NAME);
    }
    const string r_name = cn->get_value();
    if (IS_EMPTY(r_name)) {
        throw CONFIG_ERROR("NULL/Empty configuration value for node. [node=%s]", CONST_SEM_CONFIG_PARAM_RESOURCE_NAME);
    }

    this->resource = resource_factory::get_resource(r_name);

    this->name = new string(*this->resource->get_resource_name());

    this->is_server = server;

    const BasicConfigValue *v_prior = Config::get_value(CONST_SEM_CONFIG_NODE_PRIORITIES, config);
    if (IS_NULL(v_prior)) {
        throw CONFIG_ERROR("Required configuration node not found. [node=%s]", CONST_SEM_CONFIG_NODE_PRIORITIES);
    }
    this->priorities = (uint8_t) v_prior->get_short_value(this->priorities);
    if (this->priorities > MAX_PRIORITY_ALLOWED) {
        throw CONFIG_ERROR("Invalid configuration value. [%s=%d][MAX=%d]", CONST_SEM_CONFIG_NODE_PRIORITIES,
                           this->priorities, MAX_PRIORITY_ALLOWED);
    }
    this->max_concurrent = (uint16_t) this->resource->get_control_size();
    if (this->max_concurrent > SEM_VALUE_MAX) {
        throw CONFIG_ERROR("Invalid configuration value. [Max Concurrency=%d][MAX=%d]",
                           this->max_concurrent, SEM_VALUE_MAX);
    }
    const BasicConfigValue *v_mode = Config::get_value(CONST_SEM_CONFIG_NODE_MODE, config);
    if (NOT_NULL(v_mode)) {
        this->mode = v_mode->get_short_value(DEFAULT_SEM_MODE);
    }

    const BasicConfigValue *v_c_size = Config::get_value(CONST_SEM_CONFIG_NODE_CLIENTS, config);
    if (NOT_NULL(v_c_size)) {
        this->max_lock_clients = (uint16_t) v_c_size->get_int_value(DEFAULT_MAX_CONTROL_CLIENTS);
    } else {
        this->max_lock_clients = (uint16_t) DEFAULT_MAX_CONTROL_CLIENTS;
    }


    semaphores = (sem_t **) malloc(this->priorities * sizeof(sem_t *));
    memset(semaphores, 0, this->priorities * sizeof(sem_t *));

    for (int ii = 0; ii < this->priorities; ii++) {
        create_sem(ii);
    }

    for (int ii = 0; ii < this->priorities; ii++) {
        sem_t *ptr = semaphores[ii];
        if (IS_VALID_SEM_PTR(ptr))
            LOG_INFO("Created semaphore handle [name=%s][index=%d]", name->c_str(), ii);
        else
            throw CONTROL_ERROR("Invalid semaphore handle. [name=%s][index=%d]", name->c_str(), ii);
    }
}

void com::wookler::watergate::core::_semaphore::create_sem(int index) {
    PRECONDITION(index >= 0 && index < priorities);

    string sem_name = common_utils::format("%s::%s::%d", CONTROL_LOCK_PREFIX, name->c_str(), index);

    sem_t *ptr = sem_open(sem_name.c_str(), O_CREAT, mode, max_concurrent);
    if (!IS_VALID_SEM_PTR(ptr)) {
        throw CONTROL_ERROR("Error creating semaphore. [name=%s][errno=%s]", sem_name.c_str(), strerror(errno));
    }

    semaphores[index] = ptr;
}

void com::wookler::watergate::core::_semaphore::delete_sem(int index) {
    PRECONDITION(index >= 0 && index < priorities);

    if (IS_VALID_SEM_PTR(semaphores[index])) {
        if (!owner) {
            if (sem_close(semaphores[index]) != 0) {
                LOG_ERROR("Error disposing semaphore. [index=%s][errno=%s]", index, strerror(errno));
            }
        } else {
            string sem_name = common_utils::format("%s::%s::%d", CONTROL_LOCK_PREFIX, name->c_str(), index);
            if (sem_unlink(sem_name.c_str()) != 0) {
                LOG_ERROR("Error disposing semaphore. [index=%s][errno=%s]", index, strerror(errno));
            }
        }
        semaphores[index] = nullptr;
    }
}

com::wookler::watergate::core::_semaphore::~_semaphore() {
    CHECK_AND_FREE(this->name);
    CHECK_AND_FREE(resource);
    CHECK_AND_FREE(table);
}

__lock_state
com::wookler::watergate::core::_semaphore_client::try_lock(int priority, double quota, int base_priority, bool wait) {
    PRECONDITION(priority >= 0 && priority < priorities);
    PRECONDITION(base_priority >= 0 && base_priority < priorities);

    ASSERT(NOT_NULL(semaphores));

    std::lock_guard<std::mutex> guard(counts[priority]->priority_lock);

    pid_t pid = getpid();
    thread_lock_ptr *t_ptr = nullptr;
    thread_lock_record *t_rec = get_thread_lock();
    if (NOT_NULL(t_rec)) {
        t_ptr = t_rec->get_thread_ptr();
        if (t_ptr->priority_lock_index[priority]->id != counts[priority]->index) {
            t_ptr->priority_lock_index[priority]->id = -1;
        }
    } else {
        string tid = thread_utils::get_current_thread();
        if (IS_EMPTY(tid)) {
            tid = "UNKNOWN-THREAD";
        }
        throw CONTROL_ERROR("No lock record found for thread.[thread=%s]", tid.c_str());
    }

    __lock_state ls = client->has_valid_lock(priority, quota);
    if (ls == Locked) {
        counts[priority]->count++;
        t_rec->increment(priority);
        t_ptr->priority_lock_index[priority]->id = counts[priority]->index;
        t_ptr->priority_lock_index[priority]->acquired_time = time_utils::now();
        return ls;
    } else if (ls == Expired) {
        LOG_DEBUG(
                "[pid=%d][thread=%s][try_lock] Lock expired. [resetting all semaphores][priority=%d][base priority=%d]",
                pid, t_ptr->thread_id.c_str(), priority, base_priority);
        reset_locks(priority, ls);
    } else if (ls == ReleaseLock) {
        LOG_DEBUG(
                "[pid=%d][thread=%s][try_lock] Quota exhausted. [resetting all semaphores][priority=%d][base priority=%d]",
                pid, t_ptr->thread_id.c_str(), BASE_PRIORITY,
                base_priority);
        reset_locks(priority, ls);
        return QuotaReached;
    } else if (ls == QuotaReached) {
        return ls;
    } else if (ls == ForceReleased) {
        counts[priority]->count = 0;
        reset_thread_locks(priority);
        LOG_DEBUG("[pid=%d][thread=%s][try_lock] Lock released by server.[priority=%d][base priority=%d]", pid,
                  t_ptr->thread_id.c_str(),
                  priority,
                  base_priority);
    }

    sem_t *lock = get(priority);
    if (IS_VALID_SEM_PTR(lock)) {
        LOG_DEBUG("[pid=%d][thread=%s] Waiting for semaphore. [name=%s][priority=%d][base priority=%d]",
                  pid, t_ptr->thread_id.c_str(),
                  this->name->c_str(), priority, base_priority);
        int r = 0;
        if (wait) {
            r = sem_wait(lock);
        } else {
            r = sem_trywait(lock);
        }
        if (r == 0) {

            counts[priority]->count++;
            t_rec->increment(priority);
            t_ptr->priority_lock_index[priority]->id = counts[priority]->index;
            t_ptr->priority_lock_index[priority]->acquired_time = time_utils::now();
            client->update_lock(priority);

            LOG_DEBUG("[pid=%d][thread=%s] Acquired semaphore. [name=%s][priority=%d][base priority=%d][lock count=%d]",
                      pid, t_ptr->thread_id.c_str(), this->name->c_str(), priority,
                      base_priority, counts[priority]->count);

            return Locked;
        } else {
            if (wait) {
                LOG_DEBUG("Failed to acquire semaphore. [name=%s][priority=%d][base priority=%d][error=%s]",
                          this->name->c_str(), priority, base_priority,
                          strerror(errno));
                return Error;
            } else {
                return Retry;
            }
        }
    }
    throw CONTROL_ERROR("No lock found for the specified priority. [lock=%s][priority=%d]", this->name->c_str(),
                        priority);
}

__lock_state
com::wookler::watergate::core::_semaphore_client::try_lock_base(double quota, int base_priority, bool wait) {
    PRECONDITION(base_priority >= 0 && base_priority < priorities);

    ASSERT(NOT_NULL(semaphores));

    std::lock_guard<std::mutex> guard(counts[BASE_PRIORITY]->priority_lock);

    pid_t pid = getpid();
    thread_lock_ptr *t_ptr = nullptr;
    thread_lock_record *t_rec = get_thread_lock();
    if (NOT_NULL(t_rec)) {
        t_ptr = t_rec->get_thread_ptr();
        if (t_ptr->priority_lock_index[BASE_PRIORITY]->id != counts[BASE_PRIORITY]->index) {
            t_ptr->priority_lock_index[BASE_PRIORITY]->id = -1;
        }
    } else {
        string tid = thread_utils::get_current_thread();
        if (IS_EMPTY(tid)) {
            tid = "UNKNOWN-THREAD";
        }
        throw CONTROL_ERROR("No lock record found for thread.[thread=%s]", tid.c_str());
    }

    __lock_state ls = client->check_and_lock(quota);
    if (ls == Locked) {
        counts[BASE_PRIORITY]->count++;
        t_rec->increment(BASE_PRIORITY);
        t_ptr->priority_lock_index[BASE_PRIORITY]->id = counts[BASE_PRIORITY]->index;
        t_ptr->priority_lock_index[BASE_PRIORITY]->acquired_time = time_utils::now();
        return ls;
    } else if (ls == QuotaReached) {
        return ls;
    } else if (ls == Expired) {
        LOG_DEBUG(
                "[pid=%d][thread=%s][try_lock_base] Lock expired. [resetting all semaphores][priority=%d][base priority=%d]",
                pid, t_ptr->thread_id.c_str(), BASE_PRIORITY,
                base_priority);
        reset_locks(BASE_PRIORITY, ls);
    } else if (ls == ReleaseLock) {
        LOG_DEBUG(
                "[pid=%d][thread=%s][try_lock_base] Quota exhausted. [resetting all semaphores][priority=%d][base priority=%d]",
                pid, t_ptr->thread_id.c_str(), BASE_PRIORITY,
                base_priority);
        reset_locks(BASE_PRIORITY, ls);
        return QuotaReached;
    } else if (ls == ForceReleased) {
        counts[BASE_PRIORITY]->count = 0;
        reset_thread_locks(BASE_PRIORITY);
        client->reset_quota();

        LOG_DEBUG("[pid=%d][thread=%s][try_lock_base] Lock released by server.[priority=%d][base priority=%d]", pid,
                  t_ptr->thread_id.c_str(),
                  BASE_PRIORITY,
                  base_priority);
    }

    sem_t *lock = get(BASE_PRIORITY);
    if (IS_VALID_SEM_PTR(lock)) {
        int r = 0;
        if (wait) {
            r = sem_wait(lock);
        } else {
            r = sem_trywait(lock);
        }
        if (r == 0) {

            counts[BASE_PRIORITY]->count++;
            t_rec->increment(BASE_PRIORITY);
            t_ptr->priority_lock_index[BASE_PRIORITY]->id = counts[BASE_PRIORITY]->index;
            t_ptr->priority_lock_index[BASE_PRIORITY]->acquired_time = time_utils::now();
            client->update_lock(BASE_PRIORITY);
            client->reset_quota();
            client->update_quota(quota, base_priority);

            LOG_DEBUG("[pid=%d][thread=%s] Acquired semaphore. [name=%s][priority=%d][base priority=%d][lock count=%d]",
                      pid, t_ptr->thread_id.c_str(), this->name->c_str(),
                      BASE_PRIORITY, base_priority, counts[BASE_PRIORITY]->count);

            return Locked;
        } else if (errno == EAGAIN) {
            return Timeout;
        } else {
            if (wait) {
                LOG_DEBUG("Failed to acquire semaphore. [name=%s][priority=%d][base_priority=%d][error=%s]",
                          this->name->c_str(),
                          BASE_PRIORITY, base_priority,
                          strerror(errno));
                return Error;
            } else {
                return Retry;
            }
        }
    }
    throw CONTROL_ERROR("No lock found for the specified priority. [lock=%s][priority=%d]", this->name->c_str(),
                        BASE_PRIORITY);
}

bool com::wookler::watergate::core::_semaphore_client::release_lock_base(int base_priority) {
    PRECONDITION(base_priority >= 0 && base_priority < priorities);

    ASSERT(NOT_NULL(semaphores));

    std::lock_guard<std::mutex> guard(counts[BASE_PRIORITY]->priority_lock);

    pid_t pid = getpid();

    thread_lock_ptr *t_ptr = nullptr;
    thread_lock_record *t_rec = get_thread_lock();
    if (NOT_NULL(t_rec)) {
        t_ptr = t_rec->get_thread_ptr();
        if (t_ptr->priority_lock_index[BASE_PRIORITY]->id != counts[BASE_PRIORITY]->index) {
            t_ptr->priority_lock_index[BASE_PRIORITY]->id = -1;
            LOG_WARN(
                    "Lock index out-of-sync. [pid=%d][thread=%s][priority=%d][base priority=%d][current index=%d][new index=%d]",
                    pid, t_ptr->thread_id.c_str(), BASE_PRIORITY, base_priority,
                    t_ptr->priority_lock_index[BASE_PRIORITY]->id,
                    counts[BASE_PRIORITY]->index);
            return false;
        }
    } else {
        string tid = thread_utils::get_current_thread();
        if (IS_EMPTY(tid)) {
            tid = "UNKNOWN-THREAD";
        }
        throw CONTROL_ERROR("No lock record found for thread.[thread=%s]", tid.c_str());
    }

    __lock_state ls = client->has_valid_lock(BASE_PRIORITY, 0);
    if (ls == Locked) {
        counts[BASE_PRIORITY]->count--;
        t_rec->decremet(BASE_PRIORITY);

        if (counts[BASE_PRIORITY]->count <= 0) {
            t_ptr->priority_lock_index[BASE_PRIORITY]->id = -1;
            t_ptr->priority_lock_index[BASE_PRIORITY]->acquired_time = 0;

            sem_t *lock = get(BASE_PRIORITY);
            if (IS_VALID_SEM_PTR(lock)) {
                if (sem_post(lock) != 0) {
                    throw CONTROL_ERROR(
                            "Semaphores in invalid state. [name=%s][priority=%d][base priority=%d][error=%s]",
                            this->name->c_str(),
                            BASE_PRIORITY, base_priority, strerror(errno));
                }
                LOG_DEBUG("[pid=%d][thread=%s] Released semaphore [name=%s][priority=%d][base priority=%d]",
                          pid, t_ptr->thread_id.c_str(), this->name->c_str(),
                          BASE_PRIORITY, base_priority);


                client->release_lock(ls, BASE_PRIORITY);
                return true;
            } else {
                throw CONTROL_ERROR("No semaphore found for the specified priority. [lock=%s][priority=%d]",
                                    this->name->c_str(),
                                    BASE_PRIORITY);
            }
        } else {
            LOG_DEBUG("[pid=%d][thread=%s] Lock count pending. [priority=%d][base priority=%d][count=%d]", pid,
                      t_ptr->thread_id.c_str(),
                      BASE_PRIORITY, base_priority,
                      counts[BASE_PRIORITY]->count);
            return false;
        }
    } else if (ls == Expired) {
        LOG_DEBUG(
                "[pid=%d][thread=%s][release_lock_base] Lock expired. [Lock should be retried][priority=%d][base priority=%d]",
                pid, t_ptr->thread_id.c_str(), BASE_PRIORITY,
                base_priority);
        reset_locks(BASE_PRIORITY, ls);
        return true;
    } else if (ls == ForceReleased) {
        counts[BASE_PRIORITY]->count = 0;
        reset_thread_locks(BASE_PRIORITY);
        client->reset_quota();

        LOG_DEBUG("[pid=%d][thread=%s][release_lock_base] Lock released by server.[priority=%d][base priority=%d]", pid,
                  t_ptr->thread_id.c_str(),
                  BASE_PRIORITY,
                  base_priority);

        return false;
    }
    LOG_DEBUG("[pid=%d][thread=%s] Invalid lock state. [state=%s][priority=%d][base priority=%d]", pid,
              t_ptr->thread_id.c_str(),
              record_utils::get_lock_acquire_enum_string(ls).c_str(), BASE_PRIORITY, base_priority);
    return false;
}

bool com::wookler::watergate::core::_semaphore_client::release_lock(int priority, int base_priority) {
    PRECONDITION(priority >= 0 && priority < priorities);
    PRECONDITION(base_priority >= 0 && base_priority < priorities);

    ASSERT(NOT_NULL(semaphores));

    std::lock_guard<std::mutex> guard(counts[priority]->priority_lock);
    pid_t pid = getpid();

    thread_lock_ptr *t_ptr = nullptr;
    thread_lock_record *t_rec = get_thread_lock();
    if (NOT_NULL(t_rec)) {
        t_ptr = t_rec->get_thread_ptr();
        if (t_ptr->priority_lock_index[priority]->id != counts[priority]->index) {
            t_ptr->priority_lock_index[priority]->id = -1;
            LOG_WARN(
                    "Lock index out-of-sync. [pid=%d][thread=%s][priority=%d][base priority=%d][current index=%d][new index=%d]",
                    pid, t_ptr->thread_id.c_str(), priority, base_priority, t_ptr->priority_lock_index[priority]->id,
                    counts[priority]->index);
            return false;
        }
    } else {
        string tid = thread_utils::get_current_thread();
        if (IS_EMPTY(tid)) {
            tid = "UNKNOWN-THREAD";
        }
        throw CONTROL_ERROR("No lock record found for thread.[thread=%s]", tid.c_str());
    }

    __lock_state ls = client->has_valid_lock(priority, 0);
    if (ls == Locked) {
        counts[priority]->count--;
        t_rec->decremet(priority);

        if (counts[priority]->count <= 0) {
            t_ptr->priority_lock_index[priority]->id = -1;
            t_ptr->priority_lock_index[priority]->acquired_time = 0;

            sem_t *lock = get(priority);
            if (IS_VALID_SEM_PTR(lock)) {
                if (sem_post(lock) != 0) {
                    throw CONTROL_ERROR(
                            "Semaphores in invalid state. [name=%s][priority=%d][base priority=%d][error=%s]",
                            this->name->c_str(),
                            priority, base_priority, strerror(errno));
                }
                LOG_DEBUG("[pid=%d][thread=%s] Released semaphore [name=%s][priority=%d]", pid,
                          t_ptr->thread_id.c_str(), this->name->c_str(),
                          priority);

                client->release_lock(ls, priority);
                return true;
            } else {
                throw CONTROL_ERROR(
                        "No semaphore found for the specified priority. [lock=%s][priority=%d][base priority=%d]",
                        this->name->c_str(),
                        priority, base_priority);
            }
        } else {
            LOG_DEBUG("[pid=%d][thread=%s] Lock count pending. [priority=%d][base priority=%d][count=%d]", pid,
                      t_ptr->thread_id.c_str(), priority,
                      base_priority,
                      counts[priority]->count);
            return false;
        }
    } else if (ls == Expired) {
        LOG_DEBUG(
                "[pid=%d][thread=%s][release_lock] Lock expired. [resetting all semaphores] [priority=%d][base priority=%d]",
                pid, t_ptr->thread_id.c_str(), priority, base_priority);
        reset_locks(priority, ls);
        return true;
    } else if (ls == ForceReleased) {
        counts[priority]->count = 0;
        reset_thread_locks(priority);
        LOG_DEBUG("[pid=%d][thread=%s][release_lock] Lock released by server.[priority=%d][base priority=%d]", pid,
                  t_ptr->thread_id.c_str(), priority,
                  base_priority);

        return false;
    }
    LOG_DEBUG("[pid=%d][thread=%s] Invalid lock state. [state=%s][priority=%d][base priority=%d]",
              pid, t_ptr->thread_id.c_str(), record_utils::get_lock_acquire_enum_string(ls).c_str(), priority,
              base_priority);
    return false;
}

void com::wookler::watergate::core::_semaphore_owner::reset() {
    LOG_DEBUG("[name=%s] Resetting all semaphores...", name->c_str());
    clear_locks();
}

void com::wookler::watergate::core::_semaphore_owner::check_expired_locks(uint64_t expiry_time) {
    PRECONDITION(expiry_time > 0);

    uint32_t counts[MAX_PRIORITY_ALLOWED];
    memset(counts, 0, (MAX_PRIORITY_ALLOWED * sizeof(uint32_t)));

    lock_table_manager *tm = get_table_manager();
    CHECK_NOT_NULL(tm);

    tm->check_expired_locks(expiry_time, counts);
    for (int ii = 0; ii < MAX_PRIORITY_ALLOWED; ii++) {
        if (counts[ii] > 0) {
            LOG_WARN("Force releasing semaphore locks. [name=%s][priority=%d][count=%d]", name->c_str(), ii,
                     counts[ii]);
            sem_t *sem = get(ii);
            if (IS_VALID_SEM_PTR(sem)) {
                for (int jj = 0; jj < counts[ii]; jj++) {
                    sem_post(sem);
                }
            } else {
                LOG_ERROR("Invalid semaphore pointer. [name=%s][priority=%d]", name->c_str(), ii);
            }
        }
    }
}

void com::wookler::watergate::core::_semaphore_owner::check_expired_records(uint64_t expiry_time) {
    PRECONDITION(expiry_time > 0);

    lock_table_manager *tm = get_table_manager();
    CHECK_NOT_NULL(tm);
    tm->reset_expired_records(expiry_time);
}