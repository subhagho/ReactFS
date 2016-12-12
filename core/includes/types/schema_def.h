
/*
 * Copyright [2016] [Subhabrata Ghosh]
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
// Created by Subhabrata Ghosh on 11/12/16.
//

#ifndef REACTFS_SCHEMA_DEF_H
#define REACTFS_SCHEMA_DEF_H

#include "common/includes/common.h"
#include "core/includes/core.h"

#include "types_common.h"
#include "__constraints.h"

REACTFS_NS_CORE
                namespace types {

                    typedef enum __field_type__ {
                        /// Field type is a native type.
                                NATIVE = 0,
                        /// Field type is a sized type.
                                SIZED = 1,
                        /// Field type is a complex type.
                                COMPLEX = 2
                    } __field_type;

                    class __field_type_helper {
                    public:
                        static uint8_t get_type_number(__field_type type) {
                            switch (type) {
                                case __field_type::NATIVE:
                                    return 0;
                                case __field_type::SIZED:
                                    return 1;
                                case __field_type::COMPLEX:
                                    return 2;
                            }
                        }

                        static __field_type get_type(uint8_t ii) {
                            if (ii == 0) {
                                return __field_type::NATIVE;
                            } else if (ii == 1) {
                                return __field_type::SIZED;
                            } else if (ii == 2) {
                                return __field_type::COMPLEX;
                            }
                            throw BASE_ERROR("Not field type defined for value. [value=%d]", ii);
                        }
                    };

                    class __native_type {
                    protected:
                        string name;
                        uint8_t index;
                        __field_type type;
                        __type_def_enum datatype;
                        __constraint *constraint = nullptr;
                        __default *default_value = nullptr;
                        __base_datatype_io *string_handler = nullptr;

                    public:
                        __native_type(const uint8_t index, const string &name, const __type_def_enum datatype) {
                            this->index = index;
                            this->name = string(name);
                            this->datatype = datatype;
                            this->type = __field_type::NATIVE;

                            string_handler = __type_defs_utils::get_type_handler(__type_def_enum::TYPE_STRING);
                            CHECK_NOT_NULL(string_handler);
                        }

                        virtual ~__native_type() {
                            CHECK_AND_FREE(constraint);
                            CHECK_AND_FREE(default_value);
                        }

                        const string get_name() const {
                            return this->name;
                        }

                        const __type_def_enum get_datatype() const {
                            return this->datatype;
                        }

                        void set_constraint(__constraint *constraint) {
                            this->constraint = constraint;
                        }

                        const __constraint *get_constraint() const {
                            return this->constraint;
                        }

                        void set_default_value(__default *default_value) {
                            this->default_value = default_value;
                        }

                        const __default *get_default_value() const {
                            return this->default_value;
                        }

                        bool is_valid_value(void *value) {
                            if (NOT_NULL(constraint)) {
                                return constraint->validate(value);
                            }
                            return true;
                        }

                        virtual uint32_t write(void *buffer, uint64_t offset) {
                            CHECK_NOT_NULL(buffer);

                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t ft = __field_type_helper::get_type_number(this->type);
                            memcpy(ptr, &ft, sizeof(uint8_t));
                            uint32_t w_size = sizeof(uint8_t);

                            ptr = common_utils::increment_data_ptr(ptr, w_size);
                            memcpy(ptr, &index, sizeof(uint8_t));
                            w_size += sizeof(uint8_t);

                            w_size += string_handler->write(buffer, &name, (offset + w_size), ULONG_MAX);
                            uint8_t bits = 0;
                            if (NOT_NULL(constraint)) {
                                bits = bitset_utils::set_uint8_bit(bits, BIT_TYPE_CONSTRAINT);
                            }
                            if (NOT_NULL(default_value)) {
                                bits = bitset_utils::set_uint8_bit(bits, BIT_TYPE_DEFAULT_VALUE);
                            }
                            ptr = common_utils::increment_data_ptr(ptr, w_size);
                            memcpy(ptr, &bits, sizeof(uint8_t));
                            w_size += sizeof(uint8_t);
                            if (NOT_NULL(constraint)) {
                                w_size += this->constraint->write(buffer, (offset + w_size));
                            }
                            if (NOT_NULL(default_value)) {
                                w_size += this->default_value->write(buffer, (offset + w_size));
                            }
                            return w_size;
                        }

                        virtual uint32_t read(void *buffer, uint64_t offset) {
                            CHECK_NOT_NULL(buffer);

                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t *ft = static_cast<uint8_t *>(ptr);
                            POSTCONDITION(*ft == __field_type_helper::get_type_number(this->type));

                            uint32_t r_size = sizeof(uint8_t);
                            ptr = common_utils::increment_data_ptr(ptr, r_size);
                            uint16_t *index = static_cast<uint16_t *>(ptr);
                            this->index = *index;
                            r_size += sizeof(uint16_t);

                            string *sp = nullptr;
                            r_size += string_handler->read(buffer, &sp, (offset + r_size), ULONG_MAX);
                            CHECK_NOT_NULL(sp);

                            this->name = string(*sp);
                            CHECK_AND_FREE(sp);

                            ptr = common_utils::increment_data_ptr(ptr, r_size);
                            uint8_t *bits = static_cast<uint8_t *>(ptr);

                            if (bitset_utils::check_uint8_bit(*bits, BIT_TYPE_CONSTRAINT)) {

                            }
                            return r_size;
                        }
                    };

                    class __sized_type : public __native_type {
                    private:
                        uint32_t max_size = 0;

                    public:
                        __sized_type(const uint8_t index, const string &name, const __type_def_enum datatype,
                                     const uint32_t max_size)
                                : __native_type(index, name, datatype) {
                            PRECONDITION(max_size > 0);
                            this->max_size = max_size;
                            this->type = __field_type::SIZED;
                        }

                        const uint32_t get_max_size() const {
                            return this->max_size;
                        }
                    };

                    class __complex_type : public __native_type {
                    private:
                        unordered_map<string, __native_type *> fields;
                    public:
                        __complex_type(const uint8_t index, const string &name) : __native_type(index, name,
                                                                                                __type_def_enum::TYPE_STRUCT) {
                            this->type = __field_type::COMPLEX;
                        }

                        ~__complex_type() {
                            unordered_map<string, __native_type *>::iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                CHECK_AND_FREE(iter->second);
                            }
                            fields.clear();
                        }

                        void add_field(string &name, __native_type *type) {
                            CHECK_NOT_EMPTY(name);
                            CHECK_NOT_NULL(type);
                            fields.insert({name, type});
                        }

                        __native_type *get_type(string &name) {
                            unordered_map<string, __native_type *>::iterator iter = fields.find(name);
                            if (iter != fields.end()) {
                                return iter->second;
                            }
                            return nullptr;
                        }
                    };

                }
REACTFS_NS_CORE_END
#endif //REACTFS_SCHEMA_DEF_H
