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

#include <sys/statvfs.h>
#include <climits>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/exclusive_lock.h"
#include "mount_structs_v0.h"


#define MOUNTS_KEY "env-mount-points"
#define MOUNTS_CONFIG_NODE "/configuration/node/mount-points"
#define MOUNTS_CONFIG_PATH "path"
#define MOUNTS_CONFIG_LIMIT "usage-limit"

#define MOUNT_VERSION_MAJOR ((uint16_t) 0)
#define MOUNT_VERSION_MINOR ((uint16_t) 1)

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {

                typedef struct __mount_def__ {
                    string *path;
                    int64_t usage_limit;
                } __mount_def;

                typedef __mount_data_v0 __mount_data;
                typedef __mount_point_v0 __mount_point;

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

                    exclusive_lock *get_mount_lock(string &mount_point, __mount_data *mounts) {
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

                    static double get_mount_usage(__mount_point *mp) {
                        string p(mp->path);
                        double score = 0.0f;

                        uint64_t f_bytes = get_free_space(p);
                        uint64_t t_bytes = get_total_space(p);

                        uint64_t t_u_read = mp->total_bytes_read;
                        uint64_t t_u_write = mp->total_bytes_written;

                        uint64_t r_u_read = mp->bytes_read;
                        uint64_t r_u_write = mp->bytes_written;

                        uint64_t r_u_timer = mp->time_read;
                        uint64_t r_u_timew = mp->time_write;

                        double d_usage = ((double) t_bytes) / f_bytes;
                        double t_r_usage = ((double) t_u_read) / G_BYTES;
                        double t_w_usage = ((double) t_u_write) / G_BYTES;
                        double r_r_usage = ((double) r_u_read) / G_BYTES;
                        double r_w_usage = ((double) r_u_write) / G_BYTES;

                        double io_r_usage = (r_u_timer > 0 ? ((double) r_u_read) / r_u_timer : 0);
                        double io_w_usage = (r_u_timew > 0 ? ((double) r_u_write) / r_u_timew : 0);

                        score = (d_usage + t_r_usage + (2.0 * t_w_usage) + (2.0 * r_r_usage) + (3.0 * r_w_usage) +
                                 (3.0 * io_r_usage) + (4.0 * io_w_usage));
                        return score;
                    }

                    static uint64_t get_free_space(string &mount_p) {
                        PRECONDITION(!IS_EMPTY(mount_p));

                        struct statvfs buff;
                        if (statvfs(mount_p.c_str(), &buff) != 0) {
                            throw BASE_ERROR("Error getting space stats. [file=%s][error=%s]", mount_p.c_str(),
                                             strerror(errno));
                        }
                        return (buff.f_frsize * buff.f_bfree);
                    }

                    static uint64_t get_total_space(string &mount_p) {
                        PRECONDITION(!IS_EMPTY(mount_p));

                        struct statvfs buff;
                        if (statvfs(mount_p.c_str(), &buff) != 0) {
                            throw BASE_ERROR("Error getting space stats. [file=%s][error=%s]", mount_p.c_str(),
                                             strerror(errno));
                        }
                        return (buff.f_frsize * buff.f_blocks);
                    }
                };
            }
        }
    }
}
#endif //REACTFS_MOUNT_STRUCTS_H
