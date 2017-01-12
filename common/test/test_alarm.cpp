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
// Created by Subhabrata Ghosh on 20/09/16.
//

#include "test_alarm.h"
#include <boost/filesystem.hpp>

TEST_CASE("Test time function.", "[com::watergate::common::timer]") {
    const char* file = "test/data/test-conf.json";
    env_utils::create_env(boost::filesystem::absolute(file).string());
    const __env *env = env_utils::get_env();
    REQUIRE(NOT_NULL(env));

    const Config *config = env_utils::get_config();
    REQUIRE(NOT_NULL(config));

    timer t;

    t.start();

    NEW_ALARM("4s", 0);
    START_ALARM(0);

    t.stop();

    LOG_INFO("Recorded elapsed time = %d", t.get_elapsed());
    REQUIRE(t.get_elapsed() >= 4000);

    env_utils::dispose();
}

TEST_CASE("Test alarm with callback.", "[com::watergate::common::alarm]") {
    const char* file = "test/data/test-conf.json";
    env_utils::create_env(boost::filesystem::absolute(file).string());

    const __env *env = env_utils::get_env();
    REQUIRE(NOT_NULL(env));

    const Config *config = env_utils::get_config();
    REQUIRE(NOT_NULL(config));

    timer t;

    string id = common_utils::uuid();

    test_type tt(&id);
    test_callback tc(&tt);

    t.start();

    NEW_ALARM("5s", 0);
    START_ALARM(0);

    t.pause();
    NEW_ALARM(1000 * 5, 1);
    START_ALARM(1);

    t.restart();
    NEW_ALARM_WITH_CALLBACK("5s", &tc, 2);
    START_ALARM(2);

    t.stop();

    LOG_INFO("Recorded elapsed time = %d", t.get_elapsed());
    REQUIRE(id == tc.getCallBackContext());

    env_utils::dispose();

}
