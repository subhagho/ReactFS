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
#include <unistd.h>
#include <iostream>
#include <thread>
#include "main_fs.h"

using namespace com::wookler::watergate::core;

#define REQUIRE _assert
#define METRIC_LOCK_TIME "lock.acquire.wait.time"

int main(int argc, char *argv[]) {

    try {

        init_utils::create_env(getenv("CONFIG_FILE_PATH"));
        const __env *env = init_utils::get_env();
        REQUIRE(NOT_NULL(env));

        const Config *config = init_utils::get_config();
        REQUIRE(NOT_NULL(config));

        config->print();

        control_manager *manager = init_utils::init_control_manager(env, CONTROL_CONFIG_PATH);
        REQUIRE(NOT_NULL(manager));

        LOG_DEBUG("Creating Control Client...");
        const control_client *control = init_utils::init_control_client(env, CONTROL_DEF_CONFIG_PATH);
        REQUIRE(NOT_NULL(control));

        thread_lock_ptr *tptr = control->register_thread(CONTROL_NAME);
        if (IS_NULL(tptr)) {
            throw BASE_ERROR("Error registering thread. [name=%s]", CONTROL_NAME);
        }
        string tid = tptr->thread_id;

        const ConfigValue *p_config = config->find(TFW_CONFIG_NODE);
        REQUIRE(NOT_NULL(p_config));

        const BasicConfigValue *pv = Config::get_value(TFW_CONFIG_VALUE_DATAF, p_config);
        REQUIRE(NOT_NULL(pv));
        const string filename = pv->get_value();

        string data = read_data_file(filename);

        string runid = common_utils::uuid();
        Path *dir = env->get_temp_dir(runid, 0760);
        REQUIRE(NOT_NULL(dir));
        dir->append("output-basic.txt");
        LOG_INFO("Writing output to file. [file=%s]", dir->get_path().c_str());

        fs_writer *writer = new fs_writer(dir->get_path(), 1);
        writer->open("w");
        writer->write(&data, data.length(), 1);

        CHECK_AND_FREE(writer);
        CHECK_AND_FREE(manager);

        init_utils::dispose();

        exit(0);

    } catch (const exception &e) {
        cout << "ERROR : " << e.what() << "\n";
        exit(-1);
    } catch (...) {
        cout << "Unknown error...\n";
        exit(-1);
    }
}
