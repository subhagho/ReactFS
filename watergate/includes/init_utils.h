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
// Created by Subhabrata Ghosh on 13/10/16.
//

#ifndef WATERGATE_INIT_UTILS_H
#define WATERGATE_INIT_UTILS_H

#include "common/includes/__env.h"
#include "common/includes/init_utils.h"
#include "control_def.h"
#include "control_manager.h"
#include "resource_factory.h"
#include "watergate/src/core/ext/resource_helper.h"

using namespace com::wookler::reactfs::common;
using namespace com::wookler::watergate::core;
using namespace com::wookler::watergate::core::ext;

namespace com {
    namespace wookler {
        namespace watergate {
            namespace core {
                class init_utils : public env_utils {
                private:
                    static control_client *client;

                public:
                    static void init_resource_factory(const ConfigValue *config) {
                        CHECK_NOT_NULL(config);

                        resource_factory::configure(config);

                        resource_helper::register_creators();
                    }

                    static control_manager *
                    init_control_manager(const __env *env, const string path, bool reset = false,
                                         bool run_thread = true) {
                        CHECK_NOT_NULL(env);
                        CHECK_NOT_EMPTY(path);

                        const Config *config = env->get_config();
                        CHECK_NOT_NULL(config);

                        const ConfigValue *m_config = config->find(path);
                        CHECK_NOT_NULL(m_config);

                        init_resource_factory(m_config);

                        control_manager *manager = new control_manager(reset, run_thread);
                        CHECK_ALLOC(manager, TYPE_NAME(control_manager));
                        manager->init(env->get_app(), m_config);

                        return manager;
                    }

                    static const control_client *init_control_client(const __env *env, const string path) {
                        CHECK_NOT_NULL(env);
                        CHECK_NOT_EMPTY(path);

                        const Config *config = env->get_config();
                        CHECK_NOT_NULL(config);

                        const ConfigValue *c_config = config->find(path);
                        CHECK_NOT_NULL(c_config);
                        init_resource_factory(c_config);

                        const ConfigValue *dn = c_config->find(CONFIG_DEF_NODE_PATH);
                        CHECK_NOT_NULL(dn);

                        control_client *control = new control_client();
                        CHECK_ALLOC(control, TYPE_NAME(control_client));

                        control->init(env->get_app(), dn);

                        set_client(control);

                        return control;
                    }

                    static const control_client *get_client() {
                        CHECK_NOT_NULL(client);
                        CHECK_STATE_AVAILABLE(client->get_state());
                        return client;
                    }

                    static void set_client(control_client *client_p) {
                        CHECK_NOT_NULL(client_p);
                        CHECK_STATE_AVAILABLE(client_p->get_state());

                        client = client_p;
                    }

                    static void dispose() {
                        env_utils::dispose();

                        LOG_DEBUG("[pid=%d] Releasing control client...", getpid());
                        CHECK_AND_FREE(client);

                        client = nullptr;
                    }
                };
            }
        }
    }
}
#endif //WATERGATE_INIT_UTILS_H
