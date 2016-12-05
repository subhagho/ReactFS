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

#ifndef WATERGATE_ENV_UTILS_H
#define WATERGATE_ENV_UTILS_H

#include "__env.h"

using namespace com::wookler::reactfs::common;

REACTFS_NS_COMMON
                class env_utils {
                private:
                    static __env *env;

                public:

                    static void create_env(const string configfile) {
                        env = new __env();
                        CHECK_ALLOC(env, TYPE_NAME(__env));
                        env->create(configfile);
                        CHECK_ENV_STATE(env);
                    }

                    static void create_env(const string configfile, const string appname) {
                        env = new __env();
                        CHECK_ALLOC(env, TYPE_NAME(__env));
                        env->create(configfile, appname);
                        CHECK_ENV_STATE(env);
                    }

                    static const __env *get_env() {
                        CHECK_NOT_NULL(env);
                        CHECK_ENV_STATE(env);

                        return env;
                    }

                    static const Config *get_config() {
                        const __env *e = get_env();

                        return e->get_config();
                    }

                    static void dispose() {
                        LOG_DEBUG("[pid=%d] Releasing environment handle...", getpid());
                        CHECK_AND_FREE(env);

                        env = nullptr;
                    }
                };
REACTFS_NS_COMMON_END
#endif //WATERGATE_ENV_UTILS_H
