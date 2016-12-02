//
// Created by Subhabrata Ghosh on 09/11/16.
//

#include "common/includes/init_utils.h"
#include "common/includes/read_write_lock.h"
#include "common/includes/shared_lock_utils.h"

#define CONFIG_FILE getenv("CONFIG_FILE_PATH")

using namespace com::wookler::reactfs::common;

#define CONFIG_LOCK_COUNT 64

void test_rw_locks() {
    string rw_group("TEST-LOCK-GROUP-RW");

    read_write_lock_manager *manager = shared_lock_utils::get()->create_rw_manager(rw_group, 0755, true, true);
    CHECK_NOT_NULL(manager);

    read_write_lock_client *client = shared_lock_utils::get()->get_rw_client(rw_group);
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
}

void test_w_locks() {
    string w_group("TEST-LOCK-GROUP-W");

    write_lock_manager *manager = shared_lock_utils::get()->create_w_manager(w_group, 0755, true, true);
    CHECK_NOT_NULL(manager);

    write_lock_client *client = shared_lock_utils::get()->get_w_client(w_group);
    string name("test_lock_01");
    write_lock *lock = client->add_lock(name);
    lock->reset();

    POSTCONDITION(NOT_NULL(lock));

    char *user_name = getenv("USER");
    if (IS_NULL(user_name)) {
        user_name = getenv("USERNAME");
    }
    PRECONDITION(NOT_NULL(user_name));
    string uname(user_name);
    string txid = lock->get_lock(uname);
    POSTCONDITION(!IS_EMPTY(txid));
    LOG_INFO("Acquired write lock. [transation id=%s]", txid.c_str());
    bool r = lock->release_lock();
    POSTCONDITION(r);
    LOG_INFO("Released write lock. [transation id=%s]", txid.c_str());

    client->remove_lock(name);
}

int main(int argc, char **argv) {
    try {
        string rw_group("TEST-LOCK-GROUP-RW");

        PRECONDITION(argc > 0);
        char *cf = argv[1];
        CHECK_NOT_NULL(cf);

        string configf(cf);
        env_utils::create_env(configf);

        const __env *env = env_utils::get_env();
        CHECK_NOT_NULL(env);

        test_rw_locks();
        test_w_locks();

        shared_lock_utils::dispose();
        env_utils::dispose();

        exit(0);
    } catch (const exception &e) {
        LOG_ERROR(e.what());
        exit(-1);
    } catch (...) {
        LOG_ERROR("Unhandled exception occurred.");
        exit(-2);
    }
}
