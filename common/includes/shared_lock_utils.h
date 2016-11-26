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
// Created by Subhabrata Ghosh on 25/11/16.
//

#ifndef REACTFS_SHARED_LOCK_UTILS_H
#define REACTFS_SHARED_LOCK_UTILS_H

#include "read_write_lock_manager.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {

                class shared_lock_utils {
                private:
                    static mutex shared_mutex;
                    static bool is_manager;
                    static read_write_lock_client *instance;
                public:
                    static read_write_lock_manager *create_manager(mode_t mode, bool use_thread) {
                        std::lock_guard<std::mutex> guard(shared_mutex);
                        if (NOT_NULL(instance) && !is_manager) {
                            throw LOCK_ERROR("Lock environment already initialized in client mode.");
                        }
                        if (is_manager && NOT_NULL(instance)) {
                            return get_manager();
                        }
                        is_manager = true;

                        read_write_lock_manager *manager = new read_write_lock_manager(use_thread);
                        CHECK_NOT_NULL(manager);
                        manager->init(mode);

                        instance = manager;
                        return manager;
                    }

                    static read_write_lock_client *create_client() {
                        std::lock_guard<std::mutex> guard(shared_mutex);
                        if (NOT_NULL(instance)) {
                            return instance;
                        }
                        is_manager = false;
                        instance = new read_write_lock_client();
                        CHECK_NOT_NULL(instance);
                        instance->create();

                        return instance;
                    }

                    static read_write_lock_client *get() {
                        CHECK_NOT_NULL(instance);
                        return instance;
                    }

                    static read_write_lock_manager *get_manager() {
                        CHECK_NOT_NULL(instance);
                        PRECONDITION(is_manager);

                        return static_cast<read_write_lock_manager *>(instance);
                    }

                    static void dispose() {
                        std::lock_guard<std::mutex> guard(shared_mutex);
                        CHECK_AND_FREE(instance);
                    }
                };
            }
        }
    }
}
#endif //REACTFS_SHARED_LOCK_UTILS_H
