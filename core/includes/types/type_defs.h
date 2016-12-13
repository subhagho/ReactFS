//
// Created by Subhabrata Ghosh on 03/12/16.
//


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

#ifndef REACTFS_TYPE_DEFS_H
#define REACTFS_TYPE_DEFS_H

#include <type_traits>
#include <unordered_map>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/time_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"
#include "core/includes/types/type_errors.h"

#include "types_common.h"
#include "__constraints.h"
#include "schema_def.h"

using namespace REACTFS_NS_COMMON_PREFIX;

#define SIZE_MAX_TYPE_STRING 256

REACTFS_NS_CORE
                namespace types {

                    class __type_instance {
                    protected:
                        const __type_instance *parent = nullptr;
                        __native_type *type;
                        __base_datatype_io *handler;
                        uint8_t index;
                    public:
                        __type_instance(__native_type *type, const __type_instance *parent = nullptr) {
                            CHECK_NOT_NULL(type);
                            PRECONDITION(__type_enum_helper::is_native(type->get_datatype()));
                            this->parent = parent;
                            this->handler = __type_defs_utils::get_type_handler(type->get_datatype());
                            this->type = type;
                            this->index = type->get_index();
                        }

                        __type_instance(__native_type *type, __base_datatype_io *handler,
                                        __type_instance *parent = nullptr) {
                            CHECK_NOT_NULL(type);
                            CHECK_NOT_NULL(handler);
                            this->parent = parent;
                            this->handler = handler;
                            this->type = type;
                            this->index = type->get_index();
                        }

                        ~__type_instance() {
                            CHECK_AND_FREE(type);
                        }

                        uint8_t get_index() const {
                            return this->index;
                        }

                        __native_type *get_type() const {
                            return this->type;
                        }

                        __base_datatype_io *get_handler() const {
                            return this->handler;
                        }

                        const __type_instance *get_parent() const {
                            return this->parent;
                        }


                        string get_canonical_name() const {
                            if (IS_NULL(parent)) {
                                return type->get_name();
                            } else {
                                string p_name = parent->get_canonical_name();
                                CHECK_NOT_EMPTY(p_name);
                                return common_utils::format("%s.%s", p_name.c_str(), type->get_name().c_str());
                            }
                        }

                    };

                    class __dt_struct : public __base_datatype_io {
                    private:
                        __version_header *version;
                        vector<__type_instance *> fields;

                        void *get_field_value(unordered_map<uint8_t, void *> *map, __type_instance *type) {
                            CHECK_NOT_NULL(type);
                            unordered_map<uint8_t, void *>::iterator iter = map->find(type->get_index());
                            if (iter == map->end())
                                return nullptr;
                            return iter->second;
                        }

                    public:
                        __dt_struct(__version_header version) : __base_datatype_io(__type_def_enum::TYPE_STRUCT) {
                            this->version = (__version_header *) malloc(sizeof(__version_header));
                            CHECK_ALLOC(this->version, TYPE_NAME(__version_header));
                            this->version->major = version.major;
                            this->version->minor = version.minor;
                        }

                        ~__dt_struct() {
                            if (!IS_EMPTY(fields)) {
                                vector<__type_instance *>::iterator iter;
                                for (iter = fields.begin(); iter != fields.end(); iter++) {
                                    CHECK_AND_FREE(*iter);
                                }
                                fields.clear();
                            }
                            FREE_PTR(this->version);
                        }

                        void add_field(uint16_t index, __type_instance *type) {
                            CHECK_NOT_NULL(type);
                            if (index == fields.size()) {
                                fields.push_back(type);
                            } else if (index > fields.size()) {
                                for (uint16_t ii = fields.size(); ii <= index; ii++) {
                                    fields.push_back(nullptr);
                                }
                                fields[index] = type;
                            } else {
                                PRECONDITION(IS_NULL(fields[index]));
                                fields[index] = type;
                            }
                        }

                        bool remove_field(string name) {
                            if (!IS_EMPTY(fields)) {
                                vector<__type_instance *>::iterator iter;
                                for (iter = fields.begin(); iter != fields.end(); iter++) {
                                    __type_instance *t = *iter;
                                    if (t->get_canonical_name() == name) {
                                        break;
                                    }
                                }
                                if (iter != fields.end()) {
                                    fields.erase(iter);
                                    return true;
                                }
                            }
                            return false;
                        }

                        virtual uint64_t
                        read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            __version_header *v = static_cast<__version_header *>(ptr);
                            CHECK_NOT_NULL(v);

                            POSTCONDITION(version_utils::compatible(*(this->version), *v));
                            unordered_map<uint8_t, void *> **T = (unordered_map<uint8_t, void *> **) t;
                            *T = new unordered_map<uint8_t, void *>();

                            vector<__type_instance *>::iterator iter;
                            uint64_t r_offset = offset + sizeof(__version_header);
                            uint64_t t_size = sizeof(__version_header);
                            while (r_offset < max_length) {
                                ptr = common_utils::increment_data_ptr(buffer, r_offset);
                                uint8_t *ci = (uint8_t *) ptr;
                                POSTCONDITION(*ci < fields.size());
                                __type_instance *type = fields[*ci];
                                CHECK_NOT_NULL(type);
                                __base_datatype_io *handler = type->get_handler();
                                CHECK_NOT_NULL(handler);
                                r_offset += sizeof(uint8_t);
                                t_size += sizeof(uint8_t);
                                uint64_t r = 0;
                                void *value = nullptr;
                                if (type->get_type()->get_datatype() == __type_def_enum::TYPE_ARRAY) {
                                    __sized_type *st = static_cast<__sized_type *>(type->get_type());
                                    const uint32_t a_size = st->get_max_size();
                                    r = handler->read(buffer, &value, r_offset, max_length, a_size);
                                } else {
                                    r = handler->read(buffer, &value, r_offset, max_length);
                                }
                                (*T)->insert({type->get_index(), value});
                                t_size += r;
                                r_offset += r;
                            }
                            return t_size;
                        }

                        virtual uint64_t
                        write(void *buffer, void *value, uint64_t offset, uint64_t max_length, ...) override {
                            unordered_map<uint8_t, void *> *map = (unordered_map<uint8_t, void *> *) value;
                            CHECK_NOT_EMPTY_P(map);
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            memcpy(ptr, this->version, sizeof(__version_header));
                            uint64_t r_offset = offset + sizeof(__version_header);
                            uint64_t t_size = sizeof(__version_header);

                            vector<__type_instance *>::iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                __base_datatype_io *handler = (*iter)->get_handler();
                                CHECK_NOT_NULL(handler);
                                void *d = get_field_value(map, *iter);
                                if (!(*iter)->get_type()->is_valid_value(d)) {
                                    throw TYPE_VALID_ERROR("Field validation failed. [field=%s]",
                                                           (*iter)->get_type()->get_name().c_str());
                                }
                                __native_type *ntype = (*iter)->get_type();
                                CHECK_NOT_NULL(ntype);
                                if (IS_NULL(d)) {
                                    if (!ntype->is_nullable()) {
                                        throw TYPE_VALID_ERROR("Specified field is not nullable. [field=%s]",
                                                               (*iter)->get_canonical_name().c_str());
                                    }
                                    if (NOT_NULL(ntype->get_default_value())) {
                                        const __default *df = ntype->get_default_value();
                                        df->set_default(d);
                                    }
                                }
                                if (NOT_NULL(d)) {
                                    if (NOT_NULL(ntype->get_constraint())) {
                                        __constraint *constraint = ntype->get_constraint();
                                        if (!constraint->validate(d)) {
                                            throw TYPE_VALID_ERROR("Constraint validation failed. [field=%s]",
                                                                   (*iter)->get_canonical_name().c_str());
                                        }
                                    }
                                    uint64_t r = handler->write(buffer, d, r_offset, max_length);
                                    r_offset += r;
                                    t_size += r;
                                }
                            }
                            return t_size;
                        }

                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            unordered_map<uint8_t, void *> *map = (unordered_map<uint8_t, void *> *) data;
                            CHECK_NOT_EMPTY_P(map);
                            uint64_t t_size = sizeof(__version_header);
                            vector<__type_instance *>::iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                __base_datatype_io *handler = (*iter)->get_handler();
                                CHECK_NOT_NULL(handler);
                                void *d = get_field_value(map, *iter);
                                if (!(*iter)->get_type()->is_valid_value(d)) {
                                    throw TYPE_VALID_ERROR("Field validation failed. [field=%s]",
                                                           (*iter)->get_type()->get_name().c_str());
                                }
                                t_size += handler->compute_size(d, -1);
                            }
                            return t_size;
                        }

                        virtual bool
                        compare(const void *source, const void *target, __constraint_operator oper) override {
                            throw BASE_ERROR("Compare only supported for native types.");
                        }
                    };

                    typedef __dt_array<__dt_struct, __type_def_enum::TYPE_STRUCT> __struct_array;
                    typedef __dt_list<__dt_struct, __type_def_enum::TYPE_STRUCT> __struct_list;

                    class __array_init_utils {
                    private:
                        static unordered_map<string, __base_datatype_io *> array_handlers;

                    public:
                        static __base_datatype_io *get_handler(__type_def_enum inner_type) {
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(inner_type));
                            string type_n = __type_enum_helper::get_type_string(inner_type);
                            __base_datatype_io *handler = nullptr;
                            unordered_map<string, __base_datatype_io *>::iterator iter = array_handlers.find(type_n);
                            if (iter != array_handlers.end()) {
                                return iter->second;
                            }
                            handler = create_handler(inner_type);
                            CHECK_NOT_NULL(handler);
                            array_handlers.insert({type_n, handler});
                            return handler;
                        }

                        static __base_datatype_io *create_handler(__type_def_enum inner_type) {
                            __base_datatype_io *handler = nullptr;
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(inner_type));
                            switch (inner_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __byte_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__byte_array));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __char_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__char_array));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __short_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__short_array));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __int_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__int_array));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __long_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__long_array));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __float_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__float_array));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __double_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__double_array));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __timestamp_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__timestamp_array));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __string_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__string_array));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __text_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__text_array));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __struct_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__struct_array));
                                    break;
                            }
                            return handler;
                        }
                    };

                    class __list_init_utils {
                    private:
                        static unordered_map<string, __base_datatype_io *> list_handlers;

                    public:
                        static __base_datatype_io *get_handler(__type_def_enum inner_type) {
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(inner_type));
                            string type_n = __type_enum_helper::get_type_string(inner_type);
                            __base_datatype_io *handler = nullptr;
                            unordered_map<string, __base_datatype_io *>::iterator iter = list_handlers.find(type_n);
                            if (iter != list_handlers.end()) {
                                return iter->second;
                            }
                            handler = create_handler(inner_type);
                            CHECK_NOT_NULL(handler);
                            list_handlers.insert({type_n, handler});
                            return handler;
                        }

                        static __base_datatype_io *create_handler(__type_def_enum inner_type) {
                            __base_datatype_io *handler = nullptr;
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(inner_type));
                            switch (inner_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __byte_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__byte_list));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __char_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__char_list));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __short_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__short_list));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __int_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__int_list));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __long_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__long_list));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __float_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__float_list));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __double_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__double_list));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __timestamp_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__timestamp_list));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __string_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__string_list));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __text_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__text_list));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __struct_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__struct_list));
                                    break;
                            }
                            return handler;
                        }
                    };

                    class __map_init_utils {
                    private:
                        static unordered_map<string, __base_datatype_io *> map_handlers;

                        static string get_key(__type_def_enum key_type, __type_def_enum value_type) {
                            string key_n = __type_enum_helper::get_type_string(key_type);
                            string value_n = __type_enum_helper::get_type_string(value_type);
                            return common_utils::format("%s::%s", key_n, value_n);
                        }

                        static __base_datatype_io *create_char_map(__type_def_enum value_type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, __dt_struct, __type_def_enum::TYPE_STRUCT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                            }
                            return handler;
                        }

                    public:
                        static __base_datatype_io *get_handler(__type_def_enum key_type, __type_def_enum value_type) {
                            PRECONDITION(__type_enum_helper::is_native(key_type));
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(value_type));
                            string type_n = get_key(key_type, value_type);

                            __base_datatype_io *handler = nullptr;
                            unordered_map<string, __base_datatype_io *>::iterator iter = map_handlers.find(type_n);
                            if (iter != map_handlers.end()) {
                                return iter->second;
                            }
                            handler = create_handler(key_type, value_type);
                            CHECK_NOT_NULL(handler);
                            map_handlers.insert({type_n, handler});
                            return handler;
                        }

                        static __base_datatype_io *
                        create_handler(__type_def_enum key_type, __type_def_enum value_type) {
                            __base_datatype_io *handler = nullptr;
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(inner_type));
                            switch (inner_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __byte_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__byte_list));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __char_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__char_list));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __short_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__short_list));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __int_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__int_list));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __long_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__long_list));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __float_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__float_list));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __double_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__double_list));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __timestamp_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__timestamp_list));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __string_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__string_list));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __text_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__text_list));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __struct_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__struct_list));
                                    break;
                            }
                            return handler;
                        }
                    };
                }
REACTFS_NS_CORE_END

#endif //REACTFS_TYPE_DEFS_H
