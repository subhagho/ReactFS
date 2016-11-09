//
// Created by Subhabrata Ghosh on 09/11/16.
//

#include "test_shared_lock.h"

using namespace com::watergate::common;
using namespace com::wookler::reactfs::common;

#define CONFIG_LOCK_COUNT 8

TEST_CASE("Create shared lock", "[com::wookler::reactfs::common::read_write_lock") {
    env_utils::create_env(CONFIG_FILE);
    const __env *env = env_utils::get_env();
    REQUIRE(NOT_NULL(env));

    const Config *config = env_utils::get_config();
    REQUIRE(NOT_NULL(config));

    lock_manager *manager = new lock_manager();
    manager->create(0755, CONFIG_LOCK_COUNT);

    string name("test_lock_01");
    read_write_lock *lock = new read_write_lock(&name, manager->get_lock_table());
    POSTCONDITION(NOT_NULL(lock));

    CHECK_AND_FREE(manager);
    env_utils::dispose();
}