//
// Created by Subhabrata Ghosh on 04/11/16.
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

#ifndef REACTFS_ARCHIVE_WRITER_H
#define REACTFS_ARCHIVE_WRITER_H

#include <zlib.h>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common_structs.h"
#include "fs_error_base.h"

typedef struct __compressed_data__ {
    uint64_t size;
    uint8_t *data = nullptr;
} __compressed_data;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class archive_writer {
                private:
                    __compressed_data *write_gzip_data(void *source, uint64_t in_size);

                    __compressed_data *write_zlib_data(void *source, uint64_t in_size);

                    __compressed_data *write_lzo_data(void *source, uint64_t in_size);

                    __compressed_data *write_snappy_data(void *source, uint64_t in_size);

                public:
                    __compressed_data *
                    write_archive_data(void *source, uint64_t in_size,
                                       __compression_type type);
                };
            }
        }
    }
}

#endif //REACTFS_ARCHIVE_WRITER_H
