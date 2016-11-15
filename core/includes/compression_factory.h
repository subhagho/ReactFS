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

#ifndef REACTFS_COMPRESSION_FACTORY_H
#define REACTFS_COMPRESSION_FACTORY_H

#include <mutex>
#include <unordered_map>

#include "compression.h"
#include "compression_zlib.h"

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class compression_factory {
                private:
                    static mutex __lock;
                    static unordered_map<string, compression_handler *> instances;

                    static compression_handler *add_compression_handler(__compression_type type) {
                        string s_type = compression_handler::get_compression_type(type);
                        compression_handler *c = nullptr;
                        switch (type) {
                            case __compression_type::ZLIB:
                                c = new compression_zlib();
                                CHECK_NOT_NULL(c);
                                instances[s_type] = c;
                                break;
                            default:
                                throw FS_ARCHIVAL_ERROR("Compression type not supported. [type=%d]", type);
                        }
                        return c;
                    }

                public:
                    static compression_handler *get_compression_handler(__compression_type type) {
                        string s_type = compression_handler::get_compression_type(type);
                        std::lock_guard<std::mutex> guard(__lock);
                        unordered_map<string, compression_handler *>::iterator iter = instances.find(s_type);
                        if (iter != instances.end()) {
                            return iter->second;
                        }
                        return add_compression_handler(type);
                    }
                };
            }
        }
    }
}
#endif //REACTFS_COMPRESSION_FACTORY_H
