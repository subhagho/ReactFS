//
// Created by Subhabrata Ghosh on 14/10/16.
//

#ifndef WATERGATE_FS_WRITER_H
#define WATERGATE_FS_WRITER_H

#include <string>

#include "includes/common/common.h"
#include "includes/common/file_utils.h"
#include "includes/core/control_def.h"
#include "includes/core/init_utils.h"
#include "includes/core/__fs_base.h"

using namespace com::watergate::core;
using namespace com::watergate::core::io;

namespace com {
    namespace watergate {
        namespace core {
            namespace io {
                class fs_writer : public __fs_base {

                public:
                    fs_writer(const string filename, int16_t priority) : __fs_base(filename, priority) {

                    }

                    size_t write(const void *ptr, size_t size, size_t nitems) {
                        return write(ptr, size, nitems, this->timeout);
                    }

                    size_t write(const void *ptr, size_t size, size_t nitems, uint64_t timeout);
                };
            }
        }
    }
}
#endif //WATERGATE_FS_WRITER_H
