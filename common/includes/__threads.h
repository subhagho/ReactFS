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

//
// Created by Subhabrata Ghosh on 23/11/16.
//

#ifndef REACTFS_THREADS_H
#define REACTFS_THREADS_H

#include <mutex>
#include <unordered_map>
#include <queue>

#include "common.h"
#include "common_utils.h"
#include "base_error.h"
#include "__callback.h"
#include "log_utils.h"
#include "__alarm.h"
#include "process_utils.h"
#include "timer.h"

REACTFS_NS_COMMON

                /*!
                 * Enum defines the thread states for managed threads.
                 */
                typedef enum __thread_state_enum__ {
                    /// Thread is currently available (not running)
                            TS_AVAILABLE = 0,
                    /// Thread is currently executing a task.
                            TS_RUNNING,
                    /// Thread has been stopped.
                            TS_STOPPED,
                    /// Thread terminated due to an exception.
                            TS_EXCEPTION,
                    /// Thread state is unknown.
                            TS_UNKNOWN
                } __thread_state_enum;

                /*!
                 * State for a managed thread.
                 */
                class __thread_state {
                private:
                    /// Current status of the thread instance.
                    __thread_state_enum state = __thread_state_enum::TS_UNKNOWN;
                    /// Exception handle, in case a thread terminated due to error.
                    const exception *error = nullptr;

                public:
                    /*!
                     * Get the current state of the owner thread.
                     *
                     * @return - Thread state.
                     */
                    const __thread_state_enum get_state() const {
                        return state;
                    }

                    /*!
                     * Get the current state as a string.
                     *
                     * @return - Current state as string.
                     */
                    string get_state_string() const {
                        switch (state) {
                            case TS_UNKNOWN:
                                return "Unknown";
                            case TS_RUNNING:
                                return "Running";
                            case TS_AVAILABLE:
                                return "Available";
                            case TS_STOPPED:
                                return "Stopped";
                            case TS_EXCEPTION:
                                return "Exception";
                            default:
                                return common_consts::EMPTY_STRING;
                        }
                    }

                    /*!
                     * Get the current state of the owner thread.
                     *
                     * @param s - Current thread state.
                     */
                    void set_state(__thread_state_enum s) {
                        state = s;
                    }

                    /*!
                     * Set the thread state to exception and capture the cause.
                     *
                     * @param e - Error handle.
                     */
                    void set_error(const exception *e) {
                        error = e;
                        state = __thread_state_enum::TS_EXCEPTION;
                    }

                    /*!
                     * Get the exception, in case the thread is in error state.
                     *
                     * @return - Cause of error.
                     */
                    const exception *get_error() const {
                        if (state == __thread_state_enum::TS_EXCEPTION) {
                            return error;
                        }
                        return NULL;
                    }

                    /*!
                     * Check if this thread is in error state.
                     *
                     * @return - Has error?
                     */
                    bool has_error() const {
                        return (state == __thread_state_enum::TS_EXCEPTION);
                    }

                    /*!
                     * Check if the thread is available.
                     *
                     * @return - Is available?
                     */
                    bool is_available() const {
                        return (state == __thread_state_enum::TS_AVAILABLE);
                    }

                    /*!
                     * Check if the thread has been stopped.
                     *
                     * @return - Is stopped?
                     */
                    bool is_stopped() const {
                        return (state == __thread_state_enum::TS_STOPPED);
                    }

                    /*!
                     * Check if the thread is currently executing a task.
                     *
                     * @return - Is executing task?
                     */
                    bool is_running() const {
                        return (state == __thread_state_enum::TS_RUNNING);
                    }
                };

                /*!
                 * Shared queue to registering tasks
                 * that will be executed by the thread pool.
                 *
                 */
                class __thread_stack {
                protected:
                    /// Concurrency lock.
                    mutex __lock;
                    /// Task queue.
                    queue<__callback *> task_queue;

                public:
                    virtual ~__thread_stack() {

                    }

                    /*!
                     * Get the next task in the queue, if any tasks are pending.
                     *
                     * @return - Next task callback.
                     */
                    virtual __callback *next_task() {
                        unique_lock<std::mutex> lock(__lock);
                        if (!IS_EMPTY(task_queue)) {
                            __callback *c = task_queue.front();
                            task_queue.pop();
                            CHECK_NOT_NULL(c);
                            return c;
                        }
                        return nullptr;
                    }

                    /*!
                     * Add a new task to the queue.
                     *
                     * @param task - Task callback to be executed.
                     */
                    virtual void add_task(__callback *task) {
                        CHECK_NOT_NULL(task);
                        unique_lock<std::mutex> lock(__lock);
                        task_queue.push(task);
                    }

                    /*!
                     * Check if the task queue is empty.
                     *
                     * @return - Is empty?
                     */
                    bool is_queue_empty() {
                        return task_queue.empty();
                    }
                };


                /*!
                 * Task callback that can be executed periodically.
                 * This is used to launch background jobs that need to keep running when
                 * certain conditions are statisfied.
                 */
                class __runnable_callback : public __callback {
                protected:
                    /// Task name (should be unique for a thread pool).
                    string name;

                    /// Last time this callback was executed.
                    uint64_t last_run_time;
                public:
                    /*! <constructor
                     * Create a new task instance.
                     *
                     * @param name - Task name.
                     * @return
                     */
                    __runnable_callback(string name) {
                        PRECONDITION(!IS_EMPTY(name));
                        this->name = name;
                    }

                    /*!
                     * Set the last execution timestamp.
                     *
                     * @param last_run_time - Last execution timestamp.
                     */
                    void set_last_run_time(uint64_t last_run_time) {
                        this->last_run_time = last_run_time;
                    }

                    /*!
                     * Get the last execution timestamp.
                     *
                     * @return - Last executed time.
                     */
                    uint64_t get_last_run_time() {
                        return this->last_run_time;
                    }

                    /*!
                     * Get the name of this instance of the task.
                     *
                     * @return - Task name.
                     */
                    string get_name() {
                        return this->name;
                    }

                    /*!
                     * Check if this task is ready to be executed.
                     *
                     * @return - Should be executed?
                     */
                    virtual bool can_run() = 0;


                    /*!
                     * Virtual method to be implemented by sub-classes.
                     * This will be invoked in case of normal execution.
                     */
                    virtual void callback() = 0;

                    /*!
                     * Virtual method to be implemented by sub-classes.
                     * This will be invoked in case of error conditions.
                     */
                    virtual void error() = 0;

                    /*!
                     * Virtual method to be implemented by sub-classes.
                     * This will be invoked in case of error conditions.
                     *
                     * @param err - Exception handle of the error.
                     */
                    virtual void error(exception *err) = 0;
                };

                /*!
                 * Task stack used to register and execute background tasks.
                 * The task will keep getting executed based on the response of the can_run() method
                 * till they explicitly are removed from the stack.
                 */
                class __runnable_stack : public __thread_stack {
                private:
                    /// Map to register the tasks.
                    unordered_map<string, __callback *> map;
                public:
                    virtual ~__runnable_stack() {
                        unique_lock<std::mutex> lock(__lock);
                        if (!IS_EMPTY(map)) {
                            unordered_map<string, __callback *>::iterator iter;
                            for (iter = map.begin(); iter != map.end(); iter++) {
                                auto c = iter->second;
                                CHECK_AND_FREE(c);
                            }
                        }
                        map.clear();
                    }

                    /*!
                     * Get the next task in the queue, if any tasks are pending.
                     * Will peridically check to see if there are any candidate task
                     * that are ready to be queued.
                     *
                     * @return - Next task callback.
                     */
                    virtual __callback *next_task() override {
                        unique_lock<std::mutex> lock(__lock);
                        if (IS_EMPTY(task_queue) && !IS_EMPTY(map)) {
                            unordered_map<string, __callback *>::const_iterator iter;
                            for (iter = map.begin(); iter != map.end(); iter++) {
                                __callback *c = iter->second;
                                CHECK_NOT_NULL(c);
                                __runnable_callback *rc = dynamic_cast<__runnable_callback *>(c);
                                CHECK_CAST(rc, TYPE_NAME(__callback), TYPE_NAME(__runnable_callback));
                                if (rc->can_run()) {
                                    rc->set_last_run_time(time_utils::now());
                                    task_queue.push(rc);
                                }
                            }
                        }
                        if (!IS_EMPTY(task_queue)) {
                            __callback *c = task_queue.front();
                            task_queue.pop();
                            CHECK_NOT_NULL(c);
                            return c;
                        }
                        return nullptr;
                    }

                    /*!
                    * Add a new task to the registry.
                    *
                    * @param task - Task callback to be executed.
                    */
                    virtual void add_task(__callback *task) override {
                        CHECK_NOT_NULL(task);
                        __runnable_callback *rc = dynamic_cast<__runnable_callback *>(task);
                        CHECK_CAST(rc, TYPE_NAME(__callback),
                                   TYPE_NAME(_runnable_callback));
                        unique_lock<std::mutex> lock(__lock);
                        map.insert({rc->get_name(), rc});
                    }

                    /*!
                     * Remove the specified task from the registry.
                     *
                     * @param name - Task name to remove.
                     * @return - Has been removed?
                     */
                    bool remove_task(string name) {
                        PRECONDITION(!IS_EMPTY(name));
                        unordered_map<string, __callback *>::const_iterator iter = map.find(name);
                        if (iter != map.end()) {
                            map.erase(name);
                            return true;
                        }
                        return false;
                    }
                };

                /*!
                 * Class defining a managed thread for executing pooled tasks.
                 */
                class __m_thread {
                private:
                    /// State of this thread instance.
                    __thread_state __state;
                    /// ID of this thread instance
                    string __id;
                    /// Thread handle that will be launched.
                    thread *__runner = nullptr;
                    /// Task queue, the thread should dequeue tasks from.
                    __thread_stack *queue = nullptr;
                    /// Sleep interval when the queue is empty.
                    uint64_t sleep_timeout = 0;
                    ///Thread pool name this thread belong to.
                    string pool_name;

                    /*!
                     * Run function that will be invoked by the thread.
                     */
                    void run() {
                        try {
                            this->__id = thread_utils::get_current_thread();

                            LOG_DEBUG("[pool=%s][thread=%s] Starting runner thread...", pool_name.c_str(),
                                      __id.c_str());
                            __alarm alarm(sleep_timeout);
                            __state.set_state(__thread_state_enum::TS_AVAILABLE);
                            while (!__state.is_stopped()) {
                                __callback *task = queue->next_task();
                                if (IS_NULL(task)) {
                                    WAIT_ALARM(alarm);
                                    continue;
                                }
                                try {
                                    __state.set_state(__thread_state_enum::TS_RUNNING);

                                    task->callback();
                                } catch (const exception &err) {
                                    base_error be = BASE_ERROR("[task=%s] Thread terminated with exception. [error=%s]",
                                                               task->get_uuid().c_str(),
                                                               err.what());
                                    LOG_WARN(be.what());
                                    task->error(&be);
                                } catch (...) {
                                    base_error be = BASE_ERROR(
                                            "[task=%s] Thread terminated with exception. [error=UNKNOWN]",
                                            task->get_uuid().c_str());
                                    LOG_WARN(be.what());
                                    task->error(&be);
                                }
                                __state.set_state(__thread_state_enum::TS_AVAILABLE);
                            }
                        } catch (const exception &e) {
                            base_error be = BASE_ERROR(
                                    "[pool=%s][thread=%s] Thread terminated with exception. [error=%s]",
                                    pool_name.c_str(), __id.c_str(), e.what());
                            LOG_CRITICAL(be.what());
                            __state.set_error(&be);
                            throw be;
                        } catch (...) {
                            base_error be = BASE_ERROR(
                                    "[pool=%s][thread=%s] Thread terminated with exception. [error=UNKNOWN]",
                                    pool_name.c_str(), __id.c_str());
                            LOG_CRITICAL(be.what());
                            __state.set_error(&be);
                            throw be;
                        }
                        LOG_DEBUG("[pool=%s][thread=%s] Stopped runner thread...", pool_name.c_str(),
                                  __id.c_str());
                    }

                public:
                    /*!<constructor
                     * Create a new instance of a managed thread.
                     *
                     * @param queue - Task queue to be used to dequeue tasks.
                     * @param sleep_timeout - Thread sleep timeout (in millisecods).
                     * @return
                     */
                    __m_thread(string name, __thread_stack *queue, uint64_t sleep_timeout) {
                        CHECK_NOT_NULL(queue);
                        PRECONDITION(sleep_timeout > 0);

                        this->pool_name = name;
                        this->queue = queue;
                        this->sleep_timeout = sleep_timeout;
                    }

                    /*!<constructor
                    * Create a new instance of a managed thread.
                    *
                    * @param queue - Task queue to be used to dequeue tasks.
                    * @param sleep_timeout - Thread sleep timeout (as duration string).
                    * @return
                    */
                    __m_thread(string name, __thread_stack *queue, string sleep_timeout) {
                        CHECK_NOT_NULL(queue);
                        PRECONDITION(!IS_EMPTY(sleep_timeout));
                        uint64_t stime = common_utils::parse_duration(sleep_timeout);
                        POSTCONDITION(stime > 0);

                        this->pool_name = name;
                        this->queue = queue;
                        this->sleep_timeout = stime;
                    }

                    /*!<destructor
                     *
                     * Destroy this instance of a managed thread.
                     */
                    ~__m_thread() {
                        if (!__state.has_error()) {
                            __state.set_state(__thread_state_enum::TS_STOPPED);
                        }
                        if (NOT_NULL(__runner)) {
                            if (__runner->joinable()) {
                                __runner->join();
                            }
                            CHECK_AND_FREE(__runner);
                        }
                    }

                    /*!
                     * Start the thread.
                     */
                    void start() {
                        if (__state.is_available() || __state.is_running()) {
                            return;
                        }
                        PRECONDITION(IS_NULL(__runner));
                        CHECK_NOT_NULL(this->queue);
                        __runner = new thread(&__m_thread::run, this);
                        CHECK_ALLOC(__runner, TYPE_NAME(thread));
                    }

                    /*!
                     * Stop the execution of this thread.
                     */
                    void stop() {
                        if (!__state.has_error()) {
                            __state.set_state(__thread_state_enum::TS_STOPPED);
                        } else {
                            const exception *e = __state.get_error();
                            if (NOT_NULL(e)) {
                                LOG_ERROR("[thread=%s] Terminated with exception. [error=%s]", e->what());
                            } else {
                                LOG_ERROR("[thread=%s] Terminated with exception. [error=Unknown casue.]");
                            }
                        }
                    }

                    /*!
                     * Wait to joing this thread instance.
                     */
                    void join() {
                        if (NOT_NULL(__runner) && __runner->joinable()) {
                            __runner->join();
                            CHECK_AND_FREE(__runner);
                        }
                    }
                };

                /*!
                 * Class defines a thread pool instance.
                 */
                class __thread_pool {
                private:
                    /// State of this thread pool.
                    __state__ state;
                    /// Task queue to be used for this pool.
                    __thread_stack *task_queue = nullptr;
                    /// Array of managed threads that are part of this pool.
                    vector<__m_thread *> *threads;
                    /// Number of thread(s) in the pool.
                    uint16_t max_t_count = 0;
                    /// Unique name of this thread pool.
                    string name;
                    /// Ensure the pending tasks are completed prior to shutting down.
                    bool flush_before_stop = false;

                public:
                    /*!<constructor
                     *
                     * Create a new instance of a thread pool.
                     *
                     * @param name - Thread pool name
                     * @param max_t_count - Number of thread(s) in the pool.
                     * @return
                     */
                    __thread_pool(string name, uint16_t max_t_count) {
                        PRECONDITION(max_t_count > 0);
                        PRECONDITION(!IS_EMPTY(name));
                        this->max_t_count = max_t_count;
                        this->name = string(name);
                    }

                    /*!<destructor
                     * Destroy this instance of thread pool.
                     */
                    ~__thread_pool() {
                        stop();
                        for (uint16_t ii = 0; ii < this->max_t_count; ii++) {
                            __m_thread *t = (*threads)[ii];
                            CHECK_AND_FREE(t);
                        }
                        CHECK_AND_FREE(threads);
                        CHECK_AND_FREE(task_queue);
                    }

                    /*!
                     * Get the name of this thread pool.
                     *
                     * @return - Thread pool name.
                     */
                    string get_name() {
                        return this->name;
                    }

                    /*!
                     * Setup this pool to be used to queue arbitary tasks.
                     *
                     * @param thread_sleep_timeout - Sleep timeout, in case queue is empty.
                     */
                    void create_task_queue(uint64_t thread_sleep_timeout, bool flush_queue = false) {
                        PRECONDITION(thread_sleep_timeout > 0);

                        task_queue = new __thread_stack();
                        CHECK_ALLOC(task_queue, TYPE_NAME(__thread_stack));

                        threads = new vector<__m_thread *>(this->max_t_count);
                        CHECK_ALLOC(threads, TYPE_NAME(vector));

                        for (uint16_t ii = 0; ii < this->max_t_count; ii++) {
                            __m_thread *t = new __m_thread(name, task_queue, thread_sleep_timeout);
                            CHECK_ALLOC(t, TYPE_NAME(__m_thread));
                            (*threads)[ii] = t;
                        }
                        this->flush_before_stop = flush_queue;

                        state.set_state(__state_enum::Available);
                    }

                    /*!
                     * Setup this pool to be used to executed registered tasks in the background.
                     *
                     * @param thread_sleep_timeout - Sleep timeout, in case queue is empty.
                     */
                    void create_task_registry(uint64_t thread_sleep_timeout) {
                        PRECONDITION(thread_sleep_timeout > 0);

                        task_queue = new __runnable_stack();
                        CHECK_ALLOC(task_queue, TYPE_NAME(__runnable_stack));

                        threads = new vector<__m_thread *>(this->max_t_count);
                        CHECK_ALLOC(threads, TYPE_NAME(vector));

                        for (uint16_t ii = 0; ii < this->max_t_count; ii++) {
                            __m_thread *t = new __m_thread(name, task_queue, thread_sleep_timeout);
                            CHECK_ALLOC(t, TYPE_NAME(__m_thread));
                            (*threads)[ii] = t;
                        }
                        state.set_state(__state_enum::Available);
                    }

                    /*!
                     * Start this thread pool.
                     */
                    void start() {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(NOT_NULL(threads));
                        PRECONDITION(!IS_EMPTY_P(threads));
                        PRECONDITION(NOT_NULL(task_queue));

                        for (uint16_t ii = 0; ii < this->max_t_count; ii++) {
                            __m_thread *t = (*threads)[ii];
                            CHECK_NOT_NULL(t);
                            t->start();
                        }
                    }

                    /*!
                     * Stop/Pause this thread pool. Can be restarted if required.
                     */
                    void stop() {
                        CHECK_AND_DISPOSE(state);
                        if (NOT_NULL(threads)) {
                            if (flush_before_stop) {
                                NEW_ALARM(5000, 0);
                                while (!task_queue->is_queue_empty()) {
                                    START_ALARM(0);
                                }
                            }
                            for (uint16_t ii = 0; ii < threads->size(); ii++) {
                                __m_thread *t = (*threads)[ii];
                                if (NOT_NULL(t))
                                    t->stop();
                            }
                            for (uint16_t ii = 0; ii < threads->size(); ii++) {
                                __m_thread *t = (*threads)[ii];
                                if (NOT_NULL(t))
                                    t->join();
                            }
                        }
                    }

                    /*!
                     * Add/register a new task to be executed by this pool.
                     *
                     * @param task - Task instance.
                     */
                    void add_task(__callback *task) {
                        CHECK_STATE_AVAILABLE(state);
                        CHECK_NOT_NULL(task);
                        CHECK_NOT_NULL(task_queue);

                        task_queue->add_task(task);
                    }

                    /*!
                     * Remove a registered task from the pool.
                     * Note: Only applicable if the pool is running with a registry.
                     *
                     * @param name - Task name to remove.
                     * @return - Has been removed?
                     */
                    bool remove_task(string name) {
                        CHECK_STATE_AVAILABLE(state);
                        CHECK_NOT_NULL(task_queue);
                        PRECONDITION(!IS_EMPTY(name));

                        __runnable_stack *q = dynamic_cast<__runnable_stack *>(task_queue);
                        CHECK_CAST(q, TYPE_NAME(__thread_stack), TYPE_NAME(__runnable_stack));
                        return q->remove_task(name);
                    }
                };
REACTFS_NS_COMMON_END

#endif //REACTFS_THREADS_H
