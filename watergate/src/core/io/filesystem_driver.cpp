//
// Created by Subhabrata Ghosh on 10/10/16.
//

#include "includes/core/filesystem_driver.h"

const string com::watergate::core::io::fs_driver_constants::CONFIG_PARAM_ROOT_PATH = "fs.path.root";
const string com::watergate::core::io::fs_driver_constants::CONFIG_PARAM_QUOTA_BYTES = "fs.quota.bytes";
const string com::watergate::core::io::fs_driver_constants::CONFIG_PARAM_QUOTA_LEASE_TIME = "fs.quota.lease.time";
const string com::watergate::core::io::fs_driver_constants::CONFIG_PARAM_MAX_CONCURRENCY = "fs.concurrency.max";

void com::watergate::core::io::filesystem_driver::init(const ConfigValue *config) {
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

const string *com::watergate::core::io::filesystem_driver::get_resource_name() {
    return root_path->get_path_p();
}