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


#define LOCK_TABLE_NAME "Test lock-table"
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
        t->init(LOCK_TABLE_NAME, resource);

        lock_table_client *c = new lock_table_client();
        c->init(env->get_app(), LOCK_TABLE_NAME, resource, LOCK_TABLE_SIZE);

        __lock_record *records[DEFAULT_MAX_RECORDS - 1];

        int count = 0;
        for (int ii = 0; ii < DEFAULT_MAX_RECORDS - 1; ii++) {
            string app_name("LOCK_TABLE_TEST_");
            app_name.append(to_string(ii));
            pid_t pid = 1000 + ii;
            string app_id = common_utils::uuid();

            __lock_record *rec = c->new_record_test(app_name, app_id, pid);
            if (IS_NULL(rec)) {
                LOG_ERROR("Null record returned for index [%d]", ii);
            } else {
                LOG_INFO("Record created. [index=%d]", ii);
            }
            REQUIRE(NOT_NULL(rec));

            records[ii] = rec;
            if (ii % 100 == 0) {
                LOG_DEBUG("Added [%d] records...", ii);
            }
            count++;
        }

        LOG_DEBUG("Created [%d] lock records.", count);

        count = 0;
        for (int ii = 0; ii < DEFAULT_MAX_RECORDS - 1; ii++) {
            __lock_record *rec = records[ii];

            if (ii % 100 == 0) {
                LOG_DEBUG("[index=%d][application name:%s id:%s pid:%d] Registered at [%lu]", ii, rec->app.app_name,
                          rec->app.app_id, rec->app.proc_id,
                          rec->app.register_time);
            }
            c->remove_record(rec);
            count++;
        }
        LOG_DEBUG("Released [%d] lock records.", count);

        CHECK_AND_FREE(c);
        CHECK_AND_FREE(t);
        CHECK_AND_FREE(resource);

        init_utils::dispose();
    } catch (const exception &e) {
        LOG_ERROR(e.what());
        REQUIRE(false);
    }
}
