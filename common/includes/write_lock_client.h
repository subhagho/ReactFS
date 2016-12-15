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

#ifndef REACTFS_WRITE_LOCK_CLIENT_H
#define REACTFS_WRITE_LOCK_CLIENT_H

#include "write_lock.h"

REACTFS_NS_COMMON
                /*!
                 * Class creates the client environment to manager read/write locks.
                 * Locks are excepted to be shared amongst all threads within a process space
                 * and should not be created/disposed outside the context of the client.
                 *
                 */
                class write_lock_client : public lock_client {
                protected:

                    /// Pointer to SHM based shared lock table.
                    w_lock_table *table = nullptr;

                    /// Registry for locks created in the process space.
                    unordered_map<string, write_lock *> locks;

                    virtual void create(mode_t mode, bool is_manager, bool reset) override;

                    /*!
                     * Get the pointer to the shared lock table.
                     *
                     * @return - Shared lock table pointer.
                     */
                    w_lock_table *get_lock_table() {
                        CHECK_STATE_AVAILABLE(state);
                        return table;
                    }

                public:
                    write_lock_client(string group) : lock_client(group, __lock_type::SHARED_LOCK_W) {

                    }

                    /*!
                     * Dispose this instance of the lock client.
                     * Will attempt to release lock(s) if they are still acquired.
                     */
                    virtual ~write_lock_client() {
                        std::lock_guard<std::mutex> guard(thread_mutex);
                        state.set_state(__state_enum::Disposed);
                        if (!locks.empty()) {
                            unordered_map<string, write_lock *>::iterator iter;
                            for (iter = locks.begin(); iter != locks.end(); iter++) {
                                write_lock *lock = iter->second;
                                table->remove_lock(lock->get_name());
                                CHECK_AND_FREE(lock);
                            }
                        }
                        CHECK_AND_FREE(table);
                    }

                    /*!
                     * Initialize the client environment.
                     *
                     * @param group - Lock group this client manages.
                     */
                    void create();

                    /*!
                     * Add/Get the lock handle for the specified lock name.
                     * If lock already initialized will return a reference to the lock.
                     *
                     * @param name - Shared lock name.
                     * @return - Read/Write lock handle.
                     */
                    write_lock *add_lock(string name);

                    /*!
                     * Remove this reference to the shared lock.
                     *
                     * @param name - Shared lock name.
                     * @return - Has lock been removed.
                     */
                    bool remove_lock(string name);

                    /*!
                     * Get the state of this handle.
                     *
                     * @return - Current state.
                     */
                    const __state_enum get_state() {
                        return state.get_state();
                    }

                    /*!
                     * Get the exception instance for this handle, if handle has error.
                     *
                     * @return - exception instance, or null if no error.
                     */
                    const exception *get_error() {
                        if (state.has_error()) {
                            return state.get_error();
                        }
                        return nullptr;
                    }
                };
REACTFS_NS_COMMON_END
#endif //REACTFS_WRITE_LOCK_CLIENT_H
