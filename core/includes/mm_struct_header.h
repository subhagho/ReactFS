//
// Created by Subhabrata Ghosh on 21/11/16.
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

#ifndef REACTFS_MM_STRUCT_HEADER_H
#define REACTFS_MM_STRUCT_HEADER_H

#include <mutex>
#include <vector>
#include <unordered_map>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/file_utils.h"
#include "common/includes/exclusive_lock.h"
#include "common/includes/mapped_data.h"

#include "fs_error_base.h"
#include "base_block.h"
#include "block_utils.h"

#define MM_STRUCT_NAME_SIZE 64

#define MM_HEADER_FILENAME "mm_struct_header.data"
#define MM_BLOCK_FILE_FMT "mm_%s_block_%lu.data"

#define MM_LOCK_PREFIX "_mm_"
#define MM_MAX_BLOCKS 1024
#define MM_MAX_MOUNTS 32

#define MM_SIZE_SMALL 64 * 1024 * 1024
#define MM_SIZE_MEDIUM 128 * 1024 * 1014
#define MM_SIZE_LARGE 512 * 1024 * 124
#define MM_SIZE_XLARGE 1024 * 1024 * 1024

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                /*!
                 * Structure defines mount points that can be defined to balance IO load
                 * across disks.
                 */
                typedef struct __mm_mount__ {
                    /// Root path of the mount point. All files will be created under this root path.
                    char path[SIZE_MAX_PATH];
                    /// Number of blocks used by this block list on this mount point.
                    uint32_t blocks_used = 0;
                    /// Last timestamp when a block was created on this mount point.
                    uint64_t last_block_time = 0;
                } __mm_mount;

                /*!
                 * Block list record that defines the blocks used by this block list.
                 */
                typedef struct __mm_block_info__ {
                    /// Absolute block file path.
                    char filename[SIZE_MAX_PATH];
                    /// Unique block id.
                    uint32_t block_id;
                    /// Has this block been deleted.
                    bool deleted = false;
                    /// Start index of the records in this block.
                    uint64_t block_start_index = 0;
                    /// Last index of the record written to this block.
                    uint64_t block_last_index = 0;
                } __mm_block_info;

                /*!
                 * Header structure used by the block list(s) to capture block information.
                 */
                typedef struct __mm_data_header__ {
                    /// Schema version of the saved data
                    __version_header version;
                    /// Unique name of this block list.
                    char name[MM_STRUCT_NAME_SIZE];
                    /// Sub-directory prefix, used while creating block files under the mount points.
                    char dir_prefix[SIZE_MAX_PATH];
                    /// Number of mount points defined.
                    uint16_t mounts = 0;
                    /// Size of the blocks to be created.
                    uint32_t block_size = 0;
                    /// Estimated block record size (used for estimating the index file size.)
                    uint32_t block_record_size = 0;
                    /// Number of blocks created by this list (count includes the deleted blocks)
                    uint32_t block_count = 0;
                    /// Block index of the currently writable block.
                    uint32_t write_block_index = 0;
                    /// Last record index that has been written.
                    uint64_t last_written_index = 0;
                    /// Array of block information.
                    __mm_block_info block_array[MM_MAX_BLOCKS];
                    /// Array of defined mount points.
                    __mm_mount mount_points[MM_MAX_MOUNTS];
                } __mm_data_header;


                class mm_struct_header {
                private:
                    static __version_header __SCHEMA_VERSION__;

                    __state__ state;

                    mutex thread_mutex;
                    string name;
                    Path *base_dir = nullptr;

                    exclusive_lock *data_lock = nullptr;

                    file_mapped_data *mm_header_data = nullptr;
                    shm_mapped_data *mm_client_data = nullptr;

                    __mm_data_header *header = nullptr;
                    unordered_map<uint32_t, base_block *> block_index;

                    string get_filename(string name) {
                        Path p(base_dir->get_path());
                        p.append(name);
                        return string(p.get_path());
                    }

                    __mm_block_info *get_block_info(uint32_t index) {
                        PRECONDITION(NOT_NULL(header));
                        PRECONDITION(index < header->block_count);

                        return (header->block_array + index);
                    }


                    void create_new_block() {
                        uint32_t index = header->block_count;
                        __mm_mount *mount = nullptr;
                        for (uint16_t ii = 0; ii < header->mounts; ii++) {
                            if (IS_NULL(mount)) {
                                mount = (header->mount_points);
                                continue;
                            }
                            __mm_mount *nptr = (header->mount_points + ii);
                            if (nptr->last_block_time < mount->last_block_time) {
                                mount = nptr;
                            }
                        }
                        PRECONDITION(NOT_NULL(mount));
                        PRECONDITION(!IS_EMPTY(string(mount->path)));

                        mount->blocks_used++;
                        mount->last_block_time = time_utils::now();

                        Path p(mount->path);
                        string d = string(header->dir_prefix);
                        if (!IS_EMPTY(d)) {
                            p.append(d);
                        }
                        p.append(this->name);
                        if (!p.exists()) {
                            p.create(DEFAULT_RESOURCE_MODE);
                        }

                        string filename = common_utils::format(MM_BLOCK_FILE_FMT, this->name.c_str(), index);
                        p.append(filename);
                        if (p.exists()) {
                            throw FS_BASE_ERROR("Error creating block : Block file already exists. [file=%s]",
                                                p.get_path().c_str());
                        }

                        uint64_t r_index = 0;
                        if (header->block_count > 0) {
                            base_block *block = get_block(header->write_block_index);
                            CHECK_NOT_NULL(block);
                            r_index = block->get_last_index();
                            block->finish();
                        }
                        string uuid = block_utils::create_new_block(index, p.get_path(), __block_type::PRIMARY,
                                                                    header->block_size,
                                                                    header->block_record_size, r_index);
                        POSTCONDITION(!IS_EMPTY(uuid));
                        header->write_block_index = index;
                        header->block_count++;
                    }

                    void create_new_instance(uint64_t block_size, uint32_t record_size, string dir_prefix,
                                             vector<string> *mounts,
                                             bool overwrite) {
                        if (base_dir->exists()) {
                            if (overwrite) {
                                base_dir->remove();
                            } else {
                                throw FS_BASE_ERROR("Struct base directory already exists. [base dir=%s]",
                                                    base_dir->get_path().c_str());
                            }
                        }
                        base_dir->create(DEFAULT_RESOURCE_MODE);
                        string h_filename = get_filename(MM_HEADER_FILENAME);
                        uint64_t h_size = sizeof(__mm_data_header);
                        mm_header_data = new file_mapped_data(h_filename, h_size);
                        CHECK_NOT_NULL(mm_header_data);

                        void *ptr = mm_header_data->get_base_ptr();
                        header = static_cast<__mm_data_header *>(ptr);
                        memcpy(header->name, this->name.c_str(), this->name.length());
                        header->version.major = __SCHEMA_VERSION__.major;
                        header->version.minor = __SCHEMA_VERSION__.minor;
                        header->block_size = block_size;
                        header->block_count = 0;
                        header->write_block_index = 0;
                        header->block_record_size = record_size;

                        if (IS_EMPTY_P(mounts)) {
                            header->mounts = 1;
                            string path = this->base_dir->get_path();
                            strncpy(header->mount_points[0].path, path.c_str(), path.length());
                            header->mount_points[0].blocks_used = 0;
                        } else {
                            header->mounts = mounts->size();
                            for (uint32_t ii = 0; ii < mounts->size(); ii++) {
                                string path = (*mounts)[ii];
                                strncpy(header->mount_points[ii].path, path.c_str(), path.length());
                                header->mount_points[ii].blocks_used = 0;
                            }
                        }
                        create_new_block();

                        mm_header_data->flush();
                    }

                    void init() {
                        PRECONDITION(base_dir->exists());
                        string h_filename = get_filename(MM_HEADER_FILENAME);
                        Path p(h_filename);
                        PRECONDITION(p.exists());

                        mm_header_data = new file_mapped_data(h_filename);
                        CHECK_NOT_NULL(mm_header_data);

                        void *ptr = mm_header_data->get_base_ptr();
                        header = static_cast<__mm_data_header *>(ptr);
                        POSTCONDITION(strncmp(header->name, this->name.c_str(), this->name.length()) == 0);
                        PRECONDITION(version_utils::compatible(header->version, __SCHEMA_VERSION__));
                    }

                    bool is_block_loaded(uint32_t index) {
                        unordered_map<uint32_t, base_block *>::const_iterator iter = block_index.find(index);
                        if (iter != block_index.end()) {
                            return true;
                        }
                        return false;
                    }

                    base_block *get_block(uint32_t index) {
                        CHECK_STATE_AVAILABLE(state);
                        base_block *block = nullptr;
                        unordered_map<uint32_t, base_block *>::const_iterator iter = block_index.find(index);
                        if (iter != block_index.end()) {
                            block = iter->second;
                            CHECK_NOT_NULL(block);
                        } else {
                            __mm_block_info *bi = get_block_info(index);
                            if (bi->deleted) {
                                return nullptr;
                            }

                            lock_guard<std::mutex> lock(thread_mutex);
                            if (is_block_loaded(index)) {
                                unordered_map<uint32_t, base_block *>::const_iterator niter = block_index.find(index);
                                if (niter != block_index.end()) {
                                    block = niter->second;
                                    CHECK_NOT_NULL(block);
                                }
                            } else {
                                CHECK_NOT_NULL(bi);
                                PRECONDITION(bi->block_id >= 0);
                                PRECONDITION(!IS_EMPTY(string(bi->filename)));
                                PRECONDITION(!bi->deleted);

                                block = new base_block();
                                CHECK_NOT_NULL(block);
                                block->open(bi->block_id, bi->filename);
                                POSTCONDITION(block->get_block_state() == __state_enum::Available);

                                block_index[index] = block;
                            }
                        }
                        return block;
                    }

                public:
                    mm_struct_header(string name, string base_dir, uint32_t record_size,
                                     string dir_prefix = EMPTY_STRING,
                                     uint64_t block_size = MM_SIZE_SMALL,
                                     vector<string> *mounts = nullptr,
                                     bool overwrite = false) {
                        PRECONDITION(!IS_EMPTY(name));
                        PRECONDITION(!IS_EMPTY(base_dir));

                        this->base_dir = new Path(base_dir);
                        CHECK_NOT_NULL(this->base_dir);
                        this->base_dir->append(name);

                        this->name = name;

                        string name_l = common_utils::format("%s%s", MM_LOCK_PREFIX, name.c_str());
                        CREATE_LOCK_P(data_lock, &name_l, DEFAULT_RESOURCE_MODE);

                        WAIT_LOCK_GUARD(data_lock, 0);
                        try {
                            create_new_instance(block_size, record_size, dir_prefix, mounts, overwrite);
                            state.set_state(__state_enum::Available);
                        } catch (const exception &e) {
                            fs_error_base err = FS_BASE_ERROR(
                                    "Error creating struct header. [name=%s][base dir=%s][error=%s]",
                                    name.c_str(), base_dir.c_str(), e.what());
                            state.set_error(&err);
                            throw err;
                        } catch (...) {
                            fs_error_base err = FS_BASE_ERROR(
                                    "Error creating struct header. [name=%s][base dir=%s][error=%s]",
                                    name.c_str(), base_dir.c_str(), "UNKNOWN");
                            state.set_error(&err);
                            throw err;
                        }
                    }

                    mm_struct_header(string name, string base_dir) {
                        PRECONDITION(!IS_EMPTY(name));
                        PRECONDITION(!IS_EMPTY(base_dir));

                        this->base_dir = new Path(base_dir);
                        CHECK_NOT_NULL(this->base_dir);
                        this->base_dir->append(name);

                        this->name = name;

                        string name_l = common_utils::format("%s%s", MM_LOCK_PREFIX, name.c_str());
                        CREATE_LOCK_P(data_lock, &name_l, DEFAULT_RESOURCE_MODE);

                        init();

                        state.set_state(__state_enum::Available);
                    }

                    ~mm_struct_header() {
                        CHECK_AND_DISPOSE(state);
                        if (NOT_NULL(data_lock)) {
                            if (data_lock->is_locked()) {
                                data_lock->__release_lock();
                            }
                            CHECK_AND_FREE(data_lock);
                        }
                        CHECK_AND_FREE(mm_header_data);
                        CHECK_AND_FREE(mm_client_data);
                        CHECK_AND_FREE(base_dir);

                        unordered_map<uint32_t, base_block *>::const_iterator iter;
                        for (iter = block_index.begin(); iter != block_index.end(); iter++) {
                            base_block *block = iter->second;
                            CHECK_AND_FREE(block);
                        }
                        block_index.clear();
                    }

                    uint32_t get_block_count() {
                        CHECK_STATE_AVAILABLE(state);
                        return header->block_count;
                    }

                    const __mm_block_info *write(void *data, uint32_t size, uint64_t timeout = DEFAULT_LOCK_TIMEOUT) {
                        CHECK_STATE_AVAILABLE(state);

                        TRY_LOCK_WITH_ERROR(data_lock, 0, timeout);
                        try {
                            base_block *w_block = get_block(header->write_block_index);
                            uint32_t free_space = w_block->get_free_space();
                            if (free_space < size) {
                                create_new_block();
                                w_block = get_block(header->write_block_index);
                            }
                            string txid = w_block->start_transaction();
                            uint32_t i = w_block->write(data, size, txid);
                            POSTCONDITION(i >= 0);
                            w_block->commit(txid);

                            header->last_written_index = i;

                            __mm_block_info *bi = get_block_info(header->write_block_index);
                            CHECK_NOT_NULL(bi);

                            return bi;
                        } catch (const exception &e) {
                            fs_error_base err = FS_BASE_ERROR("Error writing data. [name=%s][error=%s]",
                                                              this->name.c_str(), e.what());
                            throw err;
                        } catch (...) {
                            fs_error_base err = FS_BASE_ERROR("Error writing data. [name=%s][error=%s]",
                                                              this->name.c_str(), "UNKNOWN");
                            throw err;
                        }
                    }

                    shared_read_ptr read(uint64_t index, const __mm_block_info *block_info) {
                        CHECK_STATE_AVAILABLE(state);
                        CHECK_NOT_NULL(block_info);
                        PRECONDITION(index >= 0 && index <= header->last_written_index);

                        base_block *block = get_block(block_info->block_id);
                        CHECK_NOT_NULL(block);
                        PRECONDITION(index >= block->get_start_index() && index <= block->get_last_index());

                        vector<shared_read_ptr> buff;
                        uint32_t r = block->read(index, 1, &buff);
                        if (r > 0 && buff.size() > 0) {
                            shared_read_ptr ptr = buff[0];
                            return ptr;
                        }
                        return nullptr;
                    }

                    shared_read_ptr read(uint64_t index, const uint32_t block_index) {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(block_index >= 0 && block_index < header->block_count);
                        PRECONDITION(index >= 0 && index <= header->last_written_index);

                        base_block *block = get_block(block_index);
                        CHECK_NOT_NULL(block);
                        PRECONDITION(index >= block->get_start_index() && index <= block->get_last_index());

                        vector<shared_read_ptr> buff;
                        uint32_t r = block->read(index, 1, &buff);
                        if (r > 0 && buff.size() > 0) {
                            shared_read_ptr ptr = buff[0];
                            return ptr;
                        }
                        return nullptr;
                    }

                    bool remove(uint64_t index, const uint32_t block_index) {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(block_index >= 0 && block_index < header->block_count);
                        PRECONDITION(index >= 0 && index <= header->last_written_index);

                        base_block *block = get_block(block_index);
                        CHECK_NOT_NULL(block);
                        PRECONDITION(index >= block->get_start_index() && index <= block->get_last_index());

                        string txid = block->start_transaction();
                        POSTCONDITION(!IS_EMPTY(txid));
                        bool r = block->delete_record(index, txid);
                        block->commit(txid);

                        return r;
                    }

                    bool has_record_index(uint64_t index, uint32_t block_id) {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(index >= 0 && index <= header->last_written_index);

                        base_block *block = get_block(block_id);
                        if (IS_NULL(block)) {
                            return false;
                        }

                        return (index >= block->get_start_index() && index <= block->get_last_index());
                    }

                    uint64_t get_last_index() {
                        CHECK_STATE_AVAILABLE(state);
                        return header->last_written_index;
                    }

                    bool delete_block(uint32_t block_index, uint64_t timeout = DEFAULT_LOCK_TIMEOUT) {
                        CHECK_STATE_AVAILABLE(state);
                        PRECONDITION(block_index >= 0 && block_index < header->block_count);

                        bool r = false;
                        TRY_LOCK_WITH_ERROR(data_lock, 0, timeout);
                        __mm_block_info *bi = get_block_info(block_index);
                        CHECK_NOT_NULL(bi);
                        if (!bi->deleted) {
                            bi->deleted = true;
                            if (is_block_loaded(block_index)) {
                                lock_guard<std::mutex> lock(thread_mutex);
                                base_block *block = get_block(block_index);
                                CHECK_AND_FREE(block);
                                this->block_index.erase(block_index);
                            }
                            block_utils::delete_block(bi->block_id, string(bi->filename));
                        }
                        return r;
                    }
                };

                class record_iterator {
                private:
                    uint64_t index = 0;
                    uint32_t block_index = 0;
                    mm_struct_header *header = nullptr;

                public:
                    record_iterator(mm_struct_header *header) {
                        CHECK_NOT_NULL(header);
                        this->header = header;
                    }

                    bool has_next() {
                        return (index < header->get_last_index());
                    }

                    shared_read_ptr next() {
                        PRECONDITION(index < header->get_last_index());
                        uint64_t r_index = index++;
                        while (true) {
                            if (header->has_record_index(r_index, block_index)) {
                                return header->read(r_index, block_index);
                            } else {
                                block_index++;
                            }
                        }
                        return nullptr;
                    }
                };
            }
        }
    }
}


#endif //REACTFS_MM_STRUCT_HEADER_H
