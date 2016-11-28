/*
 * Copyright [2016] [Subhabrata Ghosh]
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
// Created by Subhabrata Ghosh on 24/11/16.
//

#ifndef REACTFS_NODE_INIT_MANAGER_H
#define REACTFS_NODE_INIT_MANAGER_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/log_utils.h"

#include "mount_manager.h"
#include "node_server_env.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                namespace server {

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
                            CHECK_ALLOC(server_env, TYPE_NAME(node_server_env));

                            server_env->init(reset);

                            mount_manager *mm = new mount_manager(server_env, reset);
                            CHECK_ALLOC(mm, TYPE_NAME(mount_manager));

                            server_env->add_env_data(mm);
                            CHECK_AND_FREE(mm);
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
#endif //REACTFS_NODE_INIT_MANAGER_H
