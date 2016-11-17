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
// Created by Subhabrata Ghosh on 22/09/16.
//

#include "test_lock_table.h"
#include "watergate/src/core/control/dummy_resource.h"

using namespace com::wookler::watergate::core;
using namespace com::wookler::reactfs::common;


#define LOCK_TABLE_NAME "TEST_LOCK_TABLE"
#define LOCK_TABLE_SIZE 4096

TEST_CASE("Test SHM based lock table.", "[com::watergate::core::lock_table]") {
    try {
        init_utils::create_env(CONFIG_FILE);
        const __env *env = init_utils::get_env();
        REQUIRE(NOT_NULL(env));

        const Config *config = init_utils::get_config();
        REQUIRE(NOT_NULL(config));

        config->print();

        dummy_resource *resource = new dummy_resource();

        lock_table_manager *t = new lock_table_manager();
        t->init(LOCK_TABLE_NAME, resource, LOCK_TABLE_SIZE, true);


        uint32_t table_size = t->get_table_size();
        lock_table_client *records[table_size];

        int count = 0;
        for (int ii = 0; ii < table_size; ii++) {
            string app_name("LOCK_TABLE_TEST_");
            app_name.append(to_string(ii));
            pid_t pid = 1000 + ii;
            string app_id = common_utils::uuid();

            __app app(app_name);
            lock_table_client *c = new lock_table_client();
            c->init(&app, LOCK_TABLE_NAME, resource, table_size);

            POSTCONDITION(c->get_state().get_state() == __state_enum::Available);

            records[ii] = c;
            if (ii % 100 == 0) {
                LOG_DEBUG("Added [%d] records...", ii);
            }
            count++;
        }


        LOG_DEBUG("Created [%d] lock records.", count);

        for (int ii = 0; ii < table_size; ii++) {
            lock_table_client *c = records[ii];
            CHECK_NOT_NULL(c);
            CHECK_AND_FREE(c);
        }
        CHECK_AND_FREE(t);
        CHECK_AND_FREE(resource);

        init_utils::dispose();
    } catch (const exception &e) {
        LOG_ERROR(e.what());
        REQUIRE(false);
    }
}
