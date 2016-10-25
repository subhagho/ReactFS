//
// Created by Subhabrata Ghosh on 25/10/16.
//

#ifndef REACTFS_FS_ERROR_BASE_H
#define REACTFS_FS_ERROR_BASE_H

#include "common/includes/common_utils.h"
#include "common/includes/base_error.h"

#define CONST_FS_BASE_ERROR_PREFIX "File System Error : "

#define FS_BASE_ERROR(fmt, ...) fs_error_base(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define FS_BASE_ERROR_PTR(fmt, ...) new fs_error_base(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define FS_BASE_ERROR_E(e) fs_error_base(__FILE__, __LINE__, e)
#define FS_BASE_ERROR_E_PTR(e) new fs_error_base(__FILE__, __LINE__, e)

using namespace com::watergate::common;

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
            }
        }
    }
}
#endif //REACTFS_FS_ERROR_BASE_H
