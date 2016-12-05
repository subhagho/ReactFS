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
                    /*!
                     * Client handle class to interact with defined mount points.
                     * Assumes the mount manager has already initialized the mount configuration for this node.
                     */
                    class mount_client {
                    private:
                        /// Data structure version for the mount strucutres.
                        __version_header version;

                        /// Handle to the shared mount data.
                        __mount_data *mounts = nullptr;

                        /// Local structures to manage mount interactions.
                        __mount_map *mount_map = nullptr;

                        /*!
                         * Check if the mount at the specified index is readable.
                         *
                         * @param index - Mount array index.
                         * @return - Is readable?
                         */
                        bool is_readable(uint16_t index) {
                            __mount_point mp = mounts->mounts[index];
                            if (mp.state == __mount_state::MP_READ_ONLY || mp.state == __mount_state::MP_READ_WRITE) {
                                return true;
                            }
                            return false;
                        }

                        /*!
                         * Check if the mount at the specified index is writable.
                         *
                         * @param index - Mount array index.
                         * @return - Is writable?
                         */
                        bool is_writable(uint16_t index) {
                            __mount_point mp = mounts->mounts[index];
                            if (mp.state == __mount_state::MP_READ_WRITE) {
                                return true;
                            }
                            return false;
                        }

                    public:
                        /*!<constructor
                         * Default mount client constructor.
                         */
                        mount_client() {
                            version.major = MOUNT_VERSION_MAJOR;
                            version.minor = MOUNT_VERSION_MINOR;
                        }

                        /*!<destructor
                         * Default destructor for the mount client.
                         */
                        ~mount_client() {
                            CHECK_AND_FREE(mount_map);
                            mounts = nullptr;
                        }

                        /*!
                         * Initialize this mount client using the mapped data passed in the shared pointer.
                         *
                         * @param ptr - Mount data pointer.
                         */
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

                        /*!
                         * Get the mount point that the specified file system path is under.
                         *
                         * @param path - File system path.
                         * @return - Associated mount point.
                         */
                        string get_mount_point(const string *path) const {
                            CHECK_NOT_NULL(mounts);
                            PRECONDITION(!IS_EMPTY_P(path));

                            string r_path = file_utils::cannonical_path(*path);
                            for (uint16_t ii = 0; ii < mounts->mount_count; ii++) {
                                string m_path = string(mounts->mounts[ii].path);
                                POSTCONDITION(!IS_EMPTY(m_path));
                                if (*path == m_path) {
                                    return m_path;
                                }
                                if (!string_utils::ends_with(&m_path, "/")) {
                                    m_path.append("/");
                                }
                                if (string_utils::starts_with(&r_path, m_path)) {
                                    return m_path;
                                }
                            }
                            return common_consts::EMPTY_STRING;
                        }

                        /*!
                         * Update the read metrics associated with the mount point for the specified path.
                         *
                         * @param path - File system path.
                         * @param bytes_read - Bytes read.
                         * @param time - Time to read the specified bytes.
                         */
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
                                mount_metrics::update_hourly_metrics(&(mp->bytes_read), bytes_read, time);
                            }
                        }

                        /*!
                         * Update the write metrics associated with the mount point for the specified path.
                         *
                         * @param path - File system path.
                         * @param bytes_written - Bytes written.
                         * @param time - Time to write the specified bytes.
                         */
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
                                mount_metrics::update_hourly_metrics(&(mp->bytes_written), bytes_written, time);
                            }
                        }

                        /*!
                         * Check if a block of the specified size can be created on the passed mount point.
                         *
                         * @param path - Mount point path.
                         * @param size - Required data size.
                         * @return - Can create?
                         */
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

                        /*!
                         * Reserve space on the mount point associated with the specified path.
                         *
                         * @param path - Path to get the mount point.
                         * @param size - Size to reserve.
                         * @return - Is reserved?
                         */
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
                                    mp->total_blocks++;
                                    reserved = true;
                                }
                                return reserved;
                            }
                            return false;
                        }

                        /*!
                         * Get the next mount point to be used to create a block.
                         *
                         * @param size - Requested block size.
                         * @return - Mount point.
                         */
                        string get_next_mount(uint64_t size) {
                            double score = ULONG_MAX;
                            string mount;
                            for (uint16_t ii = 0; ii < mounts->mount_count; ii++) {
                                __mount_point *mp = &mounts->mounts[ii];
                                if (mp->state != __mount_state::MP_READ_WRITE) {
                                    continue;
                                }

                                uint64_t avail = (mp->usage_limit - mp->total_bytes_used);
                                if (avail < size) {
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

                        /*!
                         * Release the data reserved for the file used by a deleted block.
                         *
                         * @param path - Path of the deleted block.
                         * @param size - File size of the file being deleted.
                         */
                        void release_block(const string *path, uint64_t size) {
                            string m_path = get_mount_point(path);
                            short m_index = mount_map->get_mount_index(m_path);
                            POSTCONDITION(m_index >= 0 && m_index < mounts->mount_count);
                            __mount_point *mp = &mounts->mounts[m_index];
                            exclusive_lock *lock = mount_map->get_mount_lock(m_path, mounts);
                            CHECK_NOT_NULL(lock);
                            WAIT_LOCK_GUARD(lock, 0);
                            mp->total_bytes_used -= size;
                            mp->total_blocks--;
                        }
                    };
                }
            }
        }
    }
}

#endif //REACTFS_MOUNT_CLIENT_H
