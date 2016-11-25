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
// Created by Subhabrata Ghosh on 28/10/16.
//

#ifndef REACTFS_COMMON_STRUCTS_H
#define REACTFS_COMMON_STRUCTS_H

#include "common/includes/common.h"
#include "common/includes/base_error.h"
#include "block_common_structs_v0.h"

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                typedef __compression_v0 __compression;
                typedef __encryption_v0 __encryption;
                typedef __record_header_v0 __record_header;
                typedef __record_v0 __record;
                typedef __record_index_header_v0 __record_index_header;
                typedef __record_index_ptr_v0 __record_index_ptr;
                typedef __rollback_info_v0 __rollback_info;
                typedef __block_header_v0 __block_header;

                class __mapped_ptr {
                private:
                    uint32_t size = 0;
                    void *data_ptr = nullptr;
                public:
                    void set_data_ptr(void *data_ptr, uint32_t size) {
                        PRECONDITION(NOT_NULL(data_ptr));

                        this->data_ptr = data_ptr;
                        this->size = size;
                    }

                    void *get_data_ptr() {
                        CHECK_NOT_NULL(this->data_ptr);
                        return this->data_ptr;
                    }

                    uint32_t get_size() {
                        return this->size;
                    }
                };

                class __read_ptr {
                private:
                    bool allocated = false;
                    uint32_t size = 0;
                    void *data_ptr = nullptr;
                    const void *const_ptr = nullptr;
                public:
                    __read_ptr(uint32_t size) {
                        this->size = size;
                    }

                    ~__read_ptr() {
                        if (allocated) {
                            FREE_PTR(data_ptr);
                        }
                        data_ptr = nullptr;
                    }

                    void set_data_ptr(const void *data_ptr, uint32_t size) {
                        PRECONDITION(NOT_NULL(data_ptr));

                        this->const_ptr = data_ptr;
                        this->size = size;
                    }

                    void copy(const void *data_ptr, uint32_t size = 0) {
                        PRECONDITION(NOT_NULL(data_ptr));
                        if (allocated) {
                            FREE_PTR(this->data_ptr);
                        }
                        if (size == 0) {
                            size = this->size;
                        }
                        PRECONDITION(size > 0);

                        this->data_ptr = (BYTE_PTR) malloc(size * sizeof(BYTE_PTR));
                        CHECK_NOT_NULL(this->data_ptr);

                        allocated = true;
                        memcpy(this->data_ptr, data_ptr, size);
                    }

                    const void *get_data_ptr() {
                        if (allocated)
                            return this->data_ptr;
                        else
                            return this->const_ptr;
                    }

                    uint32_t get_size() {
                        return this->size;
                    }
                };

                typedef shared_ptr<__read_ptr> shared_read_ptr;
                typedef shared_ptr<__mapped_ptr> shared_mapped_ptr;
            }
        }
    }
}
#endif //REACTFS_COMMON_STRUCTS_H
