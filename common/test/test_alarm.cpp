//
// Created by Subhabrata Ghosh on 20/09/16.
//

#include "test_alarm.h"


TEST_CASE("Test time function.", "[com::watergate::common::timer]") {
    env_utils::create_env(CONFIG_FILE);
    const __env *env = env_utils::get_env();
    REQUIRE(NOT_NULL(env));

    const Config *config = env_utils::get_config();
    REQUIRE(NOT_NULL(config));

    timer t;

    t.start();
    START_ALARM("4s");

    t.stop();

    LOG_INFO("Recorded elapsed time = %d", t.get_elapsed());

    env_utils::dispose();
}

TEST_CASE("Test alarm with callback.", "[com::watergate::common::alarm]") {
    env_utils::create_env(CONFIG_FILE);
    const __env *env = env_utils::get_env();
    REQUIRE(NOT_NULL(env));

    const Config *config = env_utils::get_config();
    REQUIRE(NOT_NULL(config));

    timer t;

    string *id = new string(common_utils::uuid());
    test_type *tt = new test_type(id);
    test_callback *tc = new test_callback(tt);

    t.start();
    START_ALARM("5s");

    t.pause();
    START_ALARM(1000 * 5);
    t.restart();
    START_ALARM_WITH_CALLBACK("5s", tc);
    t.stop();

    CHECK_AND_FREE(tt);
    CHECK_AND_FREE(tc);
    CHECK_AND_FREE(id);

    LOG_INFO("Recorded elapsed time = %d", t.get_elapsed());

    env_utils::dispose();
}