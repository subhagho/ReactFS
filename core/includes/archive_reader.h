//
// Created by Subhabrata Ghosh on 28/10/16.
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

#ifndef REACTFS_ARCHIVE_READER_H
#define REACTFS_ARCHIVE_READER_H

#include <zlib.h>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common_structs.h"
#include "fs_error_base.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class archive_reader {

                    int read_zlib_data(void *source, uint64_t in_size, void *dest, uint64_t out_size);

                    int read_lzo_data(void *source, uint64_t in_size, void *dest, uint64_t out_size);

                    int read_snappy_data(void *source, uint64_t in_size, void *dest, uint64_t out_size);

                public:
                    void
                    read_archive_data(void *source, uint64_t in_size, void *dest, uint64_t out_size,
                                      __compression_type type);
                };
            }
        }
    }
}


#endif //REACTFS_ARCHIVE_READER_H
