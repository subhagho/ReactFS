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
// Created by Subhabrata Ghosh on 15/11/16.
//

#ifndef REACTFS_COMPRESSION_H
#define REACTFS_COMPRESSION_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "fs_error_base.h"
#include "common_structs.h"

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {

                class compression_handler {
                private:
                    __compression_type type;

                protected:
                    compression_handler(__compression_type type) {
                        this->type = type;
                    }

                public:
                    __compression_type get_type() {
                        return this->type;
                    }

                    virtual int read_archive_data(void *source, uint64_t in_size, temp_buffer *buffer) = 0;

                    virtual void write_archive_data(void *source, uint64_t in_size, temp_buffer *buffer) = 0;

                    static string get_compression_ext(__compression_type type) {
                        switch (type) {
                            case __compression_type::GZIP:
                                return ".gz";
                            case __compression_type::LZO:
                                return ".lz";
                            case __compression_type::SNAPPY:
                                return ".snappy";
                            case __compression_type::ZLIB:
                                return ".z";
                            default:
                                break;
                        }
                        return EMPTY_STRING;
                    }

                    static string get_compression_type(__compression_type type) {
                        switch (type) {
                            case __compression_type::GZIP:
                                return "GZIP";
                            case __compression_type::LZO:
                                return "LZO";
                            case __compression_type::SNAPPY:
                                return "SNAPPY";
                            case __compression_type::ZLIB:
                                return "ZLIB";
                            default:
                                break;
                        }
                        return EMPTY_STRING;
                    }
                };
            }
        }
    }
}
#endif //REACTFS_COMPRESSION_H
