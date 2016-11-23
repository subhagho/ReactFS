//
// Created by Subhabrata Ghosh on 09/11/16.
//


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

#ifndef REACTFS_TEST_SHARED_LOCK_H
#define REACTFS_TEST_SHARED_LOCK_H
#define CATCH_CONFIG_MAIN

#include <iostream>
#include "test_common.h"
#include "common/includes/init_utils.h"
#include "common/includes/__threads.h"

#define CONFIG_FILE getenv("CONFIG_FILE_PATH")
#define RUNTIME_DELTA 1000
#define CALLBACK_COUNT 5

class test_pool_callback : public __runnable_callback {
private:
    int id;
    uint64_t last_run_time;
public:
    test_pool_callback(int id) : __runnable_callback(common_utils::format("CALLBACK_%d", id)) {
        this->id = id;
    }

    virtual void callback() override {
        last_run_time = time_utils::now();
        LOG_INFO("[id=%d][time=%lu] Called...", id, last_run_time);
    }

    void error() override {
        LOG_ERROR("[id=%d][time=%lu] Unknown error occurred...");
    }

    void error(exception *err) override {
        LOG_ERROR("[id=%d][time=%lu] %s", id, last_run_time, err->what());
    }

    bool can_run() override {
        uint64_t now = time_utils::now();
        return ((now - last_run_time) > RUNTIME_DELTA);
    }
};

#endif //REACTFS_TEST_SHARED_LOCK_H
