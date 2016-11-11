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
#include "watergate/src/core/control/dummy_resource.h"
#include "watergate/includes/init_utils.h"

#define CONTROL_DEF_CONFIG_PATH "/configuration/control/def"
#define CONTROL_CONFIG_PATH "/configuration/control"

using namespace com::wookler::watergate::core;

#define CONTROL_NAME "dummy-resource-1"
#define REQUIRE _assert
#define METRIC_LOCK_TIME "lock.acquire.wait.time"
#define LOCK_TABLE_NAME "Test lock-table"

int main(int argc, char *argv[]) {

    try {

        init_utils::create_env(getenv("CONFIG_FILE_PATH"));
        const __env *env = init_utils::get_env();
        REQUIRE(NOT_NULL(env));

        const Config *config = init_utils::get_config();
        REQUIRE(NOT_NULL(config));

        control_manager *manager = init_utils::init_control_manager(env, CONTROL_CONFIG_PATH);
        REQUIRE(NOT_NULL(manager));

        const control_client *control = init_utils::init_control_client(env, CONTROL_DEF_CONFIG_PATH);
        REQUIRE(NOT_NULL(control));

        thread_lock_ptr *tptr = control->register_thread(CONTROL_NAME);
        if (IS_NULL(tptr)) {
            throw BASE_ERROR("Error registering thread. [name=%s]", CONTROL_NAME);
        }
        string tid = tptr->thread_id;

        int err = 0;
        _lock_state r = control->lock(CONTROL_NAME, 0, 500, &err);
        REQUIRE(err == 0);
        REQUIRE(r == Locked);
        LOG_INFO("Successfully acquired lock [name=%s][priority=%d]", CONTROL_NAME, 0);

        bool b = control->release(CONTROL_NAME, 0);
        REQUIRE(b);
        LOG_INFO("Successfully released lock [name=%s][priority=%d]", CONTROL_NAME, 0);

        r = control->lock(CONTROL_NAME, 1, 500, &err);
        REQUIRE(err == 0);
        REQUIRE(r == Locked);
        LOG_INFO("Successfully acquired lock [name=%s][priority=%d]", CONTROL_NAME, 1);

        b = control->release(CONTROL_NAME, 1);
        REQUIRE(b);
        LOG_INFO("Successfully released lock [name=%s][priority=%d]", CONTROL_NAME, 1);

        control->dump();

        CHECK_AND_FREE(manager);

        init_utils::dispose();

        exit(0);

    } catch (const exception &e) {
        cout << "ERROR : " << e.what() << "\n";
        exit(-1);
    } catch (...) {
        cout << "Unknown error...\n";
        exit(-1);
    }
}
