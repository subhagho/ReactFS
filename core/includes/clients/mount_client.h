//
// Created by Subhabrata Ghosh on 24/11/16.
//


/*
 * Copyright [yyyy] [name of copyright owner]
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

#ifndef REACTFS_MOUNT_CLIENT_H
#define REACTFS_MOUNT_CLIENT_H

#include <unordered_map>

#include "core/includes/common_structs.h"
#include "core/includes/mount_structs.h"

using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                namespace client {
                    class mount_client {
                    private:
                        __version_header version;

                        __mount_data *mounts = nullptr;
                        __mount_map *mount_map = nullptr;

                        bool is_readable(uint16_t index) {
                            __mount_point mp = mounts->mounts[index];
                            if (mp.state == __mount_state::MP_READ_ONLY || mp.state == __mount_state::MP_READ_WRITE) {
                                return true;
                            }
                            return false;
                        }

                        bool is_writable(uint16_t index) {
                            __mount_point mp = mounts->mounts[index];
                            if (mp.state == __mount_state::MP_READ_WRITE) {
                                return true;
                            }
                            return false;
                        }

                    public:
                        mount_client() {
                            version.major = MOUNT_VERSION_MAJOR;
                            version.minor = MOUNT_VERSION_MINOR;
                        }

                        void init(shared_mapped_ptr *ptr) {

                            CHECK_NOT_NULL(ptr);

                            PRECONDITION(ptr->get()->get_size() == sizeof(__mount_data));
                            void *p = ptr->get()->get_data_ptr();
                            CHECK_NOT_NULL(p);

                            mounts = static_cast<__mount_data *>(p);
                            CHECK_CAST(mounts, "void *", TYPE_NAME(__mount_data));

                            version_utils::compatible(version, mounts->version);
                            mount_map = new __mount_map();
                            CHECK_ALLOC(mount_map, TYPE_NAME(__mount_map));
                            for (uint16_t ii = 0; ii < mounts->mount_count; ii++) {
                                __mount_point *mp = &mounts->mounts[ii];
                                if (mp->state == __mount_state::MP_UNAVAILABLE) {
                                    continue;
                                }
                                string name_l = string(mp->lock_name);
                                POSTCONDITION(!IS_EMPTY(name_l));

                                exclusive_lock *lock = nullptr;
                                INIT_LOCK_P(lock, &name_l);

                                mount_map->add_mount_lock(lock->get_name(), lock);
                                mount_map->add_mount_index(string(mp->path), ii);
                            }
                        }

                        string get_mount_point(string *path) const {
                            CHECK_NOT_NULL(mounts);
                            PRECONDITION(!IS_EMPTY_P(path));

                            string r_path = file_utils::cannonical_path(*path);
                            for (uint16_t ii = 0; ii < mounts->mount_count; ii++) {
                                string m_path = string(mounts->mounts[ii].path);
                                POSTCONDITION(!IS_EMPTY(m_path));
                                if (!string_utils::ends_with(&m_path, "/")) {
                                    m_path.append("/");
                                }
                                if (string_utils::starts_with(&r_path, m_path)) {
                                    return m_path;
                                }
                            }
                            return EMPTY_STRING;
                        }

                        void update_read_metrics(string *path, uint64_t bytes_read, uint64_t time) {
                            string m_path = get_mount_point(path);
                            short m_index = mount_map->get_mount_index(m_path);
                            POSTCONDITION(m_index >= 0 && m_index < mounts->mount_count);
                            __mount_point *mp = &mounts->mounts[m_index];
                            if (is_readable(m_index)) {
                                exclusive_lock *lock = mount_map->get_mount_lock(m_path, mounts);
                                CHECK_NOT_NULL(lock);
                                TRY_LOCK_WITH_ERROR(lock, 0, DEFAULT_LOCK_TIMEOUT);
                                mp->total_bytes_read += bytes_read;
                                metrics_utils::update_hourly_metrics(&(mp->bytes_read), bytes_read, time);
                            }
                        }

                        void update_write_metrics(string *path, uint64_t bytes_written, uint64_t time) {
                            string m_path = get_mount_point(path);
                            short m_index = mount_map->get_mount_index(m_path);
                            POSTCONDITION(m_index >= 0 && m_index < mounts->mount_count);
                            __mount_point *mp = &mounts->mounts[m_index];
                            if (is_writable(m_index)) {
                                exclusive_lock *lock = mount_map->get_mount_lock(m_path, mounts);
                                CHECK_NOT_NULL(lock);
                                TRY_LOCK_WITH_ERROR(lock, 0, DEFAULT_LOCK_TIMEOUT);
                                mp->total_bytes_written += bytes_written;
                                metrics_utils::update_hourly_metrics(&(mp->bytes_written), bytes_written, time);
                            }
                        }

                        bool can_create_block(string *path, uint64_t size) {
                            string m_path = get_mount_point(path);
                            short m_index = mount_map->get_mount_index(m_path);
                            POSTCONDITION(m_index >= 0 && m_index < mounts->mount_count);
                            __mount_point *mp = &mounts->mounts[m_index];
                            if (is_writable(m_index) && mp->usage_limit > 0) {
                                string p(mp->path);
                                uint64_t available_bytes = (mp->usage_limit - mp->total_bytes_used);
                                uint64_t actual_freespace = mount_utils::get_free_space(p);
                                available_bytes = (available_bytes < actual_freespace ? available_bytes
                                                                                      : actual_freespace);
                                return (available_bytes >= size);
                            }
                            return false;
                        }

                        bool reserve_block(string *path, uint64_t size) {
                            string m_path = get_mount_point(path);
                            short m_index = mount_map->get_mount_index(m_path);
                            POSTCONDITION(m_index >= 0 && m_index < mounts->mount_count);
                            __mount_point *mp = &mounts->mounts[m_index];
                            if (is_writable(m_index)) {
                                bool reserved = false;
                                exclusive_lock *lock = mount_map->get_mount_lock(m_path, mounts);
                                CHECK_NOT_NULL(lock);
                                WAIT_LOCK_GUARD(lock, 0);
                                if (can_create_block(path, size)) {
                                    mp->total_bytes_used += size;
                                    reserved = true;
                                }
                                return reserved;
                            }
                            return false;
                        }

                        string get_next_mount() {
                            double score = ULONG_MAX;
                            string mount;
                            for (uint16_t ii = 0; ii < mounts->mount_count; ii++) {
                                __mount_point *mp = &mounts->mounts[ii];
                                if (mp->state != __mount_state::MP_READ_WRITE) {
                                    continue;
                                }
                                double bs = mount_utils::get_mount_usage(mp);
                                if (score > bs) {
                                    mount = string(mp->path);
                                    score = bs;
                                }
                            }
                            return mount;
                        }
                    };
                }
            }
        }
    }
}

#endif //REACTFS_MOUNT_CLIENT_H
