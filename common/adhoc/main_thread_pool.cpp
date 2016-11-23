//
// Created by Subhabrata Ghosh on 27/10/16.
//

#include "common/includes/init_utils.h"
#include "common/includes/log_utils.h"
#include "common/includes/timer.h"
#include "common/includes/__threads.h"

#define CONFIG_FILE getenv("CONFIG_FILE_PATH")
#define RUNTIME_DELTA 1000
#define CALLBACK_COUNT 5

using namespace com::wookler::reactfs::common;

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

int main(int argc, char **argv) {
    try {

        PRECONDITION(argc > 1);
        char *cf = argv[1];
        CHECK_NOT_NULL(cf);

        string configf(cf);
        env_utils::create_env(configf);

        const __env *env = env_utils::get_env();
        CHECK_NOT_NULL(env);

        {
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
        }
        {
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
        }
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