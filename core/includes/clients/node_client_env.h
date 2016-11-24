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

#include "core/includes/node_env.h"
#include "watergate/includes/init_utils.h"

using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                namespace client {
                    class node_client_env : public __node_env {

                    public:
                        node_client_env() : __node_env(false) {

                        }

                        void init() {
                            create();
                        }
                    };

                    class node_init_client {
                    private:
                        static node_client_env *client_env;
                    public:
                        static void create_node_env(string config_file, bool reset = false) {
                            PRECONDITION(!IS_EMPTY(config_file));

                            init_utils::create_env(config_file);
                            const __env *env = init_utils::get_env();
                            CHECK_NOT_NULL(env);

                            client_env = new node_client_env();
                            CHECK_NOT_NULL(client_env);

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
