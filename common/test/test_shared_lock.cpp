//
// Created by Subhabrata Ghosh on 09/11/16.
//

#include "test_shared_lock.h"

using namespace com::wookler::reactfs::common;

#define CONFIG_LOCK_COUNT 8

TEST_CASE("Check basic shared lock operations", "[com::wookler::reactfs::common::read_write_lock") {
    env_utils::create_env(CONFIG_FILE);
    const __env *env = env_utils::get_env();
    REQUIRE(NOT_NULL(env));

    const Config *config = env_utils::get_config();
    REQUIRE(NOT_NULL(config));

    shared_lock_utils::create_manager(0755, true, true);
    read_write_lock_manager *manager = shared_lock_utils::get_manager();
    CHECK_NOT_NULL(manager);

    read_write_lock_client *client = shared_lock_utils::get();
    string name("test_lock_01");
    read_write_lock *lock = client->add_lock(name);
    lock->reset();

    POSTCONDITION(NOT_NULL(lock));

    char *user_name = getenv("USER");
    if (IS_NULL(user_name)) {
        user_name = getenv("USERNAME");
    }
    PRECONDITION(NOT_NULL(user_name));
    string uname(user_name);
    string txid = lock->write_lock(uname);
    POSTCONDITION(!IS_EMPTY(txid));
    LOG_INFO("Acquired write lock. [transation id=%s]", txid.c_str());
    bool r = lock->release_write_lock();
    POSTCONDITION(r);
    LOG_INFO("Released write lock. [transation id=%s]", txid.c_str());

    r = lock->read_lock();
    POSTCONDITION(r);
    LOG_INFO("Acquired read lock.");
    txid = lock->write_lock(uname, 1000);
    POSTCONDITION(IS_EMPTY(txid));
    lock->release_read_lock();
    LOG_INFO("Released read lock.");

    client->remove_lock(name);

    shared_lock_utils::dispose();
    env_utils::dispose();
}