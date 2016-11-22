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

#include "node_env.h"

#define MAX_MOUNT_POINTS 32
#define MOUNTS_KEY "env-mount-points"
#define MOUNTS_CONFIG_NODE "mount-points"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                typedef enum __mount_state__ {
                    MP_READ_WRITE = 0, MP_READ_ONLY, MP_UNAVAILABLE, MP_CORRUPTED
                } __mount_state;

                typedef struct __mount_point__ {
                    char path[SIZE_MAX_PATH];
                    __mount_state state = __mount_state::MP_UNAVAILABLE;
                    uint32_t total_blocks = 0;
                    uint64_t total_bytes_written = 0;
                    uint64_t total_bytes_read = 0;
                    uint64_t bytes_written = 0;
                    uint64_t bytes_read = 0;
                    uint64_t time_write = 0;
                    uint64_t time_read = 0;
                } __mount_point;

                typedef struct __mount_data__ {
                    uint16_t mount_count = 0;
                    uint16_t available_count = 0;
                    __mount_point mounts[MAX_MOUNT_POINTS];
                } __mount_data;

                class mount_manager : public __env_loader {
                private:
                    __mount_data *mounts = nullptr;
                    __env_record *record = nullptr;

                    void get_mount_points(const ConfigValue *node, vector<string> *v) {
                        if (node->get_type() == ConfigValueTypeEnum::Basic) {
                            const BasicConfigValue *b = dynamic_cast<const BasicConfigValue *>(node);
                            const string m = b->get_value();
                            v->push_back(string(m));
                        } else if (node->get_type() == ConfigValueTypeEnum::List) {
                            const ListConfigValue *list = dynamic_cast<const ListConfigValue *>(node);
                            const vector<ConfigValue *> values = list->get_values();
                            if (!IS_EMPTY(values)) {
                                for (ConfigValue *value : values) {
                                    if (value->get_type() == ConfigValueTypeEnum::Basic) {
                                        const BasicConfigValue *b = dynamic_cast<const BasicConfigValue *>(value);
                                        const string m = b->get_value();
                                        v->push_back(string(m));
                                    }
                                }
                            }
                        }
                    }

                public:
                    mount_manager() : __env_loader(new string(MOUNTS_KEY)) {
                        this->config_path = new string(MOUNTS_CONFIG_NODE);
                    }

                    ~mount_manager() {
                        if (NOT_NULL(record)) {
                            FREE_PTR(record->data);
                            CHECK_AND_FREE(record);
                        }
                    }

                    __env_record *load(const ConfigValue *config) override {
                        vector<string> m_points;
                        get_mount_points(config, &m_points);
                        POSTCONDITION(!IS_EMPTY(m_points));

                        __node_env *n_env = node_utils::get_node_env();
                        shared_mapped_ptr ptr = n_env->get_env_data(MOUNTS_KEY);
                        if (IS_NULL(ptr)) {
                            __mount_data *mm = (__mount_data *) malloc(sizeof(__mount_data));
                            CHECK_NOT_NULL(mm);
                            memset(mm, 0, sizeof(__mount_data));
                            mm->mount_count = m_points.size();
                            mm->available_count = m_points.size();
                            for (uint32_t ii = 0; ii < m_points.size(); ii++) {
                                string path = m_points[ii];
                                PRECONDITION(!IS_EMPTY(path));
                                strncpy(mm->mounts[ii].path, path.c_str(), path.length());
                                mm->mounts[ii].state = __mount_state::MP_READ_WRITE;
                                mm->mounts[ii].bytes_read = 0;
                                mm->mounts[ii].bytes_written = 0;
                                mm->mounts[ii].time_read = 0;
                                mm->mounts[ii].time_write = 0;
                                mm->mounts[ii].total_blocks = 0;
                                mm->mounts[ii].total_bytes_read = 0;
                                mm->mounts[ii].total_bytes_written = 0;
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
                                string path = m_points[ii];
                                PRECONDITION(!IS_EMPTY(path));
                                for (uint32_t jj = 0; jj < mounts->mount_count; jj++) {
                                    if (strlen(mounts->mounts[jj].path) == path.length() &&
                                        strncmp(mounts->mounts[jj].path, path.c_str(), path.length()) == 0) {
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
                                    string path = m_points[ii];
                                    uint32_t offset = mounts->mount_count++;
                                    __mount_point mp = mounts->mounts[offset];
                                    memset(&mp, 0, sizeof(__mount_point));
                                    strncpy(mp.path, path.c_str(), path.length());
                                    mp.state = __mount_state::MP_READ_WRITE;
                                }
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
                    }
                };
            }
        }
    }
}

#endif //REACTFS_MOUNT_MANAGER_H
