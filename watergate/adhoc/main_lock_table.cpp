//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>
#include "common/includes/__env.h"
#include "watergate/includes/lock_table.h"
#include "watergate/includes/init_utils.h"

#define CONFIG_FILE getenv("CONFIG_FILE_PATH")

#include "watergate/src/core/control/dummy_resource.h"

using namespace com::wookler::watergate::core;
using namespace com::wookler::reactfs::common;

#define REQUIRE ASSERT
#define LOCK_TABLE_NAME "Test lock-table"
#define LOCK_TABLE_SIZE 4096

int main(int argc, char *argv[]) {
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


        uint16_t table_size = t->get_table_size();
        lock_table_client *records[table_size];

        int count = 0;
        for (uint16_t ii = 0; ii < table_size; ii++) {
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

        for (uint16_t ii = 0; ii < table_size; ii++) {
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