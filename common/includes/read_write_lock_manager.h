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

#ifndef REACTFS_READ_WRITE_LOCK_MANAGER_H
#define REACTFS_READ_WRITE_LOCK_MANAGER_H

#include "read_write_lock_client.h"
#include "__threads.h"

#define RW_LOCK_MANAGER_THREAD "RW_LOCK_MANAGER_THREAD"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                /*!
                 * Class defines the constructs for creating/managing lock groups.
                 */
                class read_write_lock_manager : public read_write_lock_client {
                private:
                    /// Use a local thread for lock management?
                    bool use_manager_thread = false;

                    /// Lock thread pointer
                    thread *manager_thread = nullptr;

                    /// Method to be used by the local management thread.
                    static void run(read_write_lock_manager *manager);

                public:
                    /*!<constructor
                     * Create a new instance of the Lock manager.
                     *
                     * @param group - Lock group name.
                     * @param use_manager_thread - Use local thread for lock management?
                     */
                    read_write_lock_manager(string group, bool use_manager_thread) : read_write_lock_client(group) {
                        this->use_manager_thread = use_manager_thread;
                    }

                    /*!<destructor
                     * Default destructor.
                     */
                    ~read_write_lock_manager() override {
                        state.set_state(__state_enum::Disposed);
                        if (NOT_NULL(manager_thread)) {
                            manager_thread->join();
                            delete (manager_thread);
                            manager_thread = nullptr;
                        }
                    }

                    /*!
                     * Initialize this instance of the lock manager.
                     *
                     * @param mode - Default mode to create the lock table with.
                     * @param reset - Reset the lock table?
                     */
                    void init(mode_t mode, bool reset);

                    /*!
                     * Reset the lock table.
                     */
                    void reset();

                    /*!
                     * Set this instance into error mode.
                     *
                     * @param e - Error
                     */
                    void set_error(const exception *e) {
                        state.set_error(e);
                    }

                    /*!
                     * Check and manage lock states for all locks registered to this lock group.
                     */
                    void check_lock_states();
                };

                /*!
                 * Runnable instance to be used for managing shared locks via the environment thread pool.
                 */
                class rw_lock_manager_callback : public __runnable_callback {
                private:
                    /// Pointer to the lock manager.
                    read_write_lock_manager *manager = nullptr;
                public:
                    /*!
                     * Create a new instance of the lock manager runnable.
                     *
                     * @param manager - Pointer to the lock manager.
                     */
                    rw_lock_manager_callback(read_write_lock_manager *manager) : __runnable_callback(
                            RW_LOCK_MANAGER_THREAD) {
                        CHECK_NOT_NULL(manager);
                        PRECONDITION(manager->get_state() == __state_enum::Available);
                        this->manager = manager;
                        LOG_INFO("Created lock manager callback...");
                    }


                    /*!
                     * Check if this runnable is ready to execute.
                     *
                     * @return - Ready to execute?
                     */
                    virtual bool can_run() override {
                        uint64_t now = time_utils::now();
                        return ((now - last_run_time) >= DEFAULT_LOCK_MGR_SLEEP);
                    }

                    /*!
                     * Execute method that will be called when this instance is ready to execute.
                     */
                    virtual void callback() override {
                        PRECONDITION(NOT_NULL(manager));
                        try {
                            if (manager->get_state() == __state_enum::Available) {
                                PRECONDITION(NOT_NULL(manager));
                                manager->check_lock_states();
                            }
                        } catch (const exception &e) {
                            LOG_ERROR("Lock manager thread terminated with error. [error=%s]", e.what());
                            manager->set_error(&e);
                        } catch (...) {
                            lock_error le = LOCK_ERROR("Lock manager thread terminated with unhandled exception.");
                            manager->set_error(&le);
                        }
                    }


                    /*!
                     * Not used, use error with exception.
                     */
                    virtual void error() override {
                        // Do nothing....
                    }

                    /*!
                     * Set the error for this lock manager instance.
                     *
                     * @param err - Exception handle.
                     */
                    virtual void error(exception *err) override {
                        manager->set_error(err);
                    }
                };
            }
        }
    }
}
#endif //REACTFS_READ_WRITE_LOCK_MANAGER_H
