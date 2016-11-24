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

#ifndef REACTFS_MOUNT_CLIENT_H
#define REACTFS_MOUNT_CLIENT_H

#include <unordered_map>

#include "core/includes/mount_structs.h"
#include "node_client_env.h"

using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                namespace client {
                    class mount_client {
                    private:
                        __mount_data *mounts = nullptr;
                        __mount_map *mount_map = nullptr;

                    public:
                        void init() {
                            node_client_env *n_env = node_init_client::get_client_env();
                            shared_mapped_ptr ptr = n_env->get_env_data(MOUNTS_KEY);
                            CHECK_NOT_NULL(ptr);

                            PRECONDITION(ptr.get()->get_size() == sizeof(__mount_data));
                            void *p = ptr.get()->get_data_ptr();
                            CHECK_NOT_NULL(p);

                            mounts = static_cast<__mount_data *>(p);
                            CHECK_NOT_NULL(mounts);

                            mount_map = new __mount_map();
                            for (uint16_t ii = 0; ii < mounts->mount_count; ii++) {
                                __mount_point mp = mounts->mounts[ii];
                                if (mp.state == __mount_state::MP_UNAVAILABLE) {
                                    continue;
                                }
                                string name_l = string(mp.lock_name);
                                POSTCONDITION(!IS_EMPTY(name_l));

                                exclusive_lock *lock = new exclusive_lock(&name_l);
                                CHECK_NOT_NULL(lock);

                                mount_map->add_mount_lock(lock->get_name(), lock);
                                mount_map->add_mount_index(string(mp.path), ii);
                            }
                        }

                        string get_mount_point() {

                        }
                    };
                }
            }
        }
    }
}

#endif //REACTFS_MOUNT_CLIENT_H
