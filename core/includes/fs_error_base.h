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
#define CONST_FS_ARCH_ERROR_PREFIX "Block Archival Error : "
#define CONST_BLOCK_VALID_ERROR_PREFIX "Block Validation Error : "

#define FS_BASE_ERROR(fmt, ...) fs_error_base(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define FS_BASE_ERROR_PTR(fmt, ...) new fs_error_base(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define FS_BASE_ERROR_E(e) fs_error_base(__FILE__, __LINE__, e)
#define FS_BASE_ERROR_E_PTR(e) new fs_error_base(__FILE__, __LINE__, e)

#define FS_ARCHIVAL_ERROR(fmt, ...) fs_error_base(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define FS_ARCHIVAL_ERROR_PTR(fmt, ...) new fs_error_base(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define FS_ARCHIVAL_ERROR_E(e) fs_error_base(__FILE__, __LINE__, e)
#define FS_ARCHIVAL_ERROR_E_PTR(e) new fs_error_base(__FILE__, __LINE__, e)

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class fs_error_base : public base_error {
                public:
                    fs_error_base(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                              CONST_FS_BASE_ERROR_PREFIX,
                                                                                              mesg) {
                    }

                    fs_error_base(char const *file, const int line, const exception &e) : base_error(file, line,
                                                                                                     CONST_FS_BASE_ERROR_PREFIX,
                                                                                                     e.what()) {
                    }
                };

                class fs_archival_error : public base_error {
                public:
                    fs_archival_error(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                                  CONST_FS_ARCH_ERROR_PREFIX,
                                                                                                  mesg) {
                    }

                    fs_archival_error(char const *file, const int line, const exception &e) : base_error(file, line,
                                                                                                         CONST_FS_ARCH_ERROR_PREFIX,
                                                                                                         e.what()) {
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
