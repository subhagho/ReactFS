//
// Created by Subhabrata Ghosh on 24/11/16.
//


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

#ifndef REACTFS_NODE_SERVER_ENV_H
#define REACTFS_NODE_SERVER_ENV_H

#include "core/includes/node_env.h"
#include "common/includes/__threads.h"
#include "watergate/includes/init_utils.h"

#define CONTROL_CONFIG_PATH "/configuration/node/control"
#define DEFAULT_THREAD_POOL_NAME "__DEFAULT_THREAD_POOL_"

using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                namespace server {

                    class node_server_env : public __node_env {
                    private:
                        control_manager *priority_manager = nullptr;
                        __thread_pool *default_pool = nullptr;
                    public:
                        node_server_env() : __node_env(true) {

                        }

                        ~node_server_env() {
                            CHECK_AND_DISPOSE(state);
                            CHECK_AND_FREE(priority_manager);
                            CHECK_AND_FREE(default_pool);
                        }

                        void init(bool reset) {
                            create();
                            try {
                                priority_manager = init_utils::init_control_manager(env, CONTROL_CONFIG_PATH, reset);
                                CHECK_NOT_NULL(priority_manager);

                                const Config *config = this->env->get_config();
                                CHECK_NOT_NULL(config);

                                const ConfigValue *node = config->find(NODE_ENV_CONFIG_THREAD_POOL);
                                CHECK_NOT_NULL(node);

                                const BasicConfigValue *s_node = Config::get_value(NODE_ENV_CONFIG_POOL_SIZE, node);
                                CHECK_NOT_NULL(s_node);
                                string s_size = s_node->get_value();
                                POSTCONDITION(!IS_EMPTY(s_size));

                                int p_size = atoi(s_size.c_str());
                                POSTCONDITION(p_size > 0);

                                default_pool = new __thread_pool(DEFAULT_THREAD_POOL_NAME, p_size);
                                CHECK_NOT_NULL(default_pool);

                            } catch (const exception &e) {
                                base_error err = BASE_ERROR(
                                        "Error occurred while creating node envirnoment. [error=%s]",
                                        e.what());
                                state.set_error(&err);
                                throw err;
                            } catch (...) {
                                base_error err = BASE_ERROR(
                                        "Error occurred while creating node envirnoment. [error=%s]",
                                        "UNKNOWN ERROR");
                                state.set_error(&err);
                                throw err;
                            }
                        }


                        bool add_env_data(__env_loader *loader) {
                            CHECK_STATE_AVAILABLE(state);
                            CHECK_NOT_NULL(loader);
                            PRECONDITION(!IS_EMPTY_P(loader->get_key()));
                            PRECONDITION(!IS_EMPTY_P(loader->get_config_path()));

                            const Config *config = env->get_config();
                            const ConfigValue *node = config->find(*loader->get_config_path());

                            __env_record *record = loader->load(node);
                            if (NOT_NULL(record)) {
                                void *ptr = write(loader->get_key(), record->data, record->size);
                                CHECK_NOT_NULL(ptr);

                                shared_mapped_ptr s_ptr = make_shared<__mapped_ptr>();
                                (*s_ptr).set_data_ptr(ptr, record->size);

                                data_index.insert({*(loader->get_key()), s_ptr});

                                loader->load_finished(s_ptr);

                                return true;
                            }
                            return false;
                        }

                        control_manager *get_priority_manager() {
                            CHECK_STATE_AVAILABLE(state);
                            return priority_manager;
                        }
                    };

                    class node_init_manager {
                    private:
                        static node_server_env *server_env;
                    public:
                        static void create_node_env(string config_file, bool reset = false) {
                            PRECONDITION(!IS_EMPTY(config_file));

                            init_utils::create_env(config_file);
                            const __env *env = init_utils::get_env();
                            CHECK_NOT_NULL(env);

                            server_env = new node_server_env();
                            CHECK_NOT_NULL(server_env);

                            server_env->init(reset);
                        }

                        static void shutdown() {
                            CHECK_AND_FREE(server_env);
                            init_utils::dispose();
                        }

                        static node_server_env *get_server_env() {
                            CHECK_NOT_NULL(server_env);

                            return server_env;
                        }
                    };
                }
            }
        }
    }
}
#endif //REACTFS_NODE_SERVER_ENV_H
