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
// Created by Subhabrata Ghosh on 19/09/16.
//
#include <unistd.h>
#include <iostream>
#include <thread>

#include "common/includes/common.h"
#include "common/includes/__env.h"
#include "watergate/includes/control_manager.h"
#include "watergate/includes/init_utils.h"

#define CONTROL_DEF_CONFIG_PATH "/configuration/control/def"
#define CONTROL_CONFIG_PATH "/configuration/control"


using namespace com::wookler::watergate::core;

#define CONTROL_NAME "dummy-resource-1"
#define REQUIRE _assert
#define METRIC_LOCK_TIME "lock.acquire.wait.time"

typedef struct {
    string thread_id;
    uint32_t thread_index;
    uint32_t priority;
    uint64_t elapsed_time;
    uint64_t lock_wait_time;
} thread_record;

void rund(const control_client *control, int priority, thread_record *record) {
    try {
        thread_lock_ptr *tptr = control->register_thread(CONTROL_NAME);
        if (IS_NULL(tptr)) {
            throw BASE_ERROR("Error registering thread. [name=%s]", CONTROL_NAME);
        }
        string tid = tptr->thread_id;
        record->priority = priority;
        record->thread_id = string(tid);
        timer tg, tl;
        tg.start();
        int count = 0;
        for (int ii = 0; ii < 8; ii++) {
            int err = 0;
            int retry_count = 0;
            while (true) {
                START_TIMER(METRIC_LOCK_TIME);
                tl.restart();
                err = 0;
                __lock_state r = control->lock(CONTROL_NAME, priority, 200, 5000 * (priority + 1), &err);
                tl.pause();
                END_TIMER(METRIC_LOCK_TIME, METRIC_LOCK_TIME);
                if (r == Locked && err == 0) {
                    LOG_INFO("Successfully acquired lock [thread=%s][name=%s][priority=%d][try=%d]", tid.c_str(),
                             CONTROL_NAME, priority,
                             ii);
                    count++;
                    usleep(5 * 1000);
                    bool r = control->release(CONTROL_NAME, priority);
                    LOG_INFO("Successfully released lock [thread=%s][name=%s][priority=%d][try=%d]", tid.c_str(),
                             CONTROL_NAME, priority,
                             ii);
                    break;
                } else if (err != 0) {
                    LOG_ERROR("Filed to acquired lock [thread=%s][name=%s][priority=%d][try=%d][response=%d][error=%d]",
                              tid.c_str(),
                              CONTROL_NAME, priority, ii, r, err);
                } else
                    LOG_ERROR("Filed to acquired lock [thread=%s][name=%s][priority=%d][try=%d][response=%d]",
                              tid.c_str(),
                              CONTROL_NAME, priority, ii, r);
                retry_count++;

                if (retry_count > 1000) {
                    throw BASE_ERROR("Exceeded max retry count. [thread=%s][priority=%d]", tid.c_str(), priority);
                }
            }
        }
        tg.stop();

        record->elapsed_time = tg.get_elapsed();
        record->lock_wait_time = tl.get_current_elapsed();

        LOG_INFO("[thread=%s][priority=%d] Finished execution...", tid.c_str(), priority);
    } catch (const exception &e) {
        cout << "ERROR : " << e.what() << "\n";
    } catch (...) {
        cout << "Unknown error...\n";
    }
}

