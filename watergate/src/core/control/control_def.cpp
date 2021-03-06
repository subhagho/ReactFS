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
// Created by Subhabrata Ghosh on 31/08/16.
//

#include "watergate/includes/control_def.h"

void com::wookler::watergate::core::control_def::create(const __app *app, const ConfigValue *config, bool server,
                                                        bool overwrite) {
    try {
        CHECK_NOT_NULL(app);
        CHECK_NOT_NULL(config);

        if (config->get_type() == ConfigValueTypeEnum::Node) {
            add_resource_lock(app, config, server, overwrite);
        } else if (config->get_type() == ConfigValueTypeEnum::List) {
            const ListConfigValue *list = static_cast<const ListConfigValue *>(config);
            const vector<ConfigValue *> values = list->get_values();
            if (!IS_EMPTY(values)) {
                for (auto v : values) {
                    add_resource_lock(app, v, server, overwrite);
                }
            }
        }

        state.set_state(Available);
    } catch (const exception &e) {
        state.set_error(&e);
        throw CONTROL_ERROR("Error creating control handle. \n\tNested : %s", e.what());
    }
}

void com::wookler::watergate::core::control_def::add_resource_lock(const __app *app, const ConfigValue *config,
                                                                   bool server, bool overwrite) {
    __semaphore *sem = nullptr;
    if (server) {
        sem = new __semaphore_owner();
        CHECK_ALLOC(sem, TYPE_NAME(__semaphore_owner));
    } else {
        sem = new __semaphore_client();
        CHECK_ALLOC(sem, TYPE_NAME(__semaphore_client));
    }
    sem->init(app, config, (server && overwrite));

    semaphores.insert(make_pair(*(sem->get_name()), sem));
    LOG_DEBUG("Added resource lock [%s]", sem->get_name()->c_str());

    for (int ii = 0; ii < sem->get_max_priority(); ii++) {
        string m = get_metrics_name(METRIC_LOCK_PREFIX, *sem->get_name(), ii);
        if (!IS_EMPTY(m)) {
            metrics_utils::create_metric(m, AverageMetric);
        }
        m = get_metrics_name(METRIC_LOCK_TIMEOUT_PREFIX, *sem->get_name(), ii);
        if (!IS_EMPTY(m)) {
            metrics_utils::create_metric(m, BasicMetric);
        }
    }
    string m = get_metrics_name(METRIC_QUOTA_PREFIX, *sem->get_name(), -1);
    if (!IS_EMPTY(m)) {
        metrics_utils::create_metric(m, BasicMetric);
    }
    m = get_metrics_name(METRIC_QUOTA_REACHED_PREFIX, *sem->get_name(), -1);
    if (!IS_EMPTY(m)) {
        metrics_utils::create_metric(m, BasicMetric);
    }
    LOG_INFO("Created new semaphore handle. [name=%s]...", sem->get_name()->c_str());
}

com::wookler::watergate::core::control_def::~control_def() {
    state.set_state(Disposed);
    if (!IS_EMPTY(semaphores)) {
        for (auto kv : semaphores) {
            if (NOT_NULL(kv.second)) {
                delete (kv.second);
            }
        }
        semaphores.clear();
    }
}

__lock_state
com::wookler::watergate::core::control_client::try_lock(string name, int priority, int base_priority,
                                                        double quota) const {
    CHECK_STATE_AVAILABLE(state);

    __semaphore *sem = get_lock(name);
    if (IS_NULL(sem)) {
        throw CONTROL_ERROR("No registered lock with specified name. [name=%s]", name.c_str());
    }
    __semaphore_client *sem_c = static_cast<__semaphore_client *>(sem);
    CHECK_CAST(sem_c, TYPE_NAME(__semaphore), TYPE_NAME(__semaphore_client));

    __lock_state r = __lock_state::None;

    if (IS_BASE_PRIORITY(priority)) {
        r = sem_c->try_lock_base(quota, base_priority, false);
        if (r == Locked) {
            string q_name = get_metrics_name(METRIC_QUOTA_PREFIX, name, -1);
            metrics_utils::update(q_name, quota);
        }
    } else
        r = sem_c->try_lock(priority, quota, base_priority, false);
    return r;
}

__lock_state
com::wookler::watergate::core::control_client::wait_lock(string name, int priority, int base_priority,
                                                         double quota) const {
    CHECK_STATE_AVAILABLE(state);

    __semaphore *sem = get_lock(name);
    if (IS_NULL(sem)) {
        throw CONTROL_ERROR("No registered lock with specified name. [name=%s]", name.c_str());
    }

    __semaphore_client *sem_c = static_cast<__semaphore_client *>(sem);
    CHECK_CAST(sem_c, TYPE_NAME(__semaphore), TYPE_NAME(__semaphore_client));

    __lock_state r = __lock_state::None;

    if (IS_BASE_PRIORITY(priority)) {
        r = sem_c->try_lock_base(quota, base_priority, true);
        if (r == Locked) {
            string q_name = get_metrics_name(METRIC_QUOTA_PREFIX, name, -1);
            metrics_utils::update(q_name, quota);
        }
    } else
        r = sem_c->try_lock(priority, quota, base_priority, true);
    return r;
}

