//
// Created by Subhabrata Ghosh on 31/08/16.
//

#ifndef WATERGATE_CONTROL_OWNER_H
#define WATERGATE_CONTROL_OWNER_H

#include <stdio.h>
#include <mutex>

#include "includes/common/common_utils.h"
#include "includes/common/file_utils.h"
#include "includes/common/config.h"
#include "resource_def.h"
#include "control.h"
#include "includes/common/__state__.h"
#include "control_errors.h"
#include "includes/common/alarm.h"
#include "includes/common/metrics.h"

#define DEFAULT_MAX_TIMEOUT 30 * 1000
#define DEFAULT_LOCK_LOOP_SLEEP_TIME 10
#define METRIC_LOCK_PREFIX "metrics.lock.time"
#define METRIC_LOCK_TIMEOUT_PREFIX "metrics.lock.timeouts"
#define METRIC_QUOTA_PREFIX "metrics.quota.total"
#define METRIC_QUOTA_REACHED_PREFIX "metrics.quota.exhausted"

namespace com {
    namespace watergate {
        namespace core {
            class control_def {
            protected:
                __state__ state;
                unordered_map<string, _semaphore *> semaphores;

                void add_resource_lock(const __app *app, const ConfigValue *config, bool server);

                _semaphore *get_lock(string name) const {
                    CHECK_STATE_AVAILABLE(state);
                    if (!IS_EMPTY(name)) {
                        unordered_map<string, _semaphore *>::const_iterator iter = semaphores.find(name);
                        if (iter != semaphores.end()) {
                            return iter->second;
                        }
                    }
                    return nullptr;
                }

                void create(const __app *app, const ConfigValue *config, bool server);

            public:

                virtual ~control_def();

                virtual void init(const __app *app, const ConfigValue *config) = 0;

                const __state__ get_state() const {
                    return state;
                }

                string get_metrics_name(string prefix, string name, int priority) const {
                    if (priority >= 0)
                        return common_utils::format("%s::%s::priority_%d", prefix.c_str(), name.c_str(), priority);
                    else
                        return common_utils::format("%s::%s", prefix.c_str(), name.c_str());
                }
            };

            class control_client : public control_def {
            private:

                _lock_state try_lock(string name, int priority, int base_priority, double quota) const;

                _lock_state wait_lock(string name, int priority, int base_priority, double quota) const;

                bool release_lock(string name, int priority, int base_priority) const;

                _lock_state lock_get(string name, int priority, double quota, long timeout, int *err) const;

                bool has_valid_lock(string name, int priority) const;

            public:
                ~control_client() override {

                }

                void init(const __app *app, const ConfigValue *config) override {
                    create(app, config, false);
                }

                const string find_lock(const string name, resource_type_enum type) const {
                    CHECK_STATE_AVAILABLE(state);
                    if (!IS_EMPTY(name)) {
                        unordered_map<string, _semaphore *>::const_iterator iter;
                        for (iter = semaphores.begin(); iter != semaphores.end(); iter++) {
                            _semaphore *sem = iter->second;
                            if (NOT_NULL(sem)) {
                                _semaphore_client *c = static_cast<_semaphore_client *>(sem);
                                if (c->is_resource_type(type)) {
                                    if (c->accept(name)) {
                                        return *c->get_name();
                                    }
                                }
                            }
                        }
                    }

                    return EMPTY_STRING;
                }

                uint64_t get_quota(const string name) const {
                    _semaphore *sem = get_lock(name);
                    if (NOT_NULL(sem)) {
                        _semaphore_client *sem_c = static_cast<_semaphore_client *>(sem);
                        return sem_c->get_quota();
                    }
                    return 0;
                }

                _lock_state lock(string name, int priority, double quota, int *err) const {
                    return lock(name, priority, quota, DEFAULT_MAX_TIMEOUT, err);
                }

                _lock_state lock(string name, int priority, double quota, uint64_t timeout, int *err) const {
                    CHECK_STATE_AVAILABLE(state);

                    string m_name = get_metrics_name(METRIC_LOCK_PREFIX, name, priority);
                    START_TIMER(m_name);

                    timer t;
                    t.start();

                    _lock_state ret;
                    com::watergate::common::alarm a(DEFAULT_LOCK_LOOP_SLEEP_TIME * (priority + 1));
                    while (true) {
                        err = 0;
                        ret = lock_get(name, priority, quota, timeout, err);
                        if (ret != QuotaReached && ret != Retry) {
                            break;
                        }
                        if (!a.start()) {
                            ret = Error;
                            break;
                        }
                        if (t.get_current_elapsed() > timeout && (priority != 0)) {
                            TRACE("Lock call timeout. [name=%s][priority=%d][timeout=%lu]", name.c_str(), priority,
                                  timeout);
                            *err = ERR_CORE_CONTROL_TIMEOUT;
                            ret = Timeout;
                            break;
                        }
                    }
                    END_TIMER(m_name, m_name);
                    return ret;
                }

                thread_lock_ptr *register_thread(string lock_name) const {
                    CHECK_STATE_AVAILABLE(state);

                    _semaphore *sem = get_lock(lock_name);
                    if (IS_NULL(sem)) {
                        throw CONTROL_ERROR("No registered lock with specified name. [name=%s]", lock_name.c_str());
                    }

                    _semaphore_client *sem_c = static_cast<_semaphore_client *>(sem);
                    LOG_DEBUG("[pid=%d] Registering current thread.", getpid());

                    thread_lock_record *rec = sem_c->register_thread();
                    if (NOT_NULL(rec)) {
                        return rec->get_thread_ptr();
                    }
                    return nullptr;
                }

                bool release(string name, int priority) const;

                void dump() const {
                    LOG_DEBUG("**************[REGISTERED CONTROLS:%d]**************", getpid());
                    if (!IS_EMPTY(semaphores)) {
                        unordered_map<string, _semaphore *>::const_iterator iter;
                        for (iter = semaphores.begin(); iter != semaphores.end(); iter++) {
                            _semaphore *sem = iter->second;
                            if (NOT_NULL(sem)) {
                                _semaphore_client *c = static_cast<_semaphore_client *>(sem);
                                c->dump();
                            }
                        }
                    }
                    LOG_DEBUG("**************[REGISTERED CONTROLS:%d]**************", getpid());
                }

                void test_assert() const {
                    if (!IS_EMPTY(semaphores)) {
                        unordered_map<string, _semaphore *>::const_iterator iter;
                        for (iter = semaphores.begin(); iter != semaphores.end(); iter++) {
                            _semaphore *sem = iter->second;
                            if (NOT_NULL(sem)) {
                                _semaphore_client *c = static_cast<_semaphore_client *>(sem);
                                c->test_assert();
                            }
                        }
                    }
                }
            };
        }
    }
}

#endif //WATERGATE_CONTROL_OWNER_H
