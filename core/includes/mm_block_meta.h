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

//
// Created by Subhabrata Ghosh on 20/11/16.
//

#ifndef REACTFS_MM_BLOCK_LIST_H
#define REACTFS_MM_BLOCK_LIST_H

#include <iostream>
#include <fstream>
#include <unordered_map>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/log_utils.h"
#include "common/includes/base_error.h"
#include "common/includes/exclusive_lock.h"
#include "core/includes/base_block.h"

#define MM_BLOCK_FILE_PREFIX "mm_block"
#define MM_META_FILENAME "mm_block.meta"
#define MM_MAX_NAME_SIZE 32
#define MM_MAX_BLOCKS 256
#define MM_MAX_CLIENT_SIZE 256
#define MM_LOCK_PREFIX "_mm_lock_"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                namespace structs {
                    class mm_hash_constants {
                    public:
                        static const string INFO_FILE;
                        static const uint64_t SMALL;
                        static const uint64_t MEDIUM;
                        static const uint64_t LARGE;
                        static const uint64_t XLARGE;
                    };

                    typedef struct __block_info__ {
                        uint64_t block_id;
                        char path[SIZE_MAX_PATH];
                    } __block_info;

                    typedef struct __mm_header__ {
                        char name[MM_MAX_NAME_SIZE];
                        uint32_t block_size = 0;
                        uint32_t block_count = 0;
                        uint64_t write_block_index = 0;
                    } __mm_header;

                    typedef enum __mm_connection_type__ {
                        READ_WRITE = 0, READ_ONLY = 1
                    } __mm_connection_type;

                    typedef struct __mm_client_info__ {
                        pid_t client_pid;
                        __mm_connection_type client_type;
                        uint64_t connected_time;
                        uint64_t last_used_time;

                    } __mm_client_info;

                    class mm_block_meta {
                    private:
                        //! State of this instance of the block object.
                        __state__ state;

                        /// Memory-mapped file handle.
                        fmstream *stream = nullptr;

                        Path *base_dir = nullptr;
                        Path *info_file = nullptr;
                        void *base_ptr = nullptr;

                        __mm_header *header = nullptr;
                        __block_info *block_list = nullptr;
                        unordered_map<uint64_t, base_block *> blocks;
                        exclusive_lock *lock = nullptr;


                        void *get_data_ptr() {
                            return common_utils::increment_data_ptr(base_ptr, sizeof(__mm_header));
                        }

                        void *get_write_ptr() {
                            void *d_ptr = get_data_ptr();
                            return common_utils::increment_data_ptr(d_ptr,
                                                                    (header->write_block_index * sizeof(__block_info)));
                        }

                        void create_info_header(string name, uint32_t block_size) {
                            PRECONDITION(!IS_EMPTY(name));
                            PRECONDITION(name.length() <= MM_MAX_NAME_SIZE);
                            PRECONDITION(block_size > 0);
                            if (block_size <= 0) {
                                block_size = mm_hash_constants::SMALL;
                            }

                            info_file = new Path(base_dir->get_path());
                            info_file->append(MM_META_FILENAME);
                            if (info_file->exists()) {
                                info_file->remove();
                            }
                            uint64_t h_size = sizeof(__mm_header);
                            uint64_t d_size = MM_MAX_BLOCKS * sizeof(__block_info);
                            uint64_t t_size = (h_size + d_size);
                            stream = new fmstream(info_file->get_path().c_str(), t_size);
                            CHECK_NOT_NULL(stream);
                            base_ptr = stream->data();
                            CHECK_NOT_NULL(base_ptr);

                            memset(base_ptr, 0, t_size);

                            header = static_cast<__mm_header *>(base_ptr);
                            CHECK_NOT_NULL(header);
                            memcpy(header->name, name.c_str(), name.length());
                            header->block_count = 0;
                            header->block_size = block_size;
                            header->write_block_index = 0;

                            stream->flush();
                        }

                        void read_info(string name) {
                            PRECONDITION(!IS_EMPTY(name));
                            info_file = new Path(base_dir->get_path());
                            info_file->append(MM_META_FILENAME);
                            if (!info_file->exists()) {
                                throw FS_BASE_ERROR("Metadata file not found. [file=%s]",
                                                    info_file->get_path().c_str());
                            }

                            stream = new fmstream(info_file->get_path().c_str());
                            CHECK_NOT_NULL(stream);
                            base_ptr = stream->data();
                            CHECK_NOT_NULL(base_ptr);

                            header = static_cast<__mm_header *>(base_ptr);
                            CHECK_NOT_NULL(header);
                        }

                        void set_base_dir(string name, string base_dir, bool overwrite) {
                            this->base_dir = new Path(base_dir);
                            this->base_dir->append(name);
                            if (this->base_dir->exists() && overwrite) {
                                this->base_dir->remove();
                            }
                            if (!this->base_dir->exists()) {
                                this->base_dir->create(0750);
                            }
                        }

                        void create_lock(string name) {
                            PRECONDITION(!IS_EMPTY(name));
                            string lock_n = common_utils::format("%s%s", MM_LOCK_PREFIX, name);
                            lock = new exclusive_lock(lock_n);
                            CHECK_NOT_NULL(lock);
                        }

                    public:
                        mm_block_meta(Path *base_dir, string name, uint32_t block_size = 0,
                                      bool overwrite = false) {
                            create_lock(name);
                            WAIT_LOCK_P(lock);
                            try {
                                set_base_dir(name, base_dir, overwrite);

                                info_file = new Path(base_dir->get_path());
                                info_file->append(mm_hash_constants::INFO_FILE);

                                if (info_file->exists()) {
                                    read_info(name);
                                } else {
                                    create_info_header(name, block_size);
                                }
                                RELEASE_LOCK_P(lock);
                            } catch (const exception &e) {
                                RELEASE_LOCK_P(lock);
                                fs_error_base fe = FS_BASE_ERROR(
                                        "Exception occurred while creating metadata instance. [name=%s][error=%s]",
                                        name.c_str(), e.what());
                                state.set_error(fe);
                                throw fe;
                            } catch (...) {
                                RELEASE_LOCK_P(lock);
                                fs_error_base fe = FS_BASE_ERROR(
                                        "Exception occurred while creating metadata instance. [name=%s][error=%s]",
                                        name.c_str(), "UNKNOWN");
                                state.set_error(fe);
                                throw fe;
                            }
                            state.set_state(__state_enum::Available);
                        }

                        ~mm_block_meta() {
                            CHECK_AND_DISPOSE(state);

                            if (lock->is_locked()) {
                                lock->release_lock();
                            }
                            CHECK_AND_FREE(lock);

                            if (NOT_NULL(stream)) {
                                if (stream->is_open()) {
                                    stream->close();
                                }
                                delete (stream);
                                stream = nullptr;
                            }
                            for (unordered_map<uint64_t, base_block *>::const_iterator it = blocks.begin();
                                 it != blocks.end(); it++) {
                                base_block *block = it->second;
                                CHECK_AND_FREE(block);
                            }
                            blocks.clear();
                            for (__block_info *bi : block_files) {
                                CHECK_AND_FREE(bi);
                            }
                            block_files.clear();
                            block_files.clear();
                            CHECK_AND_FREE(info_file);
                            CHECK_AND_FREE(base_dir);
                        }

                        uint32_t get_block_count() {
                            return block_files.size();
                        }

                        vector<string> get_block_files() {
                            return block_files;
                        }

                        __block_info *get_new_block_info(long use_block_id = -1) {
                            CHECK_NOT_NULL(base_dir);
                            Path bp(base_dir->get_path());
                            uint64_t block_id;
                            if (use_block_id >= 0) {
                                block_id = (uint64_t) use_block_id;
                            } else {
                                block_id = block_count++;
                            }
                            string filename = common_utils::format("%s.%s_%d.RAW", name, MM_BLOCK_FILE_PREFIX,
                                                                   block_id);
                            __block_info *bi = (__block_info *) malloc(sizeof(__block_info));
                            CHECK_NOT_NULL(bi);
                            bi->filename = string(filename);
                            bi->block_id = block_id;

                            return bi;
                        }

                        uint32_t get_block_size() {
                            return block_size;
                        }

                        bool wait_lock() {
                            CHECK_NOT_NULL(lock);
                            return lock->wait_lock();
                        }

                        bool try_lock() {
                            CHECK_NOT_NULL(lock);
                            return lock->try_lock();
                        }

                        bool release_lock() {
                            CHECK_NOT_NULL(lock);
                            PRECONDITION(lock->is_locked());
                            return lock->release_lock();
                        }
                    };
                }
            }
        }
    }
}
#endif //REACTFS_MM_BLOCK_LIST_H
