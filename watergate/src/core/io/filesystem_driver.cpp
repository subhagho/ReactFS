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
// Created by Subhabrata Ghosh on 10/10/16.
//

#include "watergate/includes/filesystem_driver.h"

const string com::wookler::watergate::core::io::fs_driver_constants::CONFIG_PARAM_ROOT_PATH = "fs.path.root";
const string com::wookler::watergate::core::io::fs_driver_constants::CONFIG_PARAM_QUOTA_BYTES = "fs.quota.bytes";
const string com::wookler::watergate::core::io::fs_driver_constants::CONFIG_PARAM_QUOTA_LEASE_TIME = "fs.quota.lease.time";
const string com::wookler::watergate::core::io::fs_driver_constants::CONFIG_PARAM_MAX_CONCURRENCY = "fs.concurrency.max";
const string com::wookler::watergate::core::io::fs_driver_constants::FS_RESOURCE_CLASS = "com::wookler::watergate::core::io::filesystem_driver";

void com::wookler::watergate::core::io::filesystem_driver::setup() {

    const ParamConfigValue *params = Config::get_params(config);
    if (IS_NULL(params)) {
        throw CONFIG_ERROR("Cannot find configuration parameters. [node=%s]", config->get_key().c_str());
    }

    string rp = params->get_string(fs_driver_constants::CONFIG_PARAM_ROOT_PATH);
    if (IS_EMPTY(rp)) {
        throw CONFIG_ERROR("Missing filesystem configuration parameter. [param=%s]",
                           fs_driver_constants::CONFIG_PARAM_ROOT_PATH.c_str());
    }
    root_path = new Path(rp);

    string qb = params->get_string(fs_driver_constants::CONFIG_PARAM_QUOTA_BYTES);
    if (IS_EMPTY(qb)) {
        throw CONFIG_ERROR("Missing filesystem configuration parameter. [param=%s]",
                           fs_driver_constants::CONFIG_PARAM_QUOTA_BYTES.c_str());
    }
    size_t size = common_utils::parse_size(qb);
    if (size <= 0) {
        throw CONFIG_ERROR("Invalid quota specified. [quota=%s][parsed=%d]", qb.c_str(), size);
    }
    this->resource_quota = size;

    string lt = params->get_string(fs_driver_constants::CONFIG_PARAM_QUOTA_LEASE_TIME);
    if (IS_EMPTY(lt)) {
        throw CONFIG_ERROR("Missing filesystem configuration parameter. [param=%s]",
                           fs_driver_constants::CONFIG_PARAM_QUOTA_LEASE_TIME.c_str());
    }
    long d = common_utils::parse_duration(lt);
    if (d <= 0) {
        throw CONFIG_ERROR("Invalid quota lease time specified. [lease time=%s][parsed=%d]", lt.c_str(), d);
    }
    this->lease_time = d;

    string mc = params->get_string(fs_driver_constants::CONFIG_PARAM_MAX_CONCURRENCY);
    if (IS_EMPTY(mc)) {
        throw CONFIG_ERROR("Missing filesystem configuration parameter. [param=%s]",
                           fs_driver_constants::CONFIG_PARAM_MAX_CONCURRENCY.c_str());
    }
    this->concurrency = atoi(mc.c_str());
    if (this->concurrency <= 0) {
        throw CONFIG_ERROR("Invalid max concurrency specified. [concurrency=%s]", mc.c_str());
    }

}

const string *com::wookler::watergate::core::io::filesystem_driver::get_resource_name() {
    return root_path->get_path_p();
}