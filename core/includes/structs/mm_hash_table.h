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
// Created by Subhabrata Ghosh on 19/11/16.
//

#ifndef REACTFS_MM_HASH_TABLE_H
#define REACTFS_MM_HASH_TABLE_H

#include <iostream>
#include <fstream>
#include <mutex>
#include <unordered_map>
#include <sparsehash/sparse_hash_map>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/base_block.h"

#define INFO_REC_SEPERATOR ":"
#define INFO_REC_NAME "table name"
#define INFO_REC_CREATEDT "create date"
#define INFO_REC_BLOCK_SIZE "block size"
#define INFO_REC_BLOCK_REC "block file"

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;
using namespace google;

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

                    typedef struct __hash_block_index__ {
                        uint64_t block_id;
                        uint64_t record_index;
                    } __hash_block_index;

                    class mm_info {
                    private:
                        Path *info_file = nullptr;
                        uint32_t block_count = 0;
                        uint32_t block_size = 0;

                        ofstream infostream;

                        void read_info(string name) {
                            PRECONDITION(!IS_EMPTY(name));
                            ifstream info(info_file->get_path());
                            try {
                                vector<string> vb;
                                file_helper::readlines(&info, &vb, false);
                                if (IS_EMPTY(vb)) {
                                    throw BASE_ERROR("No block files specified in the info header.");
                                }
                                for (string line : vb) {
                                    if (IS_EMPTY(line)) {
                                        continue;
                                    }
                                    if (string_utils::starts_with(&line, "#")) {
                                        continue;
                                    }
                                }
                                info.close();
                            } catch (const exception &e) {
                                info.close();
                                throw BASE_ERROR("Error reading info records. [error=%s]", e.what());
                            } catch (...) {
                                info.close();
                                throw BASE_ERROR("Error reading info records. [error=UNKNOWN]");
                            }
                        }

                        void create_info_header(string name, uint32_t block_size) {
                            PRECONDITION(!IS_EMPTY(name));
                            PRECONDITION(block_size > 0);
                        }

                    public:
                        mm_info(Path *base_dir, string name, uint32_t block_size = 0) {
                            info_file = new Path(base_dir->get_path());
                            info_file->append(mm_hash_constants::INFO_FILE);

                            if (info_file->exists()) {
                                read_info(name);
                            } else {
                                create_info_header(name, block_size);
                            }
                        }

                        ~mm_info() {
                            if (infostream.is_open()) {
                                infostream.flush();
                                infostream.close();
                            }
                            CHECK_AND_FREE(info_file);
                        }
                    };

                    template<class _key, class _value>
                    class mm_hash_table {
                    private:
                        mutex thread_mutex;
                        sparse_hash_map<_key, __hash_block_index *> index;
                        unordered_map<uint64_t, base_block *> blocks;
                        string name;
                        Path *base_dir = nullptr;
                        Path *info_file = nullptr;
                        ofstream infostream;
                        uint32_t last_block_index = 0;

                        void set_base_dir(string name, string base_dir, bool overwrite) {
                            this->base_dir = new Path(base_dir);
                            this->base_dir->append(name);
                            if (this->base_dir->exists() && overwrite) {
                                this->base_dir->remove();
                            }
                            if (!this->base_dir->exists()) {
                                this->base_dir->create(0755);
                            }
                        }

                        bool check_info_file() {
                            info_file = new Path(base_dir->get_path());
                            info_file->append(mm_hash_constants::INFO_FILE);

                            return info_file->exists();
                        }

                        void add_block_info(string filename) {
                            if (!infostream.is_open()) {
                                infostream.open(info_file->get_path(), ios_base::app);
                            }
                        }

                        void create_new(uint64_t block_size, uint32_t est_record_size);

                        void load_hash_data();

                    public:
                        mm_hash_table(string name, string base_dir, uint32_t est_record_size = 0,
                                      uint64_t block_size = mm_hash_constants::SMALL,
                                      bool overwrite = false) {
                            lock_guard<std::mutex> lock(thread_mutex);
                            set_base_dir(name, base_dir, overwrite);

                            if (check_info_file()) {
                                create_new(block_size, est_record_size);
                            } else {
                                load_hash_data();
                            }
                        }

                        ~mm_hash_table() {
                            lock_guard<std::mutex> lock(thread_mutex);
                            CHECK_AND_FREE(base_dir);
                            CHECK_AND_FREE(info_file);

                            for (typename sparse_hash_map<_key, __hash_block_index *>::const_iterator it = index.begin();
                                 it != index.end(); ++it) {
                                __hash_block_index *bi = it->second;
                                CHECK_AND_FREE(bi);
                            }
                            index.clear();
                            for (unordered_map<uint64_t, base_block *>::const_iterator it = blocks.begin();
                                 it != blocks.end(); it++) {
                                base_block *block = it->second;
                                CHECK_AND_FREE(block);
                            }
                            blocks.clear();

                            if (infostream.is_open()) {
                                infostream.flush();
                                infostream.close();
                            }
                        }
                    };
                }
            }
        }
    }
}

template<class _key, class _value>
void com::wookler::reactfs::core::structs::mm_hash_table<_key, _value>::create_new(uint64_t block_size,
                                                                                   uint32_t est_record_size) {

}

#endif //REACTFS_MM_HASH_TABLE_H
