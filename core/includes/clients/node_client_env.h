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

#ifndef REACTFS_NODE_CLIENT_ENV_H
#define REACTFS_NODE_CLIENT_ENV_H

#include "common/includes/shared_lock_utils.h"
#include "core/includes/node_env.h"
#include "core/includes/block_shared_defs.h"
#include "watergate/includes/init_utils.h"

#include "mount_client.h"
using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                namespace client {
                    class node_client_env : public __node_env {
                        const control_client *control = nullptr;
                        mount_client *m_client = nullptr;
                        write_lock_client *block_lock_client = nullptr;

                    public:
                        node_client_env() : __node_env(false) {

                        }

                        ~node_client_env() {
                            CHECK_AND_FREE(m_client);
                            shared_lock_utils::dispose();
                            init_utils::dispose();
                        }

                        void init() {
                            create(false);
                            try {
                                block_lock_client = shared_lock_utils::get()->create_w_client(BLOCK_LOCK_GROUP);
                                CHECK_NOT_NULL(block_lock_client);

                                control = init_utils::init_control_client(env, CONTROL_CONFIG_PATH);
                                CHECK_NOT_NULL(control);

                                m_client = new mount_client();
                                CHECK_ALLOC(m_client, TYPE_NAME(mount_client));

                                shared_mapped_ptr *ptr = get_env_data(MOUNTS_KEY);
                                m_client->init(ptr);

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

                        mount_client *get_mount_client() {
                            CHECK_NOT_NULL(m_client);
                            return m_client;
                        }

                        bool is_priority_locked(string *path) {
                            PRECONDITION(!IS_EMPTY_P(path));

                            string m_path = m_client->get_mount_point(path);
                            if (!IS_EMPTY(m_path)) {
                                return control->has_loaded_lock(m_path);
                            }
                            return false;
                        }
                    };

                    class node_init_client {
                    private:
                        static node_client_env *client_env;
                    public:
                        static void create_node_env(string config_file) {
                            PRECONDITION(!IS_EMPTY(config_file));

                            init_utils::create_env(config_file);
                            const __env *env = init_utils::get_env();
                            CHECK_NOT_NULL(env);

                            client_env = new node_client_env();
                            CHECK_ALLOC(client_env, TYPE_NAME(node_client_env));

                            client_env->init();
                        }

                        static void shutdown() {
                            CHECK_AND_FREE(client_env);
                            init_utils::dispose();
                        }

                        static node_client_env *get_client_env() {
                            CHECK_NOT_NULL(client_env);

                            return client_env;
                        }
                    };
                }
            }
        }
    }
}


#endif //REACTFS_NODE_CLIENT_ENV_H
