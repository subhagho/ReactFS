//
// Created by Subhabrata Ghosh on 09/11/16.
//

#include "common/includes/init_utils.h"
#include "common/includes/read_write_lock.h"

#define CONFIG_FILE getenv("CONFIG_FILE_PATH")

using namespace com::watergate::common;
using namespace com::wookler::reactfs::common;

#define CONFIG_LOCK_COUNT 4

int main(int argc, char **argv) {
    try {

        PRECONDITION(argc > 0);
        char *cf = argv[1];
        CHECK_NOT_NULL(cf);

        string configf(cf);
        env_utils::create_env(configf);

        const __env *env = env_utils::get_env();
        CHECK_NOT_NULL(env);

        lock_manager *manager = new lock_manager();
        manager->create(0755, CONFIG_LOCK_COUNT);

        string name("test_lock_01");
        read_write_lock *lock = new read_write_lock();
        lock->create(&name, manager->get_lock_table());
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

        CHECK_AND_FREE(manager);
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
