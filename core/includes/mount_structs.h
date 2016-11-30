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

#include "common/includes/metrics.h"
#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/exclusive_lock.h"
#include "mount_structs_v0.h"


#define MOUNTS_KEY "env-mount-points"
#define MOUNTS_CONFIG_NODE "/configuration/node/mount-points"
#define MOUNTS_CONFIG_PATH "path"
#define MOUNTS_CONFIG_LIMIT "usage-limit"
#define MOUNTS_CONFIG_STATE "state"

#define MOUNT_VERSION_MAJOR ((uint16_t) 0)
#define MOUNT_VERSION_MINOR ((uint16_t) 1)

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                typedef __mount_data_v0 __mount_data;
                typedef __mount_point_v0 __mount_point;

                /*!
                 * Structure to read the mount points defined in the start-up configuration.
                 */
                typedef struct __mount_def__ {
                    /// File system path of the mount point.
                    string *path;
                    /// Usage Limit of the mount point (-1 signifies use all available disk space)
                    int64_t usage_limit;
                    /// State of the mount point
                    __mount_state state = __mount_state::MP_READ_WRITE;
                } __mount_def;

                /*!
                 * In-memory data structure for associated data for managing and using
                 * mount points.
                 */
                class __mount_map {
                private:
                    /// Map of locks defined for writing to the shared mount data.
                    unordered_map<string, exclusive_lock *> mount_locks;
                    /// Map of the record indexes for the mount points.
                    unordered_map<string, uint16_t> mount_index;

                public:
                    /*!<destructor
                     * Dispose the map.
                     *
                     * Will delete the mount locks.
                     */
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

                    /*!
                     * Add a mount record index to the index map.
                     *
                     * @param name - Mount point (path)
                     * @param index - Record index.
                     */
                    void add_mount_index(string name, uint16_t index) {
                        PRECONDITION(!IS_EMPTY(name));
                        if (name.back() != '/') {
                            name.append("/");
                        }
                        mount_index.insert({name, index});
                    }

                    /*!
                     * Get the mount point record index for the specified mount path.
                     *
                     * @param name - Mount point (path)
                     * @return - Record index.
                     */
                    short get_mount_index(string name) {
                        PRECONDITION(!IS_EMPTY(name));
                        if (name.back() != '/') {
                            name.append("/");
                        }
                        unordered_map<string, uint16_t>::iterator iter = mount_index.find(name);
                        if (iter != mount_index.end()) {
                            return iter->second;
                        }
                        return (short) -1;
                    }

                    /*!
                     * Add an instance of a shared lock used for inter-process synchronisation.
                     *
                     * @param name - Mount point (path)
                     * @param lock - Shared lock.
                     */
                    void add_mount_lock(const string *name, exclusive_lock *lock) {
                        PRECONDITION(!IS_EMPTY_P(name));
                        PRECONDITION(NOT_NULL(lock));

                        string key(*name);
                        if (key.back() != '/') {
                            key.append("/");
                        }
                        TRACE("Adding mount lock [key=%s]", key.c_str());
                        mount_locks.insert({key, lock});
                    }

                    /*!
                     * Get the shared lock to be used for updating data for this mount point.
                     *
                     * @param name - Mount point lock name (this isn't the same as the mount path)
                     * @return - Shared lock.
                     */
                    exclusive_lock *get_mount_lock(string *name) {
                        PRECONDITION(!IS_EMPTY_P(name));
                        string key(*name);
                        if (key.back() != '/') {
                            key.append("/");
                        }
                        unordered_map<string, exclusive_lock *>::iterator iter = mount_locks.find(key);
                        if (iter != mount_locks.end()) {
                            TRACE("Found mount lock for %s", key.c_str());
                            exclusive_lock *lock = iter->second;
                            POSTCONDITION(NOT_NULL(lock));
                            return iter->second;
                        }
                        return nullptr;
                    }

                    /*!
                     * Get the shared lock to be used for updating data for this mount point.
                     * This is a utiliy function that will find the lock name associated with the specified
                     * mount point and then return lock handle.
                     *
                     * @param name - Mount point (path)
                     * @param mounts - Pointer to the mount points data strucutre.
                     *
                     * @return - Shared lock.
                     */
                    exclusive_lock *get_mount_lock(string &mount_point, __mount_data *mounts) {
                        PRECONDITION(!IS_EMPTY(mount_point));
                        CHECK_NOT_NULL(mounts);

                        short index = get_mount_index(mount_point);
                        if (index >= 0) {
                            __mount_point mp = mounts->mounts[index];
                            if (mp.state != __mount_state::MP_UNAVAILABLE) {
                                string name_l = common_utils::get_normalized_name(mp.lock_name);
                                POSTCONDITION(!IS_EMPTY(name_l));
                                string n = exclusive_lock::get_lock_name(&name_l);
                                return get_mount_lock(&n);
                            }
                        }
                        return nullptr;
                    }
                };

                /*!
                 * Class defines static utility functions for mount point related operations.
                 */
                class mount_utils {
                public:
                    /*!
                     * Get the shared lock name for the specified mount definition.
                     *
                     * @param mp - Mount definition.
                     * @param index - Index of this mount point in the mount array.
                     * @return - Name to be used for creating the shared lock.
                     */
                    static string get_lock_name(__mount_point *mp, uint16_t index) {
                        string name = string(mp->path);
                        uint16_t off = 16;
                        if (name.length() <= off) {
                            off = name.length() - 1;
                        }
                        return common_utils::format("m_%s_%d", name.substr(0, off).c_str(), index);
                    }

                    /*!
                     * Get a computed usage stat for this mount point.
                     *
                     * The computation takes into account the following:
                     *  - Free/Total space
                     *  - Total Read/Writes to this mount
                     *  - Recent Read/Writes to this mount (last 24Hrs)
                     *  - Computed IO Stats (last 24Hrs)
                     *
                     * @param mp
                     * @return
                     */
                    static double get_mount_usage(__mount_point *mp) {
                        string p(mp->path);
                        double score = 0.0f;

                        uint64_t t_u_read = mp->total_bytes_read;
                        uint64_t t_u_write = mp->total_bytes_written;

                        uint64_t r_u_read = mp->bytes_read.total_value;
                        uint64_t r_u_write = mp->bytes_written.total_value;

                        uint64_t r_u_timer = mp->bytes_read.total_time;
                        uint64_t r_u_timew = mp->bytes_written.total_time;

                        double d_usage = ((double) mp->total_bytes_used) / M_BYTES;
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

                    /*!
                     * Get the available disk space for the specified mount path.
                     *
                     * @param mount_p - Mount path.
                     * @return - Available disk space.
                     */
                    static uint64_t get_free_space(string &mount_p) {
                        PRECONDITION(!IS_EMPTY(mount_p));

                        struct statvfs buff;
                        if (statvfs(mount_p.c_str(), &buff) != 0) {
                            throw BASE_ERROR("Error getting space stats. [file=%s][error=%s]", mount_p.c_str(),
                                             strerror(errno));
                        }
                        return (buff.f_frsize * buff.f_bfree);
                    }

                    /*!
                     * Get the total disk space for the specified mount path.
                     *
                     * @param mount_p - Mount path.
                     * @return - Total disk space.
                     */
                    static uint64_t get_total_space(string &mount_p) {
                        PRECONDITION(!IS_EMPTY(mount_p));

                        struct statvfs buff;
                        if (statvfs(mount_p.c_str(), &buff) != 0) {
                            throw BASE_ERROR("Error getting space stats. [file=%s][error=%s]", mount_p.c_str(),
                                             strerror(errno));
                        }
                        return (buff.f_frsize * buff.f_blocks);
                    }

                    /*!
                     * Parse the mount point state from the specified state string.
                     *
                     * @param state - State string.
                     * @return - Parsed mount state.
                     */
                    static __mount_state parse_state(string state) {
                        PRECONDITION(!IS_EMPTY(state));
                        state = string_utils::trim(state);
                        state = string_utils::toupper(state);
                        if (state == __MP_READ_WRITE) {
                            return __mount_state::MP_READ_WRITE;
                        } else if (state == __MP_READ_ONLY) {
                            return __mount_state::MP_READ_ONLY;
                        } else if (state == __MP_UNAVAILABLE) {
                            return __mount_state::MP_UNAVAILABLE;
                        } else if (state == __MP_CORRUPTED) {
                            return __mount_state::MP_CORRUPTED;
                        }
                        throw BASE_ERROR("Unknown mount state specified. [state=%s]", state.c_str());
                    }
                };

                class mount_metrics {
                public:

                    static void reset_hourly_metrics(__hourly_usage_metric *metric) {
                        metric->reset_time = time_utils::now();
                        metric->current_index = 0;
                        metric->total_time = 0;
                        metric->total_value = 0;
                        for (uint16_t ii = 0; ii < metric->size; ii++) {
                            metric->records[ii].index = ii;
                            metric->records[ii].time = 0;
                            metric->records[ii].value = 0;
                        }
                    }

                    static void update_hourly_metrics(__hourly_usage_metric *metric, uint64_t value, uint64_t time) {
                        uint32_t hours = time_utils::get_hour_diff(metric->reset_time);
                        if (hours >= metric->size) {
                            reset_hourly_metrics(metric);
                        } else {
                            uint16_t max = (hours >= metric->size ? metric->size : hours);
                            uint16_t index = metric->current_index + 1;
                            for (uint16_t ii = 0; ii < max; ii++) {
                                metric->total_value -= metric->records[index].value;
                                metric->total_time -= metric->records[index].time;

                                metric->records[index].index = ii;
                                metric->records[index].time = 0;
                                metric->records[index].value = 0;
                                index++;
                                if (index >= metric->size) {
                                    index = 0;
                                }
                            }
                            metric->current_index++;
                        }
                        metric->total_value += value;
                        metric->total_time += time;
                        metric->records[metric->current_index].value += value;
                        metric->records[metric->current_index].time += time;
                    }

                    static __avg_metric *get_hourly_metric(string name, __hourly_usage_metric *metric) {
                        uint32_t hour = time_utils::get_hour(nullptr);
                        POSTCONDITION(hour >= 0 && hour < 24);
                        __avg_metric *avg = new __avg_metric(name, false);
                        CHECK_ALLOC(metric, TYPE_NAME(_avg_metric));
                        avg->set(metric->records[hour].value, metric->records[hour].time);

                        return avg;
                    }

                    static void get_hourly_metric(__avg_metric *avg, __hourly_usage_metric *metric) {
                        uint32_t hour = time_utils::get_hour(nullptr);
                        POSTCONDITION(hour >= 0 && hour < 24);
                        CHECK_ALLOC(metric, TYPE_NAME(_avg_metric));
                        avg->set(metric->records[hour].value, metric->records[hour].time);
                    }
                };
            }
        }
    }
}
#endif //REACTFS_MOUNT_STRUCTS_H
