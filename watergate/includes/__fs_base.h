/* 
 * Copyright [2016] [Subhabrata Ghosh] 
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 *       limitations under the License. 
 * 
 */
//
// Created by Subhabrata Ghosh on 10/10/16.
//

#ifndef WATERGATE_FS_BASE_H
#define WATERGATE_FS_BASE_H

#include <stdio.h>

#include "common/includes/common.h"
#include "common/includes/file_utils.h"
#include "control_def.h"
#include "init_utils.h"

using namespace com::wookler::watergate::core;

namespace com {
    namespace wookler {
        namespace watergate {
            namespace core {
                namespace io {
                    class __fs_base {
                    protected:
                        __state__ state;

                        Path *path = nullptr;
                        FILE *f_ptr = nullptr;
                        string lock_name;
                        uint64_t quota_max = 0;
                        int16_t priority = 0;
                        uint64_t timeout = 0;

                    public:
                        __fs_base(const string filename, int16_t priority) {
                            CHECK_NOT_EMPTY(filename);
                            path = new Path(filename);
                            this->priority = priority;
                        }

                        virtual ~__fs_base() {
                            state.set_state(Disposed);
                            if (NOT_NULL(f_ptr)) {
                                if (fclose(f_ptr) != 0) {
                                    LOG_ERROR("Error closing file handle. [path=%s][error=%s]",
                                              path->get_path().c_str(),
                                              strerror(errno));
                                }
                                f_ptr = nullptr;
                            }
                            CHECK_AND_FREE(path);
                        }

                        void open(const string mode) {
                            CHECK_NOT_EMPTY(mode);

                            f_ptr = fopen(path->get_path().c_str(), mode.c_str());
                            if (IS_NULL(f_ptr)) {
                                base_error e = BASE_ERROR("Error opening file. [path=%s][mode=%s][error=%s]",
                                                          path->get_path().c_str(), mode.c_str(), strerror(errno));
                                state.set_error(&e);

                                throw e;
                            }

                            const control_client *client = init_utils::get_client();
                            CHECK_NOT_NULL(client);

                            lock_name = client->find_lock(path->get_path(), FS);
                            CHECK_NOT_EMPTY(lock_name);

                            quota_max = client->get_quota(lock_name);

                            state.set_state(Available);
                        }

                        void set_priority(int16_t priority) {
                            this->priority = priority;
                        }

                        int16_t get_priority() {
                            return this->priority;
                        }

                        void set_timeout(uint64_t timeout) {
                            this->timeout = timeout;
                        }

                        uint64_t get_timeout() {
                            return this->timeout;
                        }

                        fpos_t get_position() {
                            CHECK_STATE_AVAILABLE(state);

                            fpos_t pos = (fpos_t)0;
                            if (fgetpos(f_ptr, &pos) != 0) {
                                throw BASE_ERROR("Error getting file position. [file=%s][error=%s]",
                                                 path->get_path().c_str(), strerror(errno));
                            }
                            return pos;
                        }

                        void set_position(fpos_t pos) {
                            CHECK_STATE_AVAILABLE(state);

                            if (fsetpos(f_ptr, &pos) != 0) {
                                throw BASE_ERROR("Error setting to position. [file=%s][error=%s]",
                                                 path->get_path().c_str(), strerror(errno));
                            }
                        }

                        void seek(int whence, long offset) {
                            CHECK_STATE_AVAILABLE(state);

                            if (fseek(f_ptr, offset, whence) != 0) {
                                throw BASE_ERROR("Error seeking to position. [file=%s][error=%s]",
                                                 path->get_path().c_str(), strerror(errno));
                            }
                        }

                        void rewind() {
                            CHECK_STATE_AVAILABLE(state);

                            set_position((fpos_t) 0);
                        }

                        long get_file_size() {
                            CHECK_STATE_AVAILABLE(state);

                            if (path->exists()) {
                                struct stat stat_buf;

                                int rc = stat(path->get_path().c_str(), &stat_buf);
                                return rc == 0 ? stat_buf.st_size : -1;
                            }
                            return -1;
                        }
                    };
                }
            }
        }
    }
}

#endif //WATERGATE_FS_BASE_H
