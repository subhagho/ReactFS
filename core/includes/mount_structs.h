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

//
// Created by Subhabrata Ghosh on 24/11/16.
//

#ifndef REACTFS_MOUNT_STRUCTS_H
#define REACTFS_MOUNT_STRUCTS_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/exclusive_lock.h"

#define MAX_MOUNT_POINTS 32

#define MOUNTS_KEY "env-mount-points"
#define MOUNTS_CONFIG_NODE "mount-points"
#define MOUNTS_CONFIG_PATH "path"
#define MOUNTS_CONFIG_LIMIT "usage-limit"

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                typedef enum __mount_state__ {
                    MP_READ_WRITE = 0, MP_READ_ONLY, MP_UNAVAILABLE, MP_CORRUPTED
                } __mount_state;

                typedef struct __mount_point__ {
                    char path[SIZE_MAX_PATH];
                    char lock_name[SIZE_LOCK_NAME];
                    __mount_state state = __mount_state::MP_UNAVAILABLE;
                    int64_t usage_limit = 0;
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

                typedef struct __mount_def__ {
                    string *path;
                    int64_t usage_limit;
                } __mount_def;

                class __mount_map {
                private:
                    unordered_map<string, exclusive_lock *> mount_locks;
                    unordered_map<string, uint16_t> mount_index;

                public:
                    ~__mount_map() {
                        if (!IS_EMPTY(mount_locks)) {
                            unordered_map<string, exclusive_lock *>::iterator iter;
                            for (iter = mount_locks.begin(); iter != mount_locks.end(); iter++) {
                                exclusive_lock *lock = iter->second;
                                CHECK_AND_FREE(lock);
                            }
                            mount_locks.clear();
                        }
                        mount_index.clear();
                    }

                    void add_mount_index(string name, uint16_t index) {
                        PRECONDITION(!IS_EMPTY(name));
                        mount_index.insert({name, index});
                    }

                    short get_mount_index(string name) {
                        PRECONDITION(!IS_EMPTY(name));
                        unordered_map<string, uint16_t>::iterator iter = mount_index.find(name);
                        if (iter != mount_index.end()) {
                            return iter->second;
                        }
                        return (short) -1;
                    }

                    void add_mount_lock(const string *name, exclusive_lock *lock) {
                        PRECONDITION(!IS_EMPTY_P(name));
                        PRECONDITION(NOT_NULL(lock));

                        string key(*name);
                        mount_locks.insert({key, lock});
                    }

                    exclusive_lock *get_mount_lock(string *name) {
                        PRECONDITION(!IS_EMPTY_P(name));
                        string key(*name);
                        unordered_map<string, exclusive_lock *>::iterator iter = mount_locks.find(key);
                        if (iter != mount_locks.end()) {
                            exclusive_lock *lock = iter->second;
                            POSTCONDITION(NOT_NULL(lock));
                            return lock;
                        }
                        return nullptr;
                    }

                    exclusive_lock *get_mount_lock(string mount_point, __mount_data *mounts) {
                        PRECONDITION(!IS_EMPTY(mount_point));
                        CHECK_NOT_NULL(mounts);

                        short index = get_mount_index(mount_point);
                        if (index >= 0) {
                            __mount_point mp = mounts->mounts[index];
                            if (mp.state != __mount_state::MP_UNAVAILABLE) {
                                string name_l = string(mp.lock_name);
                                POSTCONDITION(!IS_EMPTY(name_l));
                                return get_mount_lock(&name_l);
                            }
                        }
                        return nullptr;
                    }
                };

                class mount_utils {
                public:
                    static string get_lock_name(__mount_point *mp, uint16_t index) {
                        string name = string(mp->path);
                        uint16_t off = 16;
                        if (name.length() <= off) {
                            off = name.length() - 1;
                        }
                        return common_utils::format("m_%s_%d", name.substr(0, off).c_str(), index);
                    }


                };
            }
        }
    }
}
#endif //REACTFS_MOUNT_STRUCTS_H
