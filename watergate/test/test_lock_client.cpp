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
#include <sys/types.h>
#include <unistd.h>

#include "test_lock_client.h"
#include "common/includes/__alarm.h"
#include "common/includes/timer.h"

#define REQUIRE _assert

void com::wookler::watergate::tests::common::basic_lock_client::setup() {
    pid_t pid = getpid();

    const __env *env = init_utils::get_env();
    REQUIRE(NOT_NULL(env));

    const Config *config = init_utils::get_config();
    REQUIRE(NOT_NULL(config));

    LOG_DEBUG("[pid=%d] Creating Control Client...", pid);
    const control_client *control = init_utils::init_control_client(env, CONTROL_DEF_CONFIG_PATH);
    REQUIRE(NOT_NULL(control));

    CHECK_STATE_AVAILABLE(control->get_state());

    const ConfigValue *t_config = config->find(TLC_CONFIG_NODE);
    if (IS_NULL(t_config)) {
        throw CONFIG_ERROR("Test Client configuration not specified. [node=%s]", TLC_CONFIG_NODE);
    }

    const BasicConfigValue *sv = Config::get_value(TCL_CONFIG_VALUE_SLEEP, t_config);
    if (IS_NULL(sv)) {
        throw CONFIG_ERROR("Test Client configuration not specified. [node=%s]", TCL_CONFIG_VALUE_SLEEP);
    }
    sleep_timeout = sv->get_long_value(-1);
    if (sleep_timeout <= 0)
        throw CONFIG_ERROR("Invalid Test Client configuration. [node=%s][value=%d]", TCL_CONFIG_VALUE_SLEEP,
                           sleep_timeout);

    const BasicConfigValue *tv = Config::get_value(TCL_CONFIG_VALUE_TRIES, t_config);
    if (IS_NULL(tv)) {
        throw CONFIG_ERROR("Test Client configuration not specified. [node=%s]", TCL_CONFIG_VALUE_TRIES);
    }
    lock_tries = tv->get_int_value(-1);
    if (lock_tries <= 0)
        throw CONFIG_ERROR("Invalid Test Client configuration. [node=%s][value=%d]", TCL_CONFIG_VALUE_TRIES,
                           lock_tries);
    LOG_DEBUG("[pid=%d] Setup done...", pid);
}

void com::wookler::watergate::tests::common::basic_lock_client::run() {
    try {
        pid_t pid = getpid();
        LOG_INFO("[pid=%d] Running lock control client. ", pid);

        const control_client *control = init_utils::get_client();
        CHECK_NOT_NULL(control);

        thread_lock_ptr *tptr = control->register_thread(FS_CONTROL_NAME);
        if (IS_NULL(tptr)) {
            throw BASE_ERROR("Error registering thread. [name=%s]", FS_CONTROL_NAME);
        }
        string tid = tptr->thread_id;
        int count = 0;

        timer t;

        usleep((priority + 1) * 50 * 1000);
        for (int ii = 0; ii < 8; ii++) {
            count = 0;
            LOG_DEBUG("[pid=%d][thread=%s][name=%s][priority=%d] run sequence %d", pid, tid.c_str(),
                      FS_CONTROL_NAME, priority, ii);
            while (true) {
                LOG_DEBUG("[pid=%d][thread=%s][name=%s][priority=%d] Lock try count %d", pid, tid.c_str(),
                          FS_CONTROL_NAME, priority, count);
                int err = 0;
                t.restart();
                _lock_state r = control->lock(FS_CONTROL_NAME, priority, 200, 5000, &err);
                t.pause();
                LOG_DEBUG("[pid=%d][thread=%s][name=%s][priority=%d] Lock get count %d [response=%s]", pid, tid.c_str(),
                          FS_CONTROL_NAME, priority, count, record_utils::get_lock_acquire_enum_string(r).c_str());
                if (r == Locked && err == 0) {
                    LOG_INFO("Successfully acquired lock [pid=%d][thread=%s][name=%s][priority=%d][try=%d]", pid,
                             tid.c_str(),
                             FS_CONTROL_NAME, priority,
                             ii);
                    usleep((3 - priority) * 500 * 1000);
                    bool r = control->release(FS_CONTROL_NAME, priority);
                    if (r) {
                        LOG_INFO("Successfully released lock [pid=%d][thread=%s][name=%s][priority=%d][try=%d]", pid,
                                 tid.c_str(),
                                 FS_CONTROL_NAME, priority,
                                 ii);
                    }
                    break;
                } else if (err != 0) {
                    LOG_ERROR(
                            "Filed to acquired lock [pid=%d][thread=%s][name=%s][priority=%d][try=%d][response=%d][error=%d]",
                            pid, tid.c_str(),
                            FS_CONTROL_NAME, priority, ii, r, err);
                } else
                    LOG_ERROR("Filed to acquired lock [pid=%d][thread=%s][name=%s][priority=%d][try=%d][response=%d]",
                              pid, tid.c_str(),
                              FS_CONTROL_NAME, priority, ii, r);
                usleep(5 * 1000);
                count++;
            }
            START_ALARM(sleep_timeout * (priority + 1));
        }

        LOG_DEBUG("[pid=%d][priority=%d] Finished executing. [execution time=%lu]", pid, priority,
                  t.get_current_elapsed());

        control->dump();
        control->test_assert();
    } catch (const exception &e) {
        cout << "Run exited with error. [error=" << e.what() << "]\n";
        LOG_CRITICAL("Run exited with error. [error=%s]", e.what());
    } catch (...) {
        cout << "Run exited with error. [error=UNKOWN]\n";
        LOG_CRITICAL("Run exited with error. [error=UNKOWN]");
    }
}

int main(int argc, char *argv[]) {
    cout << "Starting lock client...\n";
    for (int ii = 0; ii < argc; ii++) {
        cout << "ARG[" << ii << "]=" << argv[ii] << "\n";
    }

    try {
        argc -= (argc > 0);
        argv += (argc > 0); // skip program name argv[0] if present
        option::Stats stats(usage, argc, argv);
        std::vector<option::Option> options(stats.options_max);
        std::vector<option::Option> buffer(stats.buffer_max);
        option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

        if (parse.error())
            return 1;

        if (options[HELP] || argc == 0) {
            option::printUsage(std::cout, usage);
            return 0;
        }

        option::Option o = options[CONFIG];
        option::Option i = options[INDEX];
        int index = -1;
        if (o && o.count() > 0 && i && i.count() > 0) {
            string configf = o.arg;
            index = atoi(i.arg);
            if (!IS_EMPTY(configf) && index >= 0) {
                string pname = common_utils::format("basic_lock_client.%d", index);
                init_utils::create_env(CONFIG_FILE, pname);
                const __env *env = init_utils::get_env();
                REQUIRE(NOT_NULL(env));

                const Config *config = init_utils::get_config();
                REQUIRE(NOT_NULL(config));

            } else
                throw CONFIG_ERROR("NULL/empty configuration file path specified.");
        } else {
            option::printUsage(std::cout, usage);
            return -1;
        }
        const __env *env = init_utils::get_env();

        int priority = -1;
        o = options[PRIORITY];
        if (o && o.count() > 0) {
            if (o.arg) {
                priority = atoi(o.arg);
            }
        } else {
            option::printUsage(std::cout, usage);
            return -1;
        }

        if (priority < 0)
            throw BASE_ERROR("Invalid priority specified. [priority=%d]", priority);

        com::wookler::watergate::tests::common::basic_lock_client *client =
                new com::wookler::watergate::tests::common::basic_lock_client(
                priority);
        client->setup();
        client->run();

        CHECK_AND_FREE(client);
        LOG_DEBUG("[pid=%d][index=%d] Released run client handle...", getpid(), index);
        LOG_DEBUG("[pid=%d][index=%d] Disposed runtime environment...", getpid(), index);

        LOG_DEBUG("[pid=%d][index=%d] Exiting run...", getpid(), index);
        init_utils::dispose();
        exit(0);
    } catch (const exception &e) {
        cout << "ERROR : " << e.what() << "\n";
        exit(-1);
    } catch (...) {
        cout << "Unkown exception.\n";
        exit(-1);
    }
}
