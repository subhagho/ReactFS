//
// Created by Subhabrata Ghosh on 22/11/16.
//


/*
 * Copyright [2016] [Subhabrata Ghosh]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef REACTFS_MOUNT_MANAGER_H
#define REACTFS_MOUNT_MANAGER_H

#include <sys/statvfs.h>
#include <unordered_map>

#include "common/includes/metrics.h"
#include "common/includes/log_utils.h"
#include "common/includes/__threads.h"
#include "core/includes/mount_structs.h"
#include "node_server_env.h"

#define MOUNT_METRICS_LOGGER_NAME "mount-metrics-logger"
#define MOUNT_METRICS_LOG_FREQUENCY 1000 * 60 * 3
#define MOUNT_METRIC_NAME_R "mount-metrics-read"
#define MOUNT_METRIC_NAME_W "mount-metrics-write"

using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                namespace server {

                    class mount_metrics_logger : public __runnable_callback {
                    private:
                        __mount_data *mounts = nullptr;
                        __avg_metric *read_metric = nullptr;
                        __avg_metric *write_metric = nullptr;

                    public:
                        mount_metrics_logger(__mount_data *mounts) : __runnable_callback(MOUNT_METRICS_LOGGER_NAME) {
                            CHECK_NOT_NULL(mounts);
                            this->mounts = mounts;
                            this->read_metric = new __avg_metric(MOUNT_METRIC_NAME_R, false);
                            CHECK_ALLOC(this->read_metric, TYPE_NAME(__avg_metric));
                            this->write_metric = new __avg_metric(MOUNT_METRIC_NAME_W, false);
                            CHECK_ALLOC(this->write_metric, TYPE_NAME(__avg_metric));
                        }


                        bool can_run() {
                            uint64_t now = time_utils::now();
                            return ((now - last_run_time) >= MOUNT_METRICS_LOG_FREQUENCY);
                        }


                        void callback() {
                            try {
                                for (uint16_t ii = 0; ii < mounts->mount_count; ii++) {
                                    __mount_point *mp = &mounts->mounts[ii];
                                    CHECK_NOT_NULL(mp);
                                    if (mp->state == __mount_state::MP_READ_WRITE) {
                                        mount_metrics::get_hourly_metric(this->write_metric, &mp->bytes_written);
                                        this->write_metric->print();
                                        mount_metrics::get_hourly_metric(this->read_metric, &mp->bytes_read);
                                        this->read_metric->print();
                                    } else if (mp->state == __mount_state::MP_READ_ONLY) {
                                        mount_metrics::get_hourly_metric(this->read_metric, &mp->bytes_read);
                                        this->read_metric->print();
                                    }
                                }
                            } catch (const exception &e) {
                                LOG_CRITICAL("Error while running [%s][error=%s]", MOUNT_METRICS_LOGGER_NAME, e.what());
                            }
                        }

                        void error() {
                            // Do nothing...
                        }

                        void error(exception *err) {
                            LOG_CRITICAL("Error running [%s][error=%s]", MOUNT_METRICS_LOGGER_NAME, err->what());
                        }
                    };

                    class mount_manager : public __env_loader {
                    private:
                        bool reset = false;

                        __mount_data *mounts = nullptr;
                        __env_record *record = nullptr;
                        __mount_map *mount_map = nullptr;
                        node_server_env *n_env = nullptr;

                        void get_mount_points(const ConfigValue *node, vector<__mount_def *> *v) {
                            if (node->get_type() == ConfigValueTypeEnum::Node) {
                                __mount_def *md = get_mout_def(node);
                                CHECK_NOT_NULL(md);
                                v->push_back(md);
                            } else if (node->get_type() == ConfigValueTypeEnum::List) {
                                const ListConfigValue *list = dynamic_cast<const ListConfigValue *>(node);
                                const vector<ConfigValue *> values = list->get_values();
                                if (!IS_EMPTY(values)) {
                                    for (ConfigValue *value : values) {
                                        if (value->get_type() == ConfigValueTypeEnum::Node) {
                                            __mount_def *md = get_mout_def(value);
                                            v->push_back(md);
                                        }
                                    }
                                }
                            }
                        }

                        __mount_def *get_mout_def(const ConfigValue *node) {
                            PRECONDITION(node->get_type() == ConfigValueTypeEnum::Node);
                            const ConfigValue *nn = node->find(MOUNTS_CONFIG_PATH);
                            POSTCONDITION(NOT_NULL(nn));
                            POSTCONDITION(nn->get_type() == ConfigValueTypeEnum::Basic);
                            const BasicConfigValue *n_node = dynamic_cast<const BasicConfigValue *>(nn);
                            string name = n_node->get_value();
                            POSTCONDITION(!IS_EMPTY(name));
                            nn = node->find(MOUNTS_CONFIG_LIMIT);
                            long limit = -1;
                            if (NOT_NULL(nn)) {
                                PRECONDITION(nn->get_type() == ConfigValueTypeEnum::Basic);
                                const BasicConfigValue *v_node = dynamic_cast<const BasicConfigValue *>(nn);
                                string value = v_node->get_value();
                                POSTCONDITION(!IS_EMPTY(value));
                                limit = common_utils::parse_size(value);
                            }
                            __mount_state ms = __mount_state::MP_READ_WRITE;
                            nn = node->find(MOUNTS_CONFIG_STATE);
                            if (NOT_NULL(nn)) {
                                PRECONDITION(nn->get_type() == ConfigValueTypeEnum::Basic);
                                const BasicConfigValue *v_node = dynamic_cast<const BasicConfigValue *>(nn);
                                string value = v_node->get_value();
                                POSTCONDITION(!IS_EMPTY(value));
                                ms = mount_utils::parse_state(value);
                            }
                            __mount_def *md = (__mount_def *) malloc(sizeof(__mount_def));
                            CHECK_ALLOC(md, TYPE_NAME(__mount_def));
                            md->path = new string(name);
                            CHECK_ALLOC(md->path, TYPE_NAME(string));
                            md->usage_limit = limit;
                            md->state = ms;

                            Path p(*(md->path));
                            POSTCONDITION(p.exists());

                            return md;
                        }


                    public:
                        mount_manager(node_server_env *n_env, bool reset = false) : __env_loader(
                                new string(MOUNTS_KEY)) {
                            CHECK_NOT_NULL(n_env);
                            this->config_path = new string(MOUNTS_CONFIG_NODE);
                            CHECK_ALLOC(this->config_path, TYPE_NAME(string));
                            this->reset = reset;
                            this->n_env = n_env;
                        }

                        ~mount_manager() {
                            if (NOT_NULL(record)) {
                                FREE_PTR(record->data);
                                CHECK_AND_FREE(record);
                            }
                            CHECK_AND_FREE(mount_map);
                        }

                        __env_record *load(const ConfigValue *config) override {
                            vector<__mount_def *> m_points;
                            get_mount_points(config, &m_points);
                            POSTCONDITION(!IS_EMPTY(m_points));

                            shared_mapped_ptr *ptr = n_env->get_env_data(MOUNTS_KEY);
                            if (IS_NULL(ptr)) {
                                __mount_data *mm = (__mount_data *) malloc(sizeof(__mount_data));
                                CHECK_ALLOC(mm, TYPE_NAME(__mount_data));
                                memset(mm, 0, sizeof(__mount_data));
                                mm->mount_count = m_points.size();
                                mm->available_count = m_points.size();
                                for (uint32_t ii = 0; ii < m_points.size(); ii++) {
                                    __mount_def *md = m_points[ii];
                                    PRECONDITION(!IS_EMPTY_P(md->path));
                                    strncpy(mm->mounts[ii].path, md->path->c_str(), md->path->length());
                                    mm->mounts[ii].state = __mount_state::MP_READ_WRITE;
                                    mm->mounts[ii].total_blocks = 0;
                                    mm->mounts[ii].total_bytes_read = 0;
                                    mm->mounts[ii].total_bytes_written = 0;
                                    mm->mounts[ii].usage_limit = md->usage_limit;
                                    mount_metrics::reset_hourly_metrics(&(mm->mounts[ii].bytes_read));
                                    mount_metrics::reset_hourly_metrics(&(mm->mounts[ii].bytes_written));
                                }
                                record = (__env_record *) malloc(sizeof(__env_record));
                                CHECK_ALLOC(record, TYPE_NAME(__env_record));

                                memset(record, 0, sizeof(__env_record));
                                strncpy(record->key, this->key->c_str(), this->key->length());
                                record->size = sizeof(__mount_data);
                                record->data = malloc(sizeof(BYTE) * record->size);
                                CHECK_ALLOC(record->data, TYPE_NAME(BYTE * ));
                                memcpy(record->data, mm, record->size);

                                FREE_PTR(mm);
                                for (auto mp : m_points) {
                                    CHECK_AND_FREE(mp->path);
                                    FREE_PTR(mp);
                                }
                                return record;
                            } else {
                                PRECONDITION(ptr->get()->get_size() == sizeof(__mount_data));
                                void *p = ptr->get()->get_data_ptr();
                                CHECK_NOT_NULL(p);

                                mounts = static_cast<__mount_data *>(p);
                                CHECK_CAST(mounts, "void *", TYPE_NAME(__mount_data));

                                mounts->available_count = m_points.size();
                                vector<int> indexes;
                                for (uint32_t ii = 0; ii < m_points.size(); ii++) {
                                    bool found = false;
                                    __mount_def *md = m_points[ii];
                                    PRECONDITION(!IS_EMPTY_P(md->path));
                                    for (uint32_t jj = 0; jj < mounts->mount_count; jj++) {
                                        if (strlen(mounts->mounts[jj].path) == md->path->length() &&
                                            strncmp(mounts->mounts[jj].path, md->path->c_str(), md->path->length()) ==
                                            0) {
                                            mounts->mounts[jj].usage_limit = md->usage_limit;
                                            mounts->mounts[jj].state = md->state;
                                            indexes.push_back((int) jj);
                                            found = true;
                                            break;
                                        }
                                    }
                                    if (!found) {
                                        indexes.push_back(-1);
                                    }
                                }
                                for (uint32_t ii = 0; ii < indexes.size(); ii++) {
                                    int index = indexes[ii];
                                    if (index < 0) {
                                        __mount_def *md = m_points[ii];
                                        uint32_t offset = mounts->mount_count++;
                                        __mount_point *mp = &mounts->mounts[offset];
                                        memset(&mp, 0, sizeof(__mount_point));
                                        memcpy(mp->path, md->path->c_str(), md->path->length());
                                        mp->state = md->state;
                                        mp->usage_limit = md->usage_limit;
                                    }
                                }
                                indexes.clear();
                                for (uint32_t jj = 0; jj < mounts->mount_count; jj++) {
                                    bool found = false;
                                    for (uint32_t ii = 0; ii < m_points.size(); ii++) {
                                        __mount_def *md = m_points[ii];
                                        if (strlen(mounts->mounts[jj].path) == md->path->length() &&
                                            strncmp(mounts->mounts[jj].path, md->path->c_str(), md->path->length()) ==
                                            0) {
                                            mounts->mounts[jj].usage_limit = md->usage_limit;
                                            mounts->mounts[jj].state = md->state;
                                            indexes.push_back((int) ii);
                                            found = true;
                                            break;
                                        }
                                    }
                                    if (!found) {
                                        indexes.push_back(-1);
                                    }
                                }
                                for (uint32_t ii = 0; ii < indexes.size(); ii++) {
                                    int index = indexes[ii];
                                    if (index < 0) {
                                        __mount_point *mp = &mounts->mounts[ii];
                                        bool dir_available = false;
                                        Path p(string(mp->path));
                                        if (p.exists()) {
                                            dir_available = true;
                                        }
                                        mp->usage_limit = 0;
                                        if (dir_available)
                                            mp->state = __mount_state::MP_READ_ONLY;
                                        else
                                            mp->state = __mount_state::MP_UNAVAILABLE;
                                    }
                                }
                                for (auto mp : m_points) {
                                    CHECK_AND_FREE(mp->path);
                                    FREE_PTR(mp);
                                }
                                return nullptr;
                            }
                        }

                        void load_finished(shared_mapped_ptr record) override {
                            CHECK_NOT_NULL(record);
                            PRECONDITION(record.get()->get_size() == sizeof(__mount_data));

                            void *ptr = record.get()->get_data_ptr();
                            CHECK_NOT_NULL(ptr);

                            mounts = static_cast<__mount_data *>(ptr);
                            CHECK_CAST(mounts, "void *", TYPE_NAME(__mount_data));

                            mount_map = new __mount_map();
                            CHECK_ALLOC(mount_map, TYPE_NAME(__mount_map));
                            for (uint16_t ii = 0; ii < mounts->mount_count; ii++) {
                                __mount_point *mp = &mounts->mounts[ii];
                                if (mp->state == __mount_state::MP_UNAVAILABLE) {
                                    continue;
                                }
                                string name_l = mount_utils::get_lock_name(mp, ii);
                                POSTCONDITION(!IS_EMPTY(name_l));
                                memset(mp->lock_name, 0, SIZE_LOCK_NAME);
                                memcpy(mp->lock_name, name_l.c_str(), name_l.length());

                                exclusive_lock *lock = nullptr;
                                CREATE_LOCK_P(lock, &name_l, DEFAULT_LOCK_MODE);
                                if (reset) {
                                    lock->reset();
                                }

                                if (mp->usage_limit < 0) {
                                    string p(mp->path);
                                    uint64_t avail = mount_utils::get_free_space(p);
                                    TRACE("[mount=%s] Available space = %lu", mp->path, avail);
                                    if (avail > 0)
                                        mp->usage_limit = avail;
                                }
                                TRACE("[mount=%s] Available space = %lu GB", mp->path,
                                      (mp->usage_limit / (1024 * 1024 * 1024)));
                                mount_map->add_mount_lock(lock->get_name(), lock);
                                mount_map->add_mount_index(string(mp->path), ii);
                            }
                            if (NOT_NULL(this->record)) {
                                FREE_PTR(this->record->data);
                                CHECK_AND_FREE(this->record);
                            }
                            mount_metrics_logger *m_logger = new mount_metrics_logger(this->mounts);
                            CHECK_ALLOC(m_logger, TYPE_NAME(mount_metrics_logger));
                            n_env->add_runnable(m_logger);
                        }

                        __mount_data *get_mounts() {
                            CHECK_NOT_NULL(this->mounts);
                            return this->mounts;
                        }
                    };

                }
            }
        }
    }
}

#endif //REACTFS_MOUNT_MANAGER_H