bool com::wookler::watergate::core::control_client::release_lock(string name, int priority, int base_priority) const {
    CHECK_STATE_AVAILABLE(state);

    __semaphore *sem = get_lock(name);
    if (IS_NULL(sem)) {
        throw CONTROL_ERROR("No registered lock with specified name. [name=%s]", name.c_str());
    }

    __semaphore_client *sem_c = static_cast<__semaphore_client *>(sem);
    CHECK_CAST(sem_c, TYPE_NAME(__semaphore), TYPE_NAME(__semaphore_client));

    if (IS_BASE_PRIORITY(priority))
        return sem_c->release_lock_base(base_priority);
    else
        return sem_c->release_lock(priority, base_priority);
}

bool com::wookler::watergate::core::control_client::has_loaded_lock(string name) const {
    __semaphore *sem = get_lock(name);
    if (!IS_NULL(sem)) {
        return true;
    }
    return false;
}

bool com::wookler::watergate::core::control_client::has_valid_lock(string name, int priority) const {
    __semaphore *sem = get_lock(name);
    if (IS_NULL(sem)) {
        throw CONTROL_ERROR("No registered lock with specified name. [name=%s]", name.c_str());
    }

    __semaphore_client *sem_c = static_cast<__semaphore_client *>(sem);
    CHECK_CAST(sem_c, TYPE_NAME(__semaphore), TYPE_NAME(__semaphore_client));

    return sem_c->has_valid_lock(priority);
}

__lock_state
com::wookler::watergate::core::control_client::lock_get(string name, int priority, double quota, long __timeout,
                                                        int *err) const {
    CHECK_NOT_NULL(err);
    timer t;
    t.start();

    pid_t pid = getpid();
    string thread_id = thread_utils::get_current_thread();
    uint64_t timeout = 0;
    if (__timeout > 0) {
        timeout = (uint64_t) __timeout;
    }
    __lock_state ret = wait_lock(name, priority, priority, quota);
    if (ret == Error) {
        throw CONTROL_ERROR("Error acquiring base lock. [name=%s]", name.c_str());
    } else if (ret == Locked) {
        if ((t.get_current_elapsed() > timeout) && (priority != 0)) {
            release_lock(name, priority, priority);
            *err = ERR_CORE_CONTROL_TIMEOUT;
            ret = Timeout;
        } else {
            int locked_priority = priority;
            NEW_ALARM(DEFAULT_LOCK_LOOP_SLEEP_TIME * (priority + 1), 0);
            for (int ii = priority - 1; ii >= 0; ii--) {
                while (true) {
                    if (t.get_current_elapsed() > timeout) {
                        TRACE("[pid=%d] Lock call timeout. [name=%s][priority=%d][timeout=%d]", pid, name.c_str(),
                              priority, timeout);
                        *err = ERR_CORE_CONTROL_TIMEOUT;
                        ret = Timeout;
                        break;
                    }
                    bool error = false;
                    for (int jj = priority; jj >= locked_priority; jj--) {
                        if (!has_valid_lock(name, jj)) {
                            error = true;
                            break;
                        }
                    }
                    if (error) {
                        TRACE("[pid=%d] Prior lock timeout. [name=%s][priority=%d][timeout=%d]", pid, name.c_str(),
                              priority, timeout);
                        ret = Retry;
                        break;
                    }
                    ret = this->try_lock(name, ii, priority, quota);
                    if (ret == Locked || ret == QuotaReached || ret == Error) {
                        break;
                    } else {
                        START_ALARM(0);
                    }
                }
                if (ret != Locked) {
                    LOG_DEBUG(
                            "[pid=%d][thread=%s] Releasing acquired locks. [base priority=%d][locked priority=%d][return=%s]",
                            pid, thread_id.c_str(), priority, locked_priority,
                            record_utils::get_lock_acquire_enum_string(ret).c_str());
                    for (int jj = priority; jj >= locked_priority; jj--) {
                        bool r = release_lock(name, jj, priority);
                        TRACE(
                                "[pid=%d][thread=%s] Lock release status [%s]. [base priority=%d][priority=%d][return=%s]",
                                pid, thread_id.c_str(), (r ? "true" : "false"), priority, jj,
                                record_utils::get_lock_acquire_enum_string(ret).c_str());
                    }
                    break;
                } else {
                    locked_priority = ii;
                }
            }

            __semaphore *sem = get_lock(name);
            if (IS_NULL(sem)) {
                throw CONTROL_ERROR("No registered lock with specified name. [name=%s]", name.c_str());
            }

            __semaphore_client *sem_c = static_cast<__semaphore_client *>(sem);
            CHECK_CAST(sem_c, TYPE_NAME(__semaphore), TYPE_NAME(__semaphore_client));

            t.stop();
            long ts = t.get_elapsed();
            sem_c->update_metrics(priority, ts);
        }
    }

    if (ret == Timeout) {
        string m = get_metrics_name(METRIC_LOCK_TIMEOUT_PREFIX, name, -1);
        if (!IS_EMPTY(m)) {
            metrics_utils::update(m, 1);
        }
    } else if (ret == QuotaReached) {
        string m = get_metrics_name(METRIC_QUOTA_REACHED_PREFIX, name, -1);
        if (!IS_EMPTY(m)) {
            metrics_utils::update(m, 1);
        }
    }
    return ret;
}

bool com::wookler::watergate::core::control_client::release(string name, int priority) const {
    CHECK_STATE_AVAILABLE(state);

    bool r = true;
    for (int ii = priority; ii >= 0; ii--) {
        if (!release_lock(name, ii, priority)) {
            r = false;
        }
    }
    return r;
}
