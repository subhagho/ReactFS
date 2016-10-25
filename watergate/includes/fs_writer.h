//
// Created by Subhabrata Ghosh on 14/10/16.
//

#ifndef WATERGATE_FS_WRITER_H
#define WATERGATE_FS_WRITER_H

#include <string>

#include "common/includes/common.h"
#include "common/includes/file_utils.h"
#include "control_def.h"
#include "init_utils.h"
#include "__fs_base.h"

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
