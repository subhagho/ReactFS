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

#include <unordered_map>

#include "core/includes/mount_structs.h"
#include "node_server_env.h"

using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                namespace server {
                    class mount_manager : public __env_loader {
                    private:
                        bool reset = false;

                        __mount_data *mounts = nullptr;
                        __env_record *record = nullptr;
                        __mount_map *mount_map = nullptr;

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
                                        if (value->get_type() == ConfigValueTypeEnum::Basic) {
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
                                limit = stol(value);
                            }
                            __mount_def *md = (__mount_def *) malloc(sizeof(__mount_def));
                            CHECK_NOT_NULL(md);
                            md->path = new string(name);
                            md->usage_limit = limit;

                            Path p(*(md->path));
                            POSTCONDITION(p.exists());

                            return md;
                        }


                    public:
                        mount_manager(bool reset = false) : __env_loader(new string(MOUNTS_KEY)) {
                            this->config_path = new string(MOUNTS_CONFIG_NODE);
                            this->reset = reset;
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

                            node_server_env *n_env = node_init_manager::get_server_env();
                            shared_mapped_ptr ptr = n_env->get_env_data(MOUNTS_KEY);
                            if (IS_NULL(ptr)) {
                                __mount_data *mm = (__mount_data *) malloc(sizeof(__mount_data));
                                CHECK_NOT_NULL(mm);
                                memset(mm, 0, sizeof(__mount_data));
                                mm->mount_count = m_points.size();
                                mm->available_count = m_points.size();
                                for (uint32_t ii = 0; ii < m_points.size(); ii++) {
                                    __mount_def *md = m_points[ii];
                                    PRECONDITION(!IS_EMPTY_P(md->path));
                                    strncpy(mm->mounts[ii].path, md->path->c_str(), md->path->length());
                                    mm->mounts[ii].state = __mount_state::MP_READ_WRITE;
                                    mm->mounts[ii].bytes_read = 0;
                                    mm->mounts[ii].bytes_written = 0;
                                    mm->mounts[ii].time_read = 0;
                                    mm->mounts[ii].time_write = 0;
                                    mm->mounts[ii].total_blocks = 0;
                                    mm->mounts[ii].total_bytes_read = 0;
                                    mm->mounts[ii].total_bytes_written = 0;
                                    mm->mounts[ii].usage_limit = md->usage_limit;
                                }
                                record = (__env_record *) malloc(sizeof(__env_record));
                                CHECK_NOT_NULL(record);

                                memset(record, 0, sizeof(__env_record));
                                strncpy(record->key, this->key->c_str(), this->key->length());
                                record->size = sizeof(__mount_data);
                                record->data = malloc(sizeof(BYTE) * record->size);
                                CHECK_NOT_NULL(record->data);
                                memcpy(record->data, mm, record->size);

                                FREE_PTR(mm);
                                for (auto mp : m_points) {
                                    CHECK_AND_FREE(mp->path);
                                    FREE_PTR(mp);
                                }
                                return record;
                            } else {
                                PRECONDITION(ptr.get()->get_size() == sizeof(__mount_data));
                                void *p = ptr.get()->get_data_ptr();
                                CHECK_NOT_NULL(p);

                                mounts = static_cast<__mount_data *>(p);
                                CHECK_NOT_NULL(mounts);

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
                                            indexes[ii] = jj;
                                            found = true;
                                            break;
                                        }
                                    }
                                    if (!found) {
                                        indexes[ii] = -1;
                                    }
                                }
                                for (uint32_t ii = 0; ii < indexes.size(); ii++) {
                                    int index = indexes[ii];
                                    if (index < 0) {
                                        __mount_def *md = m_points[ii];
                                        uint32_t offset = mounts->mount_count++;
                                        __mount_point mp = mounts->mounts[offset];
                                        memset(&mp, 0, sizeof(__mount_point));
                                        strncpy(mp.path, md->path->c_str(), md->path->length());
                                        mp.state = __mount_state::MP_READ_WRITE;
                                        mp.usage_limit = md->usage_limit;
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
                                            indexes[jj] = ii;
                                            found = true;
                                            break;
                                        }
                                    }
                                    if (!found) {
                                        indexes[jj] = -1;
                                    }
                                }
                                for (uint32_t ii = 0; ii < indexes.size(); ii++) {
                                    int index = indexes[ii];
                                    if (index < 0) {
                                        __mount_point mp = mounts->mounts[ii];
                                        bool dir_available = false;
                                        Path p(string(mp.path));
                                        if (p.exists()) {
                                            dir_available = true;
                                        }
                                        mp.usage_limit = 0;
                                        if (dir_available)
                                            mp.state = __mount_state::MP_READ_ONLY;
                                        else
                                            mp.state = __mount_state::MP_UNAVAILABLE;
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
                            CHECK_NOT_NULL(mounts);

                            mount_map = new __mount_map();

                            for (uint16_t ii = 0; ii < mounts->mount_count; ii++) {
                                __mount_point mp = mounts->mounts[ii];
                                if (mp.state == __mount_state::MP_UNAVAILABLE) {
                                    continue;
                                }
                                string name_l = mount_utils::get_lock_name(&mp, ii);
                                POSTCONDITION(!IS_EMPTY(name_l));
                                memset(mp.lock_name, 0, SIZE_LOCK_NAME);
                                strncpy(mp.lock_name, name_l.c_str(), name_l.length());

                                exclusive_lock *lock = new exclusive_lock(&name_l);
                                CHECK_NOT_NULL(lock);
                                if (reset) {
                                    lock->reset();
                                }

                                mount_map->add_mount_lock(lock->get_name(), lock);
                                mount_map->add_mount_index(string(mp.path), ii);
                            }
                            if (NOT_NULL(this->record)) {
                                FREE_PTR(this->record->data);
                                CHECK_AND_FREE(this->record);
                            }
                        }


                    };
                }
            }
        }
    }
}

#endif //REACTFS_MOUNT_MANAGER_H