void run(const control_client *control, int priority, thread_record *record) {
    try {
        thread_lock_ptr *tptr = control->register_thread(CONTROL_NAME);
        if (IS_NULL(tptr)) {
            throw BASE_ERROR("Error registering thread. [name=%s]", CONTROL_NAME);
        }
        string tid = tptr->thread_id;
        record->priority = priority;
        record->thread_id = string(tid);
        timer tg, tl;
        tg.start();
        int count = 0;
        for (int ii = 0; ii < 8; ii++) {
            int err = 0;
            int retry_count = 0;
            while (true) {
                START_TIMER(METRIC_LOCK_TIME);
                tl.restart();
                err = 0;
                __lock_state r = control->lock(CONTROL_NAME, priority, 200, 5000 * (priority + 1), &err);
                tl.pause();
                END_TIMER(METRIC_LOCK_TIME, METRIC_LOCK_TIME);

                if (r == Locked && err == 0) {
                    LOG_INFO("Successfully acquired lock [thread=%s][name=%s][priority=%d][try=%d]", tid.c_str(),
                             CONTROL_NAME, priority,
                             ii);
                    count++;
                    usleep(5 * 1000);
                    bool r = control->release(CONTROL_NAME, priority);
                    LOG_INFO("Successfully released lock [thread=%s][name=%s][priority=%d][index=%d]", tid.c_str(),
                             CONTROL_NAME, priority,
                             ii);
                    break;
                } else if (err != 0) {
                    LOG_ERROR("Filed to acquired lock [thread=%s][name=%s][priority=%d][try=%d][response=%d][error=%d]",
                              tid.c_str(),
                              CONTROL_NAME, priority, ii, r, err);
                } else
                    LOG_ERROR("Filed to acquired lock [thread=%s][name=%s][priority=%d][try=%d][response=%d]",
                              tid.c_str(),
                              CONTROL_NAME, priority, ii, r);
                retry_count++;

                if (retry_count > 1000) {
                    throw BASE_ERROR("Exceeded max retry count. [thread=%s][priority=%d]", tid.c_str(), priority);
                }
            }
        }
        tg.stop();

        record->elapsed_time = tg.get_elapsed();
        record->lock_wait_time = tl.get_current_elapsed();

        LOG_INFO("[thread=%s][priority=%d] Finished execution...", tid.c_str(), priority);
    } catch (const exception &e) {
        cout << "ERROR : " << e.what() << "\n";
    } catch (...) {
        cout << "Unknown error...\n";
    }
}

int main(int argc, char *argv[]) {

    try {
        if (argc < 1) {
            throw BASE_ERROR("Missing required argument. <config file>");
        }
        string cf(argv[1]);
        REQUIRE(!IS_EMPTY(cf));

        init_utils::create_env(cf);
        const __env *env = init_utils::get_env();
        REQUIRE(NOT_NULL(env));

        const Config *config = init_utils::get_config();
        REQUIRE(NOT_NULL(config));

        control_manager *manager = init_utils::init_control_manager(env, CONTROL_CONFIG_PATH);
        REQUIRE(NOT_NULL(manager));

        const control_client *control = init_utils::init_control_client(env, CONTROL_DEF_CONFIG_PATH);
        REQUIRE(NOT_NULL(control));

        bool r = metrics_utils::create_metric(METRIC_LOCK_TIME, AverageMetric, true);
        if (!r) {
            throw BASE_ERROR("Error creating metrics. [name=%s]", METRIC_LOCK_TIME);
        }

        int t_count = 6;

        thread *threads[t_count];
        thread_record *records[t_count];
        for (int ii = 0; ii < t_count - 1; ii++) {
            thread_record *tr = new thread_record();
            tr->thread_index = ii;
            thread *t = new thread(run, control, ii % 2, tr);

            threads[ii] = t;
            records[ii] = tr;
        }
        {
            int ii = t_count - 1;
            thread_record *tr = new thread_record();
            tr->thread_index = ii;
            thread *t = new thread(rund, control, 1, tr);

            threads[ii] = t;
            records[ii] = tr;
        }
        control->dump();

        for (int ii = 0; ii < t_count; ii++) {
            threads[ii]->join();
        }

        control->dump();

        manager->clear_locks();

        for (int ii = 0; ii < t_count; ii++) {
            thread_record *tr = records[ii];
            if (NOT_NULL(tr)) {
                LOG_WARN("[thread:%s][priority=%d] total elapsed time = %d, total lock wait time = %d",
                         tr->thread_id.c_str(),
                         tr->priority, tr->elapsed_time, tr->lock_wait_time);
            }
        }

        metrics_utils::dump();

        CHECK_AND_FREE(manager);

        init_utils::dispose();

    } catch (const exception &e) {
        cout << "ERROR : " << e.what() << "\n";
    } catch (...) {
        cout << "Unknown error...\n";
    }
}
