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

#include "common/includes/__env.h"

com::watergate::common::__log *LOG = nullptr;

void com::watergate::common::__env::create(string filename, string app_name) {
    try {
        this->config = new Config();
        this->config->create(filename);

        const ConfigValue *e_node = this->config->find(CONST_CONFIG_ENV_PATH);
        if (NOT_NULL(e_node)) {
            const ParamConfigValue *e_params = Config::get_params(e_node);
            if (NOT_NULL(e_params)) {
                if (IS_EMPTY(app_name)) {
                    app_name = e_params->get_string(CONST_CONFIG_ENV_PARAM_APPNAME);
                    if (IS_EMPTY(app_name)) {
                        throw ERROR_MISSING_CONFIG(CONST_CONFIG_ENV_PARAM_APPNAME);
                    }
                }
                this->app = new __app(app_name);
                string appdir = this->app->get_app_directory();
                const string workdir = e_params->get_string(CONFIG_ENV_PARAM_WORKDIR);
                if (!IS_EMPTY(workdir)) {
                    this->work_dir = new Path(workdir);
                } else {
                    this->work_dir = new Path(CONST_DEFAULT_DIR);
                    if (!IS_EMPTY(appdir)) {
                        this->work_dir->append(appdir);
                    }
                    this->work_dir->append("work");
                }
                if (!this->work_dir->exists()) {
                    this->work_dir->create(0755);
                }

                const string tempdir = e_params->get_string(CONFIG_ENV_PARAM_TEMPDIR);
                if (!IS_EMPTY(tempdir)) {
                    this->temp_dir = new Path(tempdir);
                } else {
                    this->temp_dir = new Path(CONST_DEFAULT_DIR);
                    if (!IS_EMPTY(appdir)) {
                        this->temp_dir->append(appdir);
                    }
                    this->temp_dir->append("temp");
                }
                if (!this->temp_dir->exists()) {
                    this->temp_dir->create(0755);
                }

            } else {
                setup_defaults();
            }
            LOG = new __log();
            LOG->init(e_node, this->work_dir, this->app->get_name());
        } else {
            setup_defaults();
            LOG = new __log();
            LOG->init();
        }

        metrics_utils::init();

        LOG_INFO("Initialized environment. [config=%s]", filename.c_str());
        state.set_state(Available);
    } catch (const exception &e) {
        state.set_error(&e);
        LOG_ERROR("Error initializing environment. [config=%s]", filename.c_str());
        LOG_ERROR("Error [%s]", e.what());
    } catch (...) {
        state.set_error(new runtime_error("Unknown error occurred while creating environment"));
        LOG_ERROR("Error initializing environment. [config=%s]", filename.c_str());
        LOG_ERROR("Error [%s]", state.get_error()->what());
    }
}

const Path *com::watergate::common::__env::get_temp_dir() const {
    CHECK_STATE_AVAILABLE(this->state);

    return this->temp_dir;
}

Path *com::watergate::common::__env::get_temp_dir(string name, mode_t mode) const {
    CHECK_STATE_AVAILABLE(this->state);
    assert(!name.empty());

    Path *pp = new Path(temp_dir->get_path());
    pp->append(name);

    file_utils::create_directory(pp->get_path(), mode);

    return pp;
}

const Path *com::watergate::common::__env::get_work_dir() const {
    CHECK_STATE_AVAILABLE(this->state);

    return this->work_dir;
}

Path *com::watergate::common::__env::get_work_dir(string name, mode_t mode) const {
    CHECK_STATE_AVAILABLE(this->state);
    assert(!name.empty());

    Path *pp = new Path(work_dir->get_path());
    pp->append(name);

    file_utils::create_directory(pp->get_path(), mode);

    return pp;
}

com::watergate::common::__env::~__env() {
    this->state.set_state(Disposed);
    metrics_utils::dump();
    metrics_utils::dispose();

    LOG_DEBUG("[pid=%d] Releasing config handle...", getpid());
    CHECK_AND_FREE(this->config);
    LOG_DEBUG("[pid=%d] Releasing app handle...", getpid());
    CHECK_AND_FREE(this->app);
    CHECK_AND_FREE(temp_dir);
    CHECK_AND_FREE(work_dir);
    LOG_DEBUG("[pid=%d] Releasing LOG handle...", getpid());
    CHECK_AND_FREE(LOG);
}


