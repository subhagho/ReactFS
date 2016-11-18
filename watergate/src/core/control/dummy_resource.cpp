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
// Created by Subhabrata Ghosh on 15/09/16.
//

#include "common/includes/lock_record_def.h"
#include "dummy_resource.h"
#include "watergate/includes/control.h"

const string com::wookler::watergate::core::dummy_resource::DUMMY_RESOURCE_CLASS = "com::wookler::watergate::core::dummy_resource";

void com::wookler::watergate::core::dummy_resource::setup() {
    const BasicConfigValue *nn = Config::get_value(CONFIG_NODE_RESOURCE_NAME, config);
    if (IS_NULL(nn)) {
        throw ERROR_MISSING_CONFIG_NODE(CONFIG_NODE_RESOURCE_NAME);
    }
    const string name = nn->get_value();
    if (IS_EMPTY(name)) {
        throw CONFIG_ERROR("NULL/Empty string value. [name=%s]", CONFIG_NODE_RESOURCE_NAME);
    }
    this->name = new string(name);

    const BasicConfigValue *sn = Config::get_value(CONST_DR_CONFIG_PARAM_SIZE, config);
    if (NOT_NULL(sn)) {
        int s = sn->get_int_value(-1);
        if (s > 0) {
            this->size = s;
        }
    }

    this->lease_time = 5000;
    this->resource_quota = 1000;
}

int com::wookler::watergate::core::dummy_resource::get_control_size() {
    return this->size;
}

const string *com::wookler::watergate::core::dummy_resource::get_resource_name() {
    return this->name;
}
