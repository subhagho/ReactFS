//
// Created by Subhabrata Ghosh on 09/11/16.
//

#include "test_thread_pool.h"

using namespace com::wookler::reactfs::common;

#define CONFIG_LOCK_COUNT 8

TEST_CASE("Check thread pool tasks", "[com::wookler::reactfs::common::__thread_pool") {
    env_utils::create_env(CONFIG_FILE);
    const __env *env = env_utils::get_env();
    REQUIRE(NOT_NULL(env));

    string name = "test-thread-pool";
    __thread_pool pool(name, 4);
    pool.create_task_queue(1000, true);
    pool.start();

    vector<test_pool_callback *> tasks;
    for (int ii = 0; ii < CALLBACK_COUNT; ii++) {
        test_pool_callback *c = new test_pool_callback(ii);
        pool.add_task(c);
        tasks.push_back(c);
    }
    pool.stop();
    for (auto task : tasks) {
        CHECK_AND_FREE(task);
    }

    env_utils::dispose();
}

TEST_CASE("Check thread pool registry", "[com::wookler::reactfs::common::__thread_pool") {
    env_utils::create_env(CONFIG_FILE);
    const __env *env = env_utils::get_env();
    REQUIRE(NOT_NULL(env));

    string name = "test-registry-pool";
    __thread_pool pool(name, 4);
    pool.create_task_registry(1000);
    pool.start();

    for (int ii = 0; ii < CALLBACK_COUNT; ii++) {
        test_pool_callback *c = new test_pool_callback(ii);
        pool.add_task(c);
    }
    NEW_ALARM(5000, 0);
    START_ALARM(0);
    pool.stop();

    env_utils::dispose();
}