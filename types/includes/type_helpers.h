
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
// Created by Subhabrata Ghosh on 15/12/16.
//

#ifndef REACTFS_TYPE_HELPERS_H
#define REACTFS_TYPE_HELPERS_H

#include "core/includes/core.h"

#include "type_defs.h"

using namespace REACTFS_NS_COMMON_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX::types;

REACTFS_NS_CORE
                namespace types {

                    class __array_init_utils {

                    public:
                        static __base_datatype_io *create_handler(__type_def_enum inner_type, __native_type *type) {
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
                                    handler = new __struct_array(type);
                                    CHECK_ALLOC(handler, TYPE_NAME(__struct_array));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", inner_type);
                            }
                            return handler;
                        }
                    };

                    class __list_init_utils {
                    public:

                        static __base_datatype_io *create_handler(__type_def_enum inner_type, __native_type *type) {
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
                                    handler = new __struct_list(type);
                                    CHECK_ALLOC(handler, TYPE_NAME(__struct_list));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", inner_type);
                            }
                            return handler;
                        }
                    };

                    class __map_init_utils {
                    private:
                        static string get_key(__type_def_enum key_type, __type_def_enum value_type) {
                            string key_n = __type_enum_helper::get_type_string(key_type);
                            string value_n = __type_enum_helper::get_type_string(value_type);
                            return common_utils::format("%s::%s", key_n.c_str(), value_n.c_str());
                        }

                        static __base_datatype_io *create_char_map(__type_def_enum value_type, __native_type *type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>(
                                            type);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_byte_map(__type_def_enum value_type, __native_type *type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>(
                                            type);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_short_map(__type_def_enum value_type, __native_type *type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>(
                                            type);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_int_map(__type_def_enum value_type, __native_type *type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>(
                                            type);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_long_map(__type_def_enum value_type, __native_type *type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>(
                                            type);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_float_map(__type_def_enum value_type, __native_type *type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>(
                                            type);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_double_map(__type_def_enum value_type, __native_type *type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>(
                                            type);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *
                        create_timestamp_map(__type_def_enum value_type, __native_type *type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>(
                                            type);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_string_map(__type_def_enum value_type, __native_type *type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>(
                                            type);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                    public:

                        static __base_datatype_io *
                        create_handler(__type_def_enum key_type, __type_def_enum value_type, __native_type *type) {
                            __base_datatype_io *handler = nullptr;
                            PRECONDITION(__type_enum_helper::is_native(key_type));
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(value_type));
                            switch (key_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = create_byte_map(value_type, type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = create_char_map(value_type, type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = create_short_map(value_type, type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = create_int_map(value_type, type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = create_long_map(value_type, type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = create_float_map(value_type, type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = create_double_map(value_type, type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = create_timestamp_map(value_type, type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = create_string_map(value_type, type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", key_type);
                            }
                            return handler;
                        }
                    };

                    class complex_type_loader_impl : public __complex_type_helper {
                    public:
                        void read(__native_type *parent, void *buffer, uint64_t offset, uint8_t count,
                                  vector<__native_type *> *fields,
                                  uint32_t *size) override {
                            uint32_t r_size = 0;
                            for (uint8_t ii = 0; ii < count; ii++) {
                                // Read the field type.
                                void *ptr = common_utils::increment_data_ptr(buffer, (offset + r_size));
                                uint8_t *ft = static_cast<uint8_t *>(ptr);
                                __field_type type = __field_type_helper::get_type(*ft);
                                if (type == __field_type::NATIVE) {
                                    __native_type *ptr = __type_init_utils::read_inner_type(parent, buffer,
                                                                                            (offset + r_size), &r_size);
                                    CHECK_NOT_NULL(ptr);
                                    fields->push_back(ptr);
                                } else if (type == __field_type::ARRAY) {
                                    __array_type *ptr = new __array_type(parent);
                                    CHECK_ALLOC(ptr, TYPE_NAME(__array_type));
                                    r_size += ptr->read(buffer, (offset + r_size));
                                    fields->push_back(ptr);
                                } else if (type == __field_type::LIST) {
                                    __list_type *ptr = new __list_type(parent);
                                    CHECK_ALLOC(ptr, TYPE_NAME(__array_type));
                                    r_size += ptr->read(buffer, (offset + r_size));
                                    fields->push_back(ptr);
                                } else if (type == __field_type::MAP) {
                                    __map_type *ptr = new __map_type(parent);
                                    CHECK_ALLOC(ptr, TYPE_NAME(__array_type));
                                    r_size += ptr->read(buffer, (offset + r_size));
                                    fields->push_back(ptr);
                                }
                            }
                            *size += r_size;
                        }
                        virtual __base_datatype_io *get_complex_type_handler(__native_type *type) override {
                            __complex_type *ct = dynamic_cast<__complex_type *>(type);
                            CHECK_CAST(ct, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));
                            __dt_struct *sh = new __dt_struct(ct);
                            CHECK_ALLOC(sh, TYPE_NAME(__dt_struct));

                            return sh;
                        }

                        virtual __base_datatype_io *
                        get_array_type_handler(__field_type field_type, __type_def_enum inner_type, __native_type *type) override  {
                            if (field_type == __field_type::LIST) {
                                string key = __type_defs_utils::create_list_key(__type_def_enum::TYPE_LIST,
                                                                                inner_type);
                                __base_datatype_io *handler = __type_defs_utils::get_type_handler(key);
                                if (IS_NULL(handler)) {
                                    if (inner_type != __type_def_enum::TYPE_STRUCT) {
                                        __base_datatype_io *ah = __list_init_utils::create_handler(inner_type, nullptr);
                                        CHECK_NOT_NULL(ah);
                                        if (!__type_defs_utils::add_external_handler(key, ah)) {
                                            CHECK_AND_FREE(ah);
                                        }
                                        handler = __type_defs_utils::get_type_handler(key);
                                    } else {
                                        CHECK_NOT_NULL(type);
                                        __base_datatype_io *ah = __list_init_utils::create_handler(inner_type, type);
                                        CHECK_NOT_NULL(ah);
                                        handler = ah;
                                    }
                                } else if (field_type == __field_type::ARRAY) {
                                    string key = __type_defs_utils::create_list_key(__type_def_enum::TYPE_ARRAY,
                                                                                    inner_type);
                                    __base_datatype_io *handler = __type_defs_utils::get_type_handler(key);
                                    if (IS_NULL(handler)) {
                                        if (inner_type != __type_def_enum::TYPE_STRUCT) {
                                            __base_datatype_io *ah = __array_init_utils::create_handler(inner_type,
                                                                                                        nullptr);
                                            CHECK_NOT_NULL(ah);
                                            if (!__type_defs_utils::add_external_handler(key, ah)) {
                                                CHECK_AND_FREE(ah);
                                            }
                                            handler = __type_defs_utils::get_type_handler(key);
                                        } else {
                                            CHECK_NOT_NULL(type);
                                            __base_datatype_io *ah = __array_init_utils::create_handler(inner_type,
                                                                                                        type);
                                            CHECK_NOT_NULL(ah);
                                            handler = ah;
                                        }
                                    }
                                }
                                return handler;
                            }
                        }

                        virtual __base_datatype_io *
                        get_map_type_handler(__type_def_enum key_type, __type_def_enum value_type,
                                             __native_type *type)  override  {

                            string key = __type_defs_utils::create_map_key(__type_def_enum::TYPE_MAP,
                                                                           key_type, value_type);
                            __base_datatype_io *handler = __type_defs_utils::get_type_handler(key);
                            if (IS_NULL(handler)) {
                                if (value_type != __type_def_enum::TYPE_STRUCT) {
                                    __base_datatype_io *ah = __map_init_utils::create_handler(key_type, value_type,
                                                                                              nullptr);
                                    CHECK_NOT_NULL(ah);
                                    if (!__type_defs_utils::add_external_handler(key, ah)) {
                                        CHECK_AND_FREE(ah);
                                    }
                                    handler = __type_defs_utils::get_type_handler(key);
                                } else {
                                    CHECK_NOT_NULL(type);
                                    __base_datatype_io *ah = __map_init_utils::create_handler(key_type, value_type,
                                                                                              nullptr);
                                    CHECK_NOT_NULL(ah);
                                    handler = ah;
                                }
                            }
                            return handler;
                        }
                    };

                    class type_field_helper {
                    public:
                        static __native_type *
                        create_field(__native_type *parent, __type_def_enum type, const string &name,
                                     const uint8_t index, ...) {
                            if (__type_enum_helper::is_native(type)) {
                                __native_type *ptr = new __native_type(parent, index, name, type);
                                CHECK_ALLOC(ptr, TYPE_NAME(__native_type));
                                return ptr;
                            } else if (type == __type_def_enum::TYPE_ARRAY) {
                                va_list vl;
                                va_start(vl, index);
                                int i_type = va_arg(vl, int);
                                __type_def_enum inner_type = __type_enum_helper::parse_type(i_type);
                                int m_size = va_arg(vl, int);
                                __native_type *nt = va_arg(vl, __native_type *);
                                CHECK_NOT_NULL(nt);
                                va_end(vl);
                                POSTCONDITION(m_size > 0);

                                __array_type *ptr = new __array_type(parent, index, name, inner_type, m_size, nt);
                                CHECK_ALLOC(ptr, TYPE_NAME(__array_type));
                                return ptr;
                            } else if (type == __type_def_enum::TYPE_LIST) {
                                va_list vl;
                                va_start(vl, index);
                                int i_type = va_arg(vl, int);
                                __type_def_enum inner_type = __type_enum_helper::parse_type(i_type);
                                __native_type *nt = va_arg(vl, __native_type *);
                                CHECK_NOT_NULL(nt);
                                va_end(vl);

                                __list_type *ptr = new __list_type(parent, index, name, inner_type, nt);
                                CHECK_ALLOC(ptr, TYPE_NAME(__list_type));
                                return ptr;
                            } else if (type == __type_def_enum::TYPE_MAP) {
                                va_list vl;
                                va_start(vl, index);
                                int k_type = va_arg(vl, int);
                                __type_def_enum key_type = __type_enum_helper::parse_type(k_type);
                                int v_type = va_arg(vl, int);
                                __type_def_enum value_type = __type_enum_helper::parse_type(v_type);
                                __native_type *nt = va_arg(vl, __native_type *);
                                CHECK_NOT_NULL(nt);
                                va_end(vl);

                                __map_type *ptr = new __map_type(parent, index, name, key_type, value_type, nt);
                                CHECK_ALLOC(ptr, TYPE_NAME(__map_type));

                                return ptr;
                            } else if (type == __type_def_enum::TYPE_STRUCT) {
                                __complex_type *ptr = new __complex_type(parent, index, name);
                                CHECK_ALLOC(ptr, TYPE_NAME(__complex_type));

                                return ptr;
                            }
                            return nullptr;
                        }
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_TYPE_HELPERS_H
