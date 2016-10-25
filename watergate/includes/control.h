//
// Created by Subhabrata Ghosh on 15/09/16.
//

#ifndef WATERGATE_CONTROL_H
#define WATERGATE_CONTROL_H

#include <semaphore.h>
#include <vector>

#include "common/includes/lock_record_def.h"
#include "common/includes/common.h"
#include "common/includes/base_error.h"
#include "common/includes/common_utils.h"
#include "common/includes/config.h"
#include "common/includes/__state__.h"
#include "common/includes/__app.h"
#include "common/includes/log_utils.h"
#include "resource_def.h"
#include "lock_table.h"

#define DEFAULT_SEM_MODE 0760

#define CONST_SEM_CONFIG_NODE_RESOURCE "./resource"
#define CONST_SEM_CONFIG_PARAM_RESOURCE_NAME "name"
#define CONST_SEM_CONFIG_PARAM_RESOURCE_CLASS "class"
#define CONST_SEM_CONFIG_NODE_PRIORITIES "priorities"
#define CONST_SEM_CONFIG_NODE_MODE "mode"


#define CONST_CONTROL_ERROR_PREFIX "Control Object Error : "

#define CONTROL_ERROR(fmt, ...) control_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define CONTROL_ERROR_PTR(fmt, ...) new control_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))

#define IS_VALID_SEM_PTR(ptr) (NOT_NULL(ptr) && ptr != SEM_FAILED)


#define CONTROL_LOCK_PREFIX "/locks"

using namespace com::watergate::common;

namespace com {
    namespace watergate {
        namespace core {
            class control_error : public base_error {
            public:
                control_error(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                          CONST_CONTROL_ERROR_PREFIX,
                                                                                          mesg) {
                }
            };

            struct _struct_priority_record {
                int priority;
                mutex priority_lock;
                uint64_t count = 0;
                uint64_t index = 0;
            };

            class _semaphore {
            private:
                mode_t mode = DEFAULT_SEM_MODE;
                bool is_server;

                void create_sem(int index);


            protected:
                string *name;
                int priorities;
                int max_concurrent;
                sem_t **semaphores;
                resource_def *resource;
                lock_table *table;
                bool owner = false;

                void delete_sem(int index);

                sem_t *get(int index) const {
                    PRECONDITION(index >= 0 && index < priorities);

                    return semaphores[index];
                }


                bool is_priority_valid(int p) {
                    return (p >= 0 && p < this->priorities);
                }

                void create(const __app *app, const ConfigValue *config, bool server);

            public:
                virtual ~_semaphore();

                const string *get_name() const {
                    return this->name;
                }

                const int get_max_priority() const {
                    return priorities;
                }

                bool is_resource_type(resource_type_enum type) const {
                    if (NOT_NULL(resource)) {
                        return (resource->get_type() == type);
                    }
                    return false;
                }

                virtual void init(const __app *app, const ConfigValue *config) = 0;
            };

            class _semaphore_owner : public _semaphore {
            private:
                __state__ state;
                lock_table_manager *manager;

            public:
                _semaphore_owner() {
                    this->owner = true;
                }

                ~_semaphore_owner() {
                    if (NOT_NULL(semaphores)) {
                        for (int ii = 0; ii < priorities; ii++) {
                            delete_sem(ii);
                        }
                        free(semaphores);
                    }
                }

                inline lock_table_manager *get_table_manager() {
                    CHECK_NOT_NULL(table);
                    lock_table_manager *ptr = static_cast<lock_table_manager *>(table);
                    return ptr;
                }


                void init(const __app *app, const ConfigValue *config) override {
                    create(app, config, true);

                    table = new lock_table_manager();
                    manager = get_table_manager();
                    manager->init(*name, resource);

                    reset();
                }

                void clear_locks() {
                    for (int ii = 0; ii < priorities; ii++) {
                        sem_t *sem = get(ii);
                        if (IS_VALID_SEM_PTR(sem)) {
                            LOG_INFO("Clearing semaphore lock [%s:%d]", name->c_str(), ii);
                            int count = 0;
                            while (true) {
                                if (sem_trywait(sem) == 0) {
                                    count++;
                                } else {
                                    break;
                                }
                            }
                            LOG_DEBUG("[%s]Available free lock count = %d", name->c_str(), count);
                            if (count < max_concurrent)
                                count = max_concurrent;
                            for (int jj = 0; jj < count; jj++) {
                                sem_post(sem);
                            }
                        }
                    }
                }

                void reset();

                void check_expired_locks(uint64_t expiry_time);

                void check_expired_records(uint64_t expiry_time);

            };

            class _semaphore_client : public _semaphore {
            private:
                lock_table_client *client;
                vector<_struct_priority_record *> counts;
                unordered_map<string, thread_lock_record *> threads;

                void reset_locks(int priority, _lock_state state) {
                    counts[priority]->count = 0;

                    reset_thread_locks(priority);
                    client->release_lock(state, priority);
                    sem_t *lock = get(priority);
                    if (IS_VALID_SEM_PTR(lock)) {
                        LOG_DEBUG("Force Released semaphore [name=%s][priority=%d]", this->name->c_str(), priority);
                        if (sem_post(lock) != 0) {
                            throw CONTROL_ERROR("Semaphores in invalid state. [name=%s][priority=%d][errno=%s]",
                                                this->name->c_str(),
                                                priority, strerror(errno));
                        }
                    } else {
                        throw CONTROL_ERROR("No lock found for the specified priority. [lock=%s][priority=%d]",
                                            this->name->c_str(),
                                            priority);
                    }
                }

