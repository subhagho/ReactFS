//
// Created by Subhabrata Ghosh on 15/11/16.
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

#ifndef REACTFS_COMPRESSION_ZLIB_H
#define REACTFS_COMPRESSION_ZLIB_H

#include <zlib.h>

#include "compression.h"

#define COMPRESS_BLOCK_SIZE 128 * 1024

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {

                class compression_zlib : public compression_handler {

                public:
                    compression_zlib() : compression_handler(__compression_type::ZLIB) {

                    }

                    int read_archive_data(void *source, uint64_t in_size, temp_buffer *buffer) override;

                    void write_archive_data(void *source, uint64_t in_size, temp_buffer *buffer) override;
                };
            }
        }
    }
}

#endif //REACTFS_COMPRESSION_ZLIB_H
