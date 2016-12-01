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
                class read_write_lock_manager : public read_write_lock_client {
                private:
                    bool use_manager_thread = false;

                    thread *manager_thread = nullptr;


                    static void run(read_write_lock_manager *manager);

                public:
                    read_write_lock_manager(bool use_manager_thread) {
                        this->use_manager_thread = use_manager_thread;
                    }

                    ~read_write_lock_manager() override {
                        state.set_state(__state_enum::Disposed);
                        if (NOT_NULL(manager_thread)) {
                            manager_thread->join();
                            delete (manager_thread);
                            manager_thread = nullptr;
                        }
                    }

                    void init(mode_t mode, bool reset);

                    void reset();

                    void set_error(const exception *e) {
                        state.set_error(e);
                    }

                    void check_lock_states();
                };

                class rw_lock_manager_callback : public __runnable_callback {
                private:
                    read_write_lock_manager *manager = nullptr;
                public:
                    rw_lock_manager_callback(read_write_lock_manager *manager) : __runnable_callback(
                            RW_LOCK_MANAGER_THREAD) {
                        CHECK_NOT_NULL(manager);
                        PRECONDITION(manager->get_state() == __state_enum::Available);
                        this->manager = manager;
                        LOG_INFO("Created lock manager callback...");
                    }


                    virtual bool can_run() override {
                        uint64_t now = time_utils::now();
                        return ((now - last_run_time) >= DEFAULT_LOCK_MGR_SLEEP);
                    }


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


                    virtual void error() override {
                        // Do nothing....
                    }

                    virtual void error(exception *err) override {
                        manager->set_error(err);
                    }
                };
            }
        }
    }
}
#endif //REACTFS_READ_WRITE_LOCK_MANAGER_H
