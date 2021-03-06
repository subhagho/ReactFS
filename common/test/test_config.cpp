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
// Created by Subhabrata Ghosh on 31/08/16.
//

#include "test_config.h"

using namespace com::wookler::reactfs::common;


TEST_CASE("Create environment and load configuration", "[com::watergate::common::_env]") {
    string cfgFile = file_utils::cannonical_path("test/data/test-conf.json");
    env_utils::create_env(cfgFile);
    const __env *env = env_utils::get_env();
    REQUIRE(NOT_NULL(env));

    const Config *config = env_utils::get_config();
    REQUIRE(NOT_NULL(config));

    config->print();

    env_utils::dispose();

    SECTION("Test environment reload") {
        env_utils::create_env(cfgFile);
        const __env *env0 = env_utils::get_env();
        REQUIRE(NOT_NULL(env0));

        env_utils::dispose();
    }
}

TEST_CASE("Configuration Lookup Test : 1", "[com::watergate::common::Config]") {
    string cfgFile = file_utils::cannonical_path("test/data/test-conf.json");
    env_utils::create_env(cfgFile);
    const __env *env = env_utils::get_env();
    REQUIRE(NOT_NULL(env));

    const Config *config = env_utils::get_config();
    REQUIRE(NOT_NULL(config));


    const ConfigValue *c = config->find("/configuration/adhoc-list");
    REQUIRE(NOT_NULL(c));
    c->print("");

    const ConfigValue *e = c->find("/configuration/env");
    REQUIRE(NOT_NULL(e));
    const ParamConfigValue *pv = Config::get_params(e);
    REQUIRE(NOT_NULL(pv));

    const string value = config->get_value("/configuration/nested/params[key1]");
    REQUIRE(!IS_EMPTY(value));
    LOG_DEBUG("STRING [key1=%s]", value.c_str());

    const int iv = config->get_int_value("/configuration/nested/params[key2]", 9999);
    REQUIRE(iv != 9999);
    LOG_DEBUG("INT [key2=%d]", iv);

    const double dv = config->get_double_value("/configuration/nested/params[key3]", 9999.99);
    REQUIRE(dv != 9999.99);
    LOG_DEBUG("DOUBLE [key3=%f]", dv);

    const bool bv = config->get_bool_value("/configuration/nested/params[key4]", false);
    REQUIRE(bv);
    LOG_DEBUG("BOOL [key4=%d]", bv);

    const string dtv = config->get_value("/configuration/nested/params[key5]");
    REQUIRE(!IS_EMPTY(dtv));
    LOG_DEBUG("STRING [key5=%s]", dtv.c_str());

    const time_t tv = config->get_time_value("/configuration/nested/params[key5]",
                                             common_consts::DEFAULT_DATE_FORMAT,
                                             0);
    REQUIRE(tv > 0);
    string *ts = common_utils::format_time(tv, common_consts::DEFAULT_DATE_FORMAT);
    REQUIRE(NOT_NULL(ts));
    LOG_DEBUG("TIME [key5=%s]", ts->c_str());
    CHECK_AND_FREE(ts);

    env_utils::dispose();
}

TEST_CASE("Configuration Lookup Test : 2", "[com::watergate::common::Config]") {
    string cfgFile = file_utils::cannonical_path("test/data/test-conf.json");
    env_utils::create_env(cfgFile);
    const __env *env = env_utils::get_env();
    REQUIRE(NOT_NULL(env));

    const Config *config = env_utils::get_config();
    REQUIRE(NOT_NULL(config));

    const ConfigValue *c = config->find("/configuration/adhoc-list");
    REQUIRE(NOT_NULL(c));

    const ConfigValue *cv = c->find(".[2]/nested/params[key2]");
    assert (NOT_NULL(cv));
    cv->print("");

    CHECK_THROWS(config->find("/this/is/an/error/path", true));
    env_utils::dispose();
}


