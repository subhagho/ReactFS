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
// Created by Subhabrata Ghosh on 20/12/16.
//

#ifndef REACTFS_BUFFER_UTILS_H
#define REACTFS_BUFFER_UTILS_H

#include "common.h"
#include "common_utils.h"
#include "base_error.h"

REACTFS_NS_COMMON
                class buffer_utils {
                public:

                    /*!
                     * Increment the void pointer by the specified number of bytes.
                     *
                     * @param ptr - Void pointer address.
                     * @param offset - Byte offset to increment by.
                     * @return - Incremented pointer.
                     */
                    static inline void *increment_data_ptr(void *source, uint64_t offset) {
                        if (NOT_NULL(source)) {
                            BYTE_PTR cptr = static_cast<BYTE_PTR>(source);
                            return (cptr + offset);
                        }
                        return nullptr;
                    }

                    template<typename T>
                    static inline uint64_t write(void *buffer, uint64_t *offset, T &value) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);

                        void *ptr = increment_data_ptr(buffer, *offset);
                        memcpy(ptr, &value, sizeof(T));
                        *offset += sizeof(T);

                        return sizeof(T);
                    }

                    static inline uint64_t
                    write_8str(void *buffer, uint64_t *offset, const char *value, uint8_t size) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);
                        size = ((size + 1) * sizeof(char));
                        uint64_t r_size = buffer_utils::write<uint8_t>(buffer, offset, size);
                        if (size > 0) {
                            void *ptr = increment_data_ptr(buffer, *offset);
                            memset(ptr, 0, size);
                            memcpy(ptr, value, (size - 1));
                            *offset += size;
                        }
                        return r_size + size;
                    }

                    static inline uint64_t
                    write_16str(void *buffer, uint64_t *offset, const char *value, uint16_t size) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);
                        size = ((size + 1) * sizeof(char));

                        uint64_t r_size = buffer_utils::write<uint16_t>(buffer, offset, size);
                        if (size > 0) {
                            void *ptr = increment_data_ptr(buffer, *offset);
                            memset(ptr, 0, (size + 1));
                            memcpy(ptr, value, (size - 1));
                            *offset += size;
                        }
                        return r_size + size;
                    }

                    static inline uint64_t
                    write_32str(void *buffer, uint64_t *offset, const char *value, uint32_t size) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);
                        size = ((size + 1) * sizeof(char));

                        uint64_t r_size = buffer_utils::write<uint32_t>(buffer, offset, size);
                        if (size > 0) {
                            void *ptr = increment_data_ptr(buffer, *offset);
                            memset(ptr, 0, (size + 1));
                            memcpy(ptr, value, (size - 1));
                            *offset += size;
                        }
                        return r_size + size;
                    }

                    static inline uint64_t
                    write_8str(void *buffer, uint64_t *offset, string &value) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);
                        uint8_t size = value.length();
                        size = ((size + 1) * sizeof(char));
                        uint64_t r_size = buffer_utils::write<uint8_t>(buffer, offset, size);
                        if (size > 0) {
                            void *ptr = increment_data_ptr(buffer, *offset);
                            memset(ptr, 0, (size + 1));
                            memcpy(ptr, value.c_str(), (size - 1));
                            *offset += size;
                        }
                        return r_size + size;
                    }

                    static inline uint64_t
                    write_16str(void *buffer, uint64_t *offset, string &value) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);
                        uint16_t size = value.length();
                        size = ((size + 1) * sizeof(char));

                        uint64_t r_size = buffer_utils::write<uint16_t>(buffer, offset, size);
                        if (size > 0) {
                            void *ptr = increment_data_ptr(buffer, *offset);
                            memset(ptr, 0, (size + 1));
                            memcpy(ptr, value.c_str(), (size - 1));
                            *offset += size;
                        }
                        return r_size + size;
                    }

                    static inline uint64_t
                    write_32str(void *buffer, uint64_t *offset, string &value) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);
                        uint32_t size = value.length();
                        size = ((size + 1) * sizeof(char));

                        uint64_t r_size = buffer_utils::write<uint32_t>(buffer, offset, size);
                        if (size > 0) {
                            void *ptr = increment_data_ptr(buffer, *offset);
                            memset(ptr, 0, (size + 1));
                            memcpy(ptr, value.c_str(), (size - 1));
                            *offset += size;
                        }
                        return r_size + size;
                    }

                    template<typename T>
                    static inline uint64_t read(void *buffer, uint64_t *offset, T **value) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);

                        void *ptr = increment_data_ptr(buffer, *offset);
                        *value = (T *) ptr;
                        *offset += sizeof(T);

                        return sizeof(T);
                    }

                    static inline uint64_t read_8str(void *buffer, uint64_t *offset, CHARBUFF *value) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);
                        CHECK_NOT_NULL(value);

                        uint8_t *size = nullptr;
                        uint64_t r_size = buffer_utils::read<uint8_t>(buffer, offset, &size);
                        CHECK_NOT_NULL(size);
                        POSTCONDITION(*size >= 0);
                        if (*size > 0) {
                            void *ptr = increment_data_ptr(buffer, *offset);
                            *value = (char *) ptr;
                            *offset += *size;
                        }
                        return (r_size + *size);
                    }

                    static inline uint64_t read_16str(void *buffer, uint64_t *offset, CHARBUFF *value) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);
                        CHECK_NOT_NULL(value);

                        uint16_t *size = nullptr;
                        uint64_t r_size = buffer_utils::read<uint16_t>(buffer, offset, &size);
                        CHECK_NOT_NULL(size);
                        POSTCONDITION(*size >= 0);
                        if (*size > 0) {
                            void *ptr = increment_data_ptr(buffer, *offset);
                            *value = (char *) ptr;
                            *offset += *size;
                        }
                        return (r_size + *size);
                    }

                    static inline uint64_t read_32str(void *buffer, uint64_t *offset, CHARBUFF *value) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);
                        CHECK_NOT_NULL(value);

                        uint32_t *size = nullptr;
                        uint64_t r_size = buffer_utils::read<uint32_t>(buffer, offset, &size);
                        CHECK_NOT_NULL(size);
                        POSTCONDITION(*size >= 0);
                        if (*size > 0) {
                            void *ptr = increment_data_ptr(buffer, *offset);
                            *value = (char *) ptr;
                            *offset += *size;
                        }
                        return (r_size + *size);
                    }

                    static inline uint64_t read_8str(void *buffer, uint64_t *offset, string *value) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);
                        CHECK_NOT_NULL(value);

                        uint8_t *size = nullptr;
                        uint64_t r_size = buffer_utils::read<uint8_t>(buffer, offset, &size);
                        CHECK_NOT_NULL(size);
                        POSTCONDITION(*size >= 0);
                        if (*size > 0) {
                            void *ptr = increment_data_ptr(buffer, *offset);
                            char *cp = (char *) ptr;
                            value->assign(cp, (*size - 1));
                            *offset += *size;
                        }
                        return (r_size + *size);
                    }

                    static inline uint64_t read_16str(void *buffer, uint64_t *offset, string *value) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);
                        CHECK_NOT_NULL(value);

                        uint16_t *size = nullptr;
                        uint64_t r_size = buffer_utils::read<uint16_t>(buffer, offset, &size);
                        CHECK_NOT_NULL(size);
                        POSTCONDITION(*size >= 0);
                        if (*size > 0) {
                            void *ptr = increment_data_ptr(buffer, *offset);
                            char *cp = (char *) ptr;
                            value->assign(cp, (*size - 1));
                            *offset += *size;
                        }
                        return (r_size + *size);
                    }

                    static inline uint64_t read_32str(void *buffer, uint64_t *offset, string *value) {
                        CHECK_NOT_NULL(buffer);
                        CHECK_NOT_NULL(offset);
                        PRECONDITION(*offset >= 0);
                        CHECK_NOT_NULL(value);

                        uint32_t *size = nullptr;
                        uint64_t r_size = buffer_utils::read<uint32_t>(buffer, offset, &size);
                        CHECK_NOT_NULL(size);
                        POSTCONDITION(*size >= 0);
                        if (*size > 0) {
                            void *ptr = increment_data_ptr(buffer, *offset);
                            char *cp = (char *) ptr;
                            value->assign(cp, (*size - 1));
                            *offset += *size;
                        }
                        return (r_size + *size);
                    }
                };
REACTFS_NS_COMMON_END
#endif //REACTFS_BUFFER_UTILS_H
