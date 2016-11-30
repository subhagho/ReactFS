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
// Created by Subhabrata Ghosh on 25/10/16.
//

#ifndef REACTFS_FS_ERROR_BASE_H
#define REACTFS_FS_ERROR_BASE_H

#include "common/includes/common_utils.h"
#include "common/includes/base_error.h"

#define CONST_FS_BASE_ERROR_PREFIX "File System Error : "
#define CONST_FS_BLOCK_ERROR_PREFIX "File System Block Error : "
#define CONST_BLOCK_VALID_ERROR_PREFIX "Block Validation Error : "

#define FS_BASE_ERROR(fmt, ...) fs_error_base(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define FS_BASE_ERROR_PTR(fmt, ...) new fs_error_base(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))

#define FS_BASE_ERROR_E(e) fs_error_base(__FILE__, __LINE__, e)
#define FS_BASE_ERROR_E_PTR(e) new fs_error_base(__FILE__, __LINE__, e)

#define FS_BLOCK_ERROR(e, fmt, ...) fs_block_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__), e)
#define FS_BLOCK_ERROR_PTR(e, fmt, ...) new fs_block_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__), e)

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class fs_error_base : public base_error {
                private:
                    exception *error = nullptr;
                public:
                    fs_error_base(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                              CONST_FS_BASE_ERROR_PREFIX,
                                                                                              mesg) {
                    }

                    fs_error_base(char const *file, const int line, exception *e) : base_error(file, line,
                                                                                               CONST_FS_BASE_ERROR_PREFIX,
                                                                                               e->what()) {
                        error = e;
                    }

                    ~fs_error_base() {
                        CHECK_AND_FREE(error);
                    }

                    const exception *get_nested_error() {
                        return error;
                    }
                };

                class fs_block_error : public base_error {
                private:
                    int err_code = 0;
                public:
                    static const char *error_strings[];
                    static const uint16_t ERRCODE_INDEX_COPRRUPTED;
                    static const uint16_t ERRCODE_INDEX_NOT_FOUND;
                    static const uint16_t ERRCODE_INDEX_FILE_NOUT_FOUND;
                    static const uint16_t ERRCODE_BLOCK_COPRRUPTED;
                    static const uint16_t ERRCODE_BLOCK_FILE_NOT_FOUND;
                    static const uint16_t ERRCODE_RECORD_NOT_FOUND;
                    static const uint16_t ERRCODE_INDEX_DATA_VERSION;
                    static const uint16_t ERRCODE_BLOCK_DATA_VERSION;
                    static const uint16_t ERRCODE_BLOCK_COMPRESSION;
                    static const uint16_t ERRCODE_BLOCK_OUT_OF_SPACE;
                    static const uint16_t ERRCODE_ALLOCATING_BLOCK_SPACE;
                    static const uint16_t ERRCODE_MM_MAX_BLOCKS_USED;

                    fs_block_error(char const *file, const int line, string mesg, int err_code) : base_error(file, line,
                                                                                                             CONST_FS_BLOCK_ERROR_PREFIX,
                                                                                                             mesg) {
                        this->err_code = err_code;
                    }

                    fs_block_error(char const *file, const int line, const exception &e, int err_code) : base_error(
                            file, line,
                            CONST_FS_BLOCK_ERROR_PREFIX,
                            e.what()) {
                        this->err_code = err_code;
                    }

                    const char *get_error_mesg(uint16_t errcode) {
                        return error_strings[errcode];
                    }
                };

                class block_validation_error : public base_error {
                public:
                    block_validation_error(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                                       CONST_BLOCK_VALID_ERROR_PREFIX,
                                                                                                       mesg) {
                    }

                    block_validation_error(char const *file, const int line, const exception &e) : base_error(file,
                                                                                                              line,
                                                                                                              CONST_BLOCK_VALID_ERROR_PREFIX,
                                                                                                              e.what()) {
                    }
                };
            }
        }
    }
}
#endif //REACTFS_FS_ERROR_BASE_H
