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
// Created by Subhabrata Ghosh on 11/10/16.
//

#include "watergate/includes/control_manager.h"

#define CONFIG_MANAGER_NODE_PATH "./manager"

void com::wookler::watergate::core::control_manager::run(control_manager *owner) {
    CHECK_NOT_NULL(owner);

    try {
        LOG_INFO("Starting control manager thread...");
        NEW_ALARM(DEFAULT_CONTROL_THREAD_SLEEP, 0);
        while (NOT_NULL(owner) && owner->state.is_available()) {
            START_ALARM(0);
            if (!IS_EMPTY(owner->semaphores)) {
                unordered_map<string, __semaphore *>::iterator iter;
                for (iter = owner->semaphores.begin(); iter != owner->semaphores.end(); iter++) {
                    __semaphore *sem = iter->second;
                    if (NOT_NULL(sem)) {
                        __semaphore_owner *c = static_cast<__semaphore_owner *>(sem);
                        CHECK_CAST(c, TYPE_NAME(__semaphore), TYPE_NAME(__semaphore_owner));
                        c->check_expired_locks(owner->lock_timeout);
                        c->check_expired_records(owner->record_timeout);
                    }
                }
            }
        }
        LOG_WARN("Shutting down control manager thread. [state=%s]", owner->state.get_state_string().c_str());
    } catch (const exception &e) {
        LOG_CRITICAL("[%d][name=control_manager] Control thread exited with error. [error=%s]", getpid(),
                     e.what());
    }
}

void com::wookler::watergate::core::control_manager::init(const __app *app, const ConfigValue *config) {
    CHECK_NOT_NULL(config);

    const ConfigValue *dn = config->find(CONFIG_DEF_NODE_PATH);
    CHECK_NOT_NULL(dn);

    create(app, dn, true, reset_lock_table);

    clear_locks();

    const ConfigValue *mn = config->find(CONFIG_MANAGER_NODE_PATH);
    if (NOT_NULL(mn)) {
        string ss = DEFAULT_LOCK_RESET_TIME;
        {
            const BasicConfigValue *cn = Config::get_value(CONST_CM_CONFIG_LOCK_RESET_TIME, mn);
            if (!IS_NULL(cn)) {
                const string sv = cn->get_value();
                if (!IS_EMPTY(sv)) {
                    ss = string(sv);
                }
            }
        }

        lock_timeout = common_utils::parse_duration(ss);
        PRECONDITION(lock_timeout > 0);
        LOG_INFO("Using lock timeout value %lu msec.", lock_timeout);
        ss = DEFAULT_RECORD_RESET_TIME;
        {
            const BasicConfigValue *cn = Config::get_value(CONST_CM_CONFIG_RECORD_RESET_TIME, mn);
            if (!IS_NULL(cn)) {
                const string sv = cn->get_value();
                if (!IS_EMPTY(sv)) {
                    ss = string(sv);
                }
            }
        }

        record_timeout = common_utils::parse_duration(ss);
        PRECONDITION(record_timeout > 0);
    } else {
        lock_timeout = common_utils::parse_duration(DEFAULT_LOCK_RESET_TIME);
        record_timeout = common_utils::parse_duration(DEFAULT_RECORD_RESET_TIME);
    }
    LOG_INFO("Using record reset timeout value %lu msec.", record_timeout);

    if (start_lock_thread)
        start();
}