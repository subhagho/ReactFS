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

#include <unordered_map>

#include "read_write_lock_manager.h"
#include "write_lock_manager.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {

                /*!
                 * Singleton class to manage shared lock groups.
                 */
                class shared_lock_utils {
                private:
                    static mutex shared_mutex;
                    static unordered_map<string, lock_client *> clients;
                    static unordered_map<string, lock_client *> managers;

                    /*!
                     * Find if a lock client has already been registered for the specified lock group.
                     *
                     * @param group - Lock group.
                     * @return - Lock client if already registered.
                     */
                    static lock_client *find_client(string group) {
                        if (!IS_EMPTY(clients)) {
                            unordered_map<string, lock_client *>::iterator iter = clients.find(group);
                            if (iter != clients.end()) {
                                return iter->second;
                            }
                        }
                        return nullptr;
                    }

                    static lock_client *find_manager(string group) {
                        if (!IS_EMPTY(clients)) {
                            unordered_map<string, lock_client *>::iterator iter = managers.find(group);
                            if (iter != managers.end()) {
                                return iter->second;
                            }
                        }
                        return nullptr;
                    }

                public:
                    static read_write_lock_manager *
                    create_rw_manager(string group, mode_t mode, bool use_thread, bool reset) {
                        std::lock_guard<std::mutex> guard(shared_mutex);
                        read_write_lock_manager *manager = get_rw_manager(group);
                        if (IS_NULL(manager)) {
                            manager = new read_write_lock_manager(group, use_thread);
                            CHECK_ALLOC(manager, TYPE_NAME(read_write_lock_manager));
                            manager->init(mode, reset);

                            managers.insert({group, manager});
                        }
                        return manager;
                    }

                    static read_write_lock_client *create_rw_client(string group) {
                        std::lock_guard<std::mutex> guard(shared_mutex);
                        read_write_lock_client *client = get_rw_client(group);
                        if (IS_NULL(client)) {
                            client = new read_write_lock_client(group);
                            CHECK_ALLOC(client, TYPE_NAME(read_write_lock_client));
                            client->create();
                        }
                        return client;
                    }

                    static read_write_lock_client *get_rw_client(string group) {
                        read_write_lock_client *client = nullptr;
                        lock_client *ptr = find_client(group);
                        if (NOT_NULL(ptr)) {
                            PRECONDITION(ptr->get_lock_type() == __lock_type::LOCK_RW);
                            client = dynamic_cast<read_write_lock_client *>(ptr);
                            CHECK_CAST(client, TYPE_NAME(lock_client), TYPE_NAME(read_write_lock_client));
                        } else {
                            ptr = find_manager(group);
                            if (NOT_NULL(ptr)) {
                                PRECONDITION(ptr->get_lock_type() == __lock_type::LOCK_RW);
                                client = dynamic_cast<read_write_lock_client *>(ptr);
                                CHECK_CAST(client, TYPE_NAME(lock_client), TYPE_NAME(read_write_lock_client));
                            }
                        }

                        return client;
                    }

                    static read_write_lock_manager *get_rw_manager(string group) {
                        read_write_lock_manager *manager = nullptr;
                        lock_client *ptr = find_manager(group);
                        if (NOT_NULL(ptr)) {
                            PRECONDITION(ptr->get_lock_type() == __lock_type::LOCK_RW);
                            manager = dynamic_cast<read_write_lock_manager *>(ptr);
                            CHECK_CAST(manager, TYPE_NAME(lock_client), TYPE_NAME(read_write_lock_manager));
                        }
                        return manager;
                    }

                    static write_lock_client *
                    create_w_manager(string group, mode_t mode, bool use_thread, bool reset) {
                        std::lock_guard<std::mutex> guard(shared_mutex);
                        write_lock_manager *manager = get_w_manager(group);
                        if (IS_NULL(manager)) {
                            manager = new write_lock_manager(group, use_thread);
                            CHECK_ALLOC(manager, TYPE_NAME(write_lock_client));
                            manager->init(mode, reset);

                            managers.insert({group, manager});
                        }
                        return manager;
                    }

                    static write_lock_client *create_w_client(string group) {
                        std::lock_guard<std::mutex> guard(shared_mutex);
                        write_lock_client *client = get_w_client(group);
                        if (IS_NULL(client)) {
                            client = new write_lock_client(group);
                            CHECK_ALLOC(client, TYPE_NAME(write_lock_client));
                            client->create();
                        }
                        return client;
                    }

                    static write_lock_client *get_w_client(string group) {
                        write_lock_client *client = nullptr;
                        lock_client *ptr = find_client(group);
                        if (NOT_NULL(ptr)) {
                            PRECONDITION(ptr->get_lock_type() == __lock_type::LOCK_W);
                            client = dynamic_cast<write_lock_client *>(ptr);
                            CHECK_CAST(client, TYPE_NAME(lock_client), TYPE_NAME(write_lock_client));
                        } else {
                            ptr = find_manager(group);
                            if (NOT_NULL(ptr)) {
                                PRECONDITION(ptr->get_lock_type() == __lock_type::LOCK_W);
                                client = dynamic_cast<write_lock_client *>(ptr);
                                CHECK_CAST(client, TYPE_NAME(lock_client), TYPE_NAME(write_lock_client));
                            }
                        }

                        return client;
                    }

                    static write_lock_manager *get_w_manager(string group) {
                        write_lock_manager *manager = nullptr;
                        lock_client *ptr = find_manager(group);
                        if (NOT_NULL(ptr)) {
                            PRECONDITION(ptr->get_lock_type() == __lock_type::LOCK_W);
                            manager = dynamic_cast<write_lock_manager *>(ptr);
                            CHECK_CAST(manager, TYPE_NAME(lock_client), TYPE_NAME(write_lock_manager));
                        }
                        return manager;
                    }

                    static void dispose() {
                        std::lock_guard<std::mutex> guard(shared_mutex);
                        if (!IS_EMPTY(clients)) {
                            unordered_map<string, lock_client *>::iterator iter;
                            for (iter = clients.begin(); iter != clients.end(); iter++) {
                                CHECK_AND_FREE(iter->second);
                            }
                            clients.clear();
                        }
                        if (!IS_EMPTY(managers)) {
                            unordered_map<string, lock_client *>::iterator iter;
                            for (iter = managers.begin(); iter != managers.end(); iter++) {
                                CHECK_AND_FREE(iter->second);
                            }
                            managers.clear();
                        }
                    }
                };
            }
        }
    }
}
#endif //REACTFS_SHARED_LOCK_UTILS_H
