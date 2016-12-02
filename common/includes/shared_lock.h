//
// Created by Subhabrata Ghosh on 02/12/16.
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

#ifndef REACTFS_SHARED_LOCK_H
#define REACTFS_SHARED_LOCK_H

#include <mutex>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <thread>
#include <unordered_map>

#include "exclusive_lock.h"
#include "timer.h"
#include "__alarm.h"
#include "process_utils.h"
#include "unordered_map"
#include "__bitset.h"
#include "mapped_data.h"
#include "lock_structs_v0.h"

#define DEFAULT_LOCK_MGR_SLEEP 30 * 1000
#define DEFAULT_READ_LOCK_TIMEOUT 5 * 60 * 1000
#define DEFAULT_WRITE_LOCK_TIMEOUT  60 * 1000

#define SHARED_LOCK_TABLE_PREFIX "TBSLK_"
#define SHARED_LOCK_PREFIX "_LKSTB"
#define SHARED_LOCK_MAJ_VERSION ((uint16_t) 0)
#define SHARED_LOCK_MIN_VERSION ((uint16_t) 1)

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                typedef __shared_lock_data_v0 __shared_lock_data;
                typedef __owner_v0 __owner;

                /*!
                 * Base class used for defining shared lock data.
                 *
                 */
                class base_lock_table {
                protected:
                    /// Current version of the data structures being used.
                    __version_header version;

                    /// Name of this lock group.
                    string group;

                    /// Memory-mapped file handle.
                    shm_mapped_data *mm_data = nullptr;

                    /// Shared lock handle to writing data.
                    exclusive_lock *table_lock = nullptr;

                    /// Persisted data header pointer.
                    __shared_lock_data *header_ptr = nullptr;

                    /*!
                     * Create/Map the data file to the local instance.
                     *
                     * @param max_count - Maximum number of shared locks allowed
                     * @param struct_size - Size of the lock structure.
                     * @param mode - File create mode.
                     * @param manager - Is this a server instance.
                     * @param reset - Reset the lock table.
                     */
                    void __create(uint32_t max_count, uint32_t struct_size, mode_t mode, bool manager, bool reset);

                    /*!
                     * Get the name of the lock table for this group.
                     *
                     * @return - Lock table name.
                     */
                    string get_table_name() {
                        return common_utils::format("%s.%s", SHARED_LOCK_TABLE_PREFIX, group.c_str());
                    }

                    /*!
                     * Get the name of shared lock for this group.
                     *
                     * @return - Shared lock name.
                     */
                    string get_lock_name() {
                        return common_utils::format("%s_%s", SHARED_LOCK_PREFIX, group.c_str());
                    }

                    /*!
                     * Get the base data pointer for the lock table.
                     *
                     * @return - Base data pointer.
                     */
                    void *get_data_ptr() {
                        uint64_t h_size = sizeof(__shared_lock_data);
                        return common_utils::increment_data_ptr(mm_data->get_base_ptr(), h_size);
                    }

                public:
                    /*!<constructor
                     * Create a new instance shared data (SHM based) table for the specified lock group.
                     *
                     * @param group - Lock group name.
                     */
                    base_lock_table(string group) {
                        CHECK_NOT_EMPTY(group);
                        this->group = group;
                    }

                    /*!<destructor
                     * Default table destructor.
                     */
                    virtual ~base_lock_table() {
                        CHECK_AND_FREE(mm_data);
                        CHECK_AND_FREE(table_lock);
                        header_ptr = nullptr;
                        table_lock = nullptr;
                        mm_data = nullptr;
                    }

                    /*!
                     * Get the max number of shared locks that can be registered.
                     *
                     * @return - Max shared locks.
                     */
                    uint32_t get_max_size() {
                        PRECONDITION(NOT_NULL(header_ptr));
                        return header_ptr->max_count;
                    }

                    /*!
                     * Get the number of shared lock slots currently being used.
                     *
                     * @return - Used shared lock slots.
                     */
                    uint32_t get_used_size() {
                        PRECONDITION(NOT_NULL(header_ptr));
                        return header_ptr->used_count;
                    }

                    /*!
                     * Get the shared lock used to synchronize this lock group.
                     *
                     * @return - Shared lock instance.
                     */
                    exclusive_lock *get_lock() {
                        PRECONDITION(NOT_NULL(header_ptr));
                        return table_lock;
                    }
                };

                /*!
                 * Enum to define the type of locking.
                 */
                typedef enum __lock_type__ {
                    /// Write lock only
                            LOCK_W = 0,
                    /// Read/Write Lock.
                            LOCK_RW = 1
                } __lock_type;

                class lock_client {
                protected:
                    /// State of this instance of the client.
                    __state__ state;

                    /// Thread synchronisation mutex
                    mutex thread_mutex;

                    /// Lock group name.
                    string group;

                    /// Type of lock(s) this group contains.
                    __lock_type lock_type;

                    /*!
                     * Create/initialize this instance of the client.
                     *
                     * @param mode - Mode to create the lock with.
                     * @param is_manager - Is this instance a manager?
                     * @param reset - Reset the lock environment?
                     */
                    virtual void create(mode_t mode, bool is_manager, bool reset) = 0;

                public:
                    /*!<constructor
                     * Default constructor of the base lock client.
                     *
                     * @param group - Lock group this client manages.
                     * @param lock_type - Type of lock(s) this group contains.
                     */
                    lock_client(string group, __lock_type lock_type) {
                        CHECK_NOT_EMPTY(group);
                        this->group = group;
                        this->lock_type = lock_type;
                    }

                    /*!<destructor
                     * Default virtual destructor
                     */
                    virtual ~lock_client() {

                    }

                    /*!
                     * Get the lock group name.
                     *
                     * @return - Lock group name.
                     */
                    string get_group() {
                        return this->group;
                    }

                    /*!
                     * Get the type of lock(s) this group contains.
                     *
                     * @return - Lock type.
                     */
                    __lock_type get_lock_type() {
                        return this->lock_type;
                    }
                };
            }
        }
    }
}


#endif //REACTFS_SHARED_LOCK_H
