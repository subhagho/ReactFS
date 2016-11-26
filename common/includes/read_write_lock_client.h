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

#ifndef REACTFS_READ_WRITE_LOCK_CLIENT_H
#define REACTFS_READ_WRITE_LOCK_CLIENT_H

#include "read_write_lock.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                /*!
                 * Class creates the client environment to manager read/write locks.
                 * Locks are excepted to be shared amongst all threads within a process space
                 * and should not be created/disposed outside the context of the client.
                 *
                 */
                class read_write_lock_client {
                protected:
                    /// State of this instance of the client.
                    __state__ state;

                    /// Thread synchronisation mutex
                    mutex thread_mutex;

                    /// Pointer to SHM based shared lock table.
                    shared_lock_table *table = nullptr;

                    /// Registry for locks created in the process space.
                    unordered_map<string, read_write_lock *> locks;

                    /// Create/initialize this instance of the lock environment.
                    void create(mode_t mode, bool is_manager = false);


                    /*!
                     * Get the pointer to the shared lock table.
                     *
                     * @return - Shared lock table pointer.
                     */
                    shared_lock_table *get_lock_table() {
                        CHECK_STATE_AVAILABLE(state);
                        return table;
                    }

                public:
                    /*!
                     * Dispose this instance of the lock client.
                     * Will attempt to release lock(s) if they are still acquired.
                     */
                    ~read_write_lock_client() {
                        std::lock_guard<std::mutex> guard(thread_mutex);
                        state.set_state(__state_enum::Disposed);
                        if (!locks.empty()) {
                            unordered_map<string, read_write_lock *>::iterator iter;
                            for (iter = locks.begin(); iter != locks.end(); iter++) {
                                read_write_lock *lock = iter->second;
                                CHECK_AND_FREE(lock);
                            }
                        }
                        CHECK_AND_FREE(table);
                    }

                    /*!
                     * Initialize the client environment.
                     */
                    void create();

                    /*!
                     * Add/Get the lock handle for the specified lock name.
                     * If lock already initialized will return a reference to the lock.
                     *
                     * @param name - Shared lock name.
                     * @return - Read/Write lock handle.
                     */
                    read_write_lock *add_lock(string name);

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
            }
        }
    }
}
#endif //REACTFS_READ_WRITE_LOCK_CLIENT_H