                inline thread_lock_record *get_thread_lock() {
                    thread_lock_record *r = nullptr;
                    string id = thread_lock_record::get_current_thread();

                    unordered_map<string, thread_lock_record *>::iterator iter = threads.find(id);
                    if (iter != threads.end()) {
                        r = iter->second;
                    }
                    return r;
                }


                void reset_thread_locks(int priority) {
                    unordered_map<string, thread_lock_record *>::iterator iter;
                    for (iter = threads.begin(); iter != threads.end(); iter++) {
                        thread_lock_record *rec = iter->second;
                        if (NOT_NULL(rec)) {
                            rec->reset(priority);
                        }
                    }
                }

            public:
                mutex sem_lock;

                ~_semaphore_client() {
                    if (NOT_NULL(semaphores)) {
                        for (int ii = 0; ii < priorities; ii++) {
                            delete_sem(ii);
                        }
                        free(semaphores);
                    }
                    if (!IS_EMPTY(counts)) {
                        for (int ii = 0; ii < priorities; ii++) {
                            if (NOT_NULL(counts[ii])) {
                                delete (counts[ii]);
                            }
                        }
                    }
                    if (!IS_EMPTY(threads)) {
                        unordered_map<string, thread_lock_record *>::iterator iter;
                        for (iter = threads.begin(); iter != threads.end(); iter++) {
                            thread_lock_record *rec = iter->second;
                            if (NOT_NULL(rec)) {
                                delete (rec);
                            }
                        }
                    }
                }

                void init(const __app *app, const ConfigValue *config) override {
                    create(app, config, false);

                    table = new lock_table_client();
                    client = get_table_client();
                    client->init(app, *name, resource);

                    for (int ii = 0; ii < priorities; ii++) {
                        _struct_priority_record *lc = new _struct_priority_record();
                        counts.push_back(lc);
                        counts[ii]->priority = ii;
                        counts[ii]->count = 0;
                    }
                }


                inline lock_table_client *get_table_client() {
                    CHECK_NOT_NULL(table);

                    lock_table_client *ptr = static_cast<lock_table_client *>(table);
                    return ptr;
                }

                uint64_t get_quota() {
                    return client->get_quota();
                }

                thread_lock_record *register_thread() {
                    thread_lock_record *r = nullptr;

                    string id = thread_lock_record::get_current_thread();
                    LOG_DEBUG("Registering thread. [id=%s]", id.c_str());
                    unordered_map<string, thread_lock_record *>::iterator iter = threads.find(id);
                    if (iter != threads.end()) {
                        r = iter->second;
                        CHECK_NOT_NULL(r);
                    } else {
                        LOG_DEBUG("Creating new thread handle. [id=%s]", id.c_str());
                        thread_lock_ptr *ptr = thread_lock_record::create_new_ptr(priorities);
                        CHECK_NOT_NULL(ptr);
                        r = new thread_lock_record(ptr, priorities);
                        CHECK_NOT_NULL(r);
                        threads.insert(make_pair(ptr->thread_id, r));
                    }
                    return r;
                }

                void update_metrics(int priority, long lock_time) {
                    thread_lock_record *rec = get_thread_lock();
                    if (NOT_NULL(rec)) {
                        rec->update_metrics(priority, lock_time);
                    }
                }

                bool has_valid_lock(int priority) {
                    _lock_state state = client->has_valid_lock(priority, 0);
                    return (state == Locked);
                }

                _lock_state try_lock(int priority, double quota, int base_priority, bool wait);

                _lock_state try_lock_base(double quota, int base_priority, bool wait);

                bool release_lock(int priority, int base_priority);

                bool release_lock_base(int base_priority);

                bool accept(const string name) {
                    if (NOT_NULL(resource)) {
                        return resource->accept(name);
                    }
                    return false;
                }

                void dump() {
                    LOG_DEBUG("**************[LOCK:%s:%d]**************", name->c_str(), getpid());
                    if (NOT_NULL(client)) {
                        client->dump();
                    }
                    if (!IS_EMPTY(counts)) {
                        for (int ii = 0; ii < priorities; ii++) {
                            LOG_DEBUG("\t[priority=%d] lock count=%d", ii,
                                      counts[ii]->count);
                        }
                    }
                    if (!IS_EMPTY(threads)) {
                        unordered_map<string, thread_lock_record *>::iterator iter;
                        for (iter = threads.begin(); iter != threads.end(); iter++) {
                            thread_lock_record *rec = iter->second;
                            if (NOT_NULL(rec)) {
                                rec->dump();
                            }
                        }
                    }
                    LOG_DEBUG("**************[LOCK:%s:%d]**************", name->c_str(), getpid());
                }

                void test_assert() {
                    if (!IS_EMPTY(counts)) {
                        pid_t pid = getpid();
                        string thread_id = thread_lock_record::get_current_thread();
                        for (int ii = 0; ii < priorities; ii++) {
                            LOG_DEBUG("[pid=%d][thread=%s] Lock status [lock=%s][priority=%d] count=%d", pid,
                                      thread_id.c_str(), name->c_str(),
                                      ii, counts[ii]->count);
                        }
                        for (int ii = 0; ii < priorities; ii++) {
                            _assert(counts[ii]->count == 0);
                        }
                    }
                }
            };
        }
    }
}
#endif //WATERGATE_CONTROL_H
