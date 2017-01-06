
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

                    class __list_init_utils {
                    public:

                        static __base_datatype_io *
                        create_handler(__type_def_enum inner_type, __native_type *type, __record_mode mode) {
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
                                    if (mode == __record_mode::RM_READ) {
                                        handler = new ___read_struct_list(type);
                                        CHECK_ALLOC(handler, TYPE_NAME(___read_struct_list));
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        handler = new ___write_struct_list(type);
                                        CHECK_ALLOC(handler, TYPE_NAME(___write_struct_list));
                                    } else {
                                        throw BASE_ERROR("Unknown record mode specified. [mode=%d]", mode);
                                    }
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

                        static __base_datatype_io *
                        create_char_map(__type_def_enum value_type, __native_type *type, __record_mode mode) {
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
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    if (mode == __record_mode::RM_READ) {
                                        handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, mutable_record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else {
                                        throw BASE_ERROR("Unknown record mode specified. [mode=%d]", mode);
                                    }
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *
                        create_byte_map(__type_def_enum value_type, __native_type *type, __record_mode mode) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_BYTE, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_BYTE, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_BYTE, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_BYTE, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_BYTE, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_BYTE, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_BYTE, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_BYTE, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_BYTE, char, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_BYTE, char, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    if (mode == __record_mode::RM_READ) {
                                        handler = new __dt_map<uint8_t, __type_def_enum::TYPE_BYTE, record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        handler = new __dt_map<uint8_t, __type_def_enum::TYPE_BYTE, mutable_record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else {
                                        throw BASE_ERROR("Unknown record mode specified. [mode=%d]", mode);
                                    }
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *
                        create_short_map(__type_def_enum value_type, __native_type *type, __record_mode mode) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_SHORT, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_SHORT, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_SHORT, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_SHORT, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_SHORT, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_SHORT, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_SHORT, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_SHORT, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_SHORT, char, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_SHORT, char, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    if (mode == __record_mode::RM_READ) {
                                        handler = new __dt_map<short, __type_def_enum::TYPE_SHORT, record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        handler = new __dt_map<short, __type_def_enum::TYPE_SHORT, mutable_record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else {
                                        throw BASE_ERROR("Unknown record mode specified. [mode=%d]", mode);
                                    }
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *
                        create_int_map(__type_def_enum value_type, __native_type *type, __record_mode mode) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_INTEGER, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_INTEGER, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_INTEGER, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_INTEGER, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_INTEGER, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_INTEGER, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_INTEGER, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_INTEGER, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_INTEGER, char, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_INTEGER, char, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    if (mode == __record_mode::RM_READ) {
                                        handler = new __dt_map<int, __type_def_enum::TYPE_INTEGER, record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        handler = new __dt_map<int, __type_def_enum::TYPE_INTEGER, mutable_record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else {
                                        throw BASE_ERROR("Unknown record mode specified. [mode=%d]", mode);
                                    }
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *
                        create_long_map(__type_def_enum value_type, __native_type *type, __record_mode mode) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_LONG, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_LONG, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_LONG, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_LONG, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_LONG, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_LONG, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_LONG, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_LONG, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_LONG, char, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_LONG, char, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    if (mode == __record_mode::RM_READ) {
                                        handler = new __dt_map<long, __type_def_enum::TYPE_LONG, record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        handler = new __dt_map<long, __type_def_enum::TYPE_LONG, mutable_record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else {
                                        throw BASE_ERROR("Unknown record mode specified. [mode=%d]", mode);
                                    }
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *
                        create_float_map(__type_def_enum value_type, __native_type *type, __record_mode mode) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_FLOAT, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_FLOAT, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_FLOAT, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_FLOAT, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_FLOAT, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_FLOAT, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_FLOAT, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_FLOAT, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_FLOAT, char, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_FLOAT, char, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    if (mode == __record_mode::RM_READ) {
                                        handler = new __dt_map<float, __type_def_enum::TYPE_FLOAT, record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        handler = new __dt_map<float, __type_def_enum::TYPE_FLOAT, mutable_record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else {
                                        throw BASE_ERROR("Unknown record mode specified. [mode=%d]", mode);
                                    }
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *
                        create_double_map(__type_def_enum value_type, __native_type *type, __record_mode mode) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_DOUBLE, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_DOUBLE, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_DOUBLE, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_DOUBLE, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_DOUBLE, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_DOUBLE, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_DOUBLE, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_DOUBLE, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_DOUBLE, char, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_DOUBLE, char, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    if (mode == __record_mode::RM_READ) {
                                        handler = new __dt_map<double, __type_def_enum::TYPE_DOUBLE, record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        handler = new __dt_map<double, __type_def_enum::TYPE_DOUBLE, mutable_record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else {
                                        throw BASE_ERROR("Unknown record mode specified. [mode=%d]", mode);
                                    }
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *
                        create_timestamp_map(__type_def_enum value_type, __native_type *type, __record_mode mode) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP, uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP, char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP, short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP, int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP, long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP, float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP, double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP, uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP, char, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP, char, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    if (mode == __record_mode::RM_READ) {
                                        handler = new __dt_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP, record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        handler = new __dt_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP, mutable_record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else {
                                        throw BASE_ERROR("Unknown record mode specified. [mode=%d]", mode);
                                    }
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *
                        create_string_map(__type_def_enum value_type, __native_type *type, __record_mode mode) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new string_key_map<uint8_t, __type_def_enum::TYPE_BYTE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new string_key_map<char, __type_def_enum::TYPE_CHAR>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new string_key_map<short, __type_def_enum::TYPE_SHORT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new string_key_map<int, __type_def_enum::TYPE_INTEGER>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new string_key_map<long, __type_def_enum::TYPE_LONG>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new string_key_map<float, __type_def_enum::TYPE_FLOAT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new string_key_map<double, __type_def_enum::TYPE_DOUBLE>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new string_key_map<uint64_t, __type_def_enum::TYPE_TIMESTAMP>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new string_key_map<char, __type_def_enum::TYPE_STRING>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new string_key_map<char, __type_def_enum::TYPE_TEXT>(
                                            nullptr);
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    if (mode == __record_mode::RM_READ) {
                                        handler = new string_key_map<record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        handler = new string_key_map<mutable_record_struct, __type_def_enum::TYPE_STRUCT>(
                                                type);
                                        CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    } else {
                                        throw BASE_ERROR("Unknown record mode specified. [mode=%d]", mode);
                                    }
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                    public:

                        static __base_datatype_io *
                        create_handler(__type_def_enum key_type, __type_def_enum value_type, __native_type *type,
                                       __record_mode mode) {
                            __base_datatype_io *handler = nullptr;
                            PRECONDITION(__type_enum_helper::is_native(key_type));
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(value_type));
                            switch (key_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = create_byte_map(value_type, type, mode);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = create_char_map(value_type, type, mode);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = create_short_map(value_type, type, mode);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = create_int_map(value_type, type, mode);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = create_long_map(value_type, type, mode);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = create_float_map(value_type, type, mode);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = create_double_map(value_type, type, mode);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = create_timestamp_map(value_type, type, mode);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = create_string_map(value_type, type, mode);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", key_type);
                            }
                            return handler;
                        }
                    };

                    class complex_type_loader_impl : public __complex_type_helper {
                    private:
                        void free_native_list(const __list_type *lt, void *value) {
                            const __native_type *it = lt->get_inner_type();
                            CHECK_NOT_NULL(it);
                            switch (it->get_datatype()) {
                                case __type_def_enum::TYPE_CHAR: {
                                    vector<char *> *v = static_cast<vector<char *> *>(value);
                                    CHECK_CAST(v, TYPE_NAME(void * ), TYPE_NAME(vector));
                                    CHECK_AND_FREE(v);
                                }
                                    break;
                                case __type_def_enum::TYPE_BYTE: {
                                    vector<uint8_t *> *v = static_cast<vector<uint8_t *> *>(value);
                                    CHECK_CAST(v, TYPE_NAME(void * ), TYPE_NAME(vector));
                                    CHECK_AND_FREE(v);
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    vector<short *> *v = static_cast<vector<short *> *>(value);
                                    CHECK_CAST(v, TYPE_NAME(void * ), TYPE_NAME(vector));
                                    CHECK_AND_FREE(v);
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    vector<int *> *v = static_cast<vector<int *> *>(value);
                                    CHECK_CAST(v, TYPE_NAME(void * ), TYPE_NAME(vector));
                                    CHECK_AND_FREE(v);
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    vector<long *> *v = static_cast<vector<long *> *>(value);
                                    CHECK_CAST(v, TYPE_NAME(void * ), TYPE_NAME(vector));
                                    CHECK_AND_FREE(v);
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    vector<float *> *v = static_cast<vector<float *> *>(value);
                                    CHECK_CAST(v, TYPE_NAME(void * ), TYPE_NAME(vector));
                                    CHECK_AND_FREE(v);
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    vector<double *> *v = static_cast<vector<double *> *>(value);
                                    CHECK_CAST(v, TYPE_NAME(void * ), TYPE_NAME(vector));
                                    CHECK_AND_FREE(v);
                                }
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    vector<uint64_t *> *v = static_cast<vector<uint64_t *> *>(value);
                                    CHECK_CAST(v, TYPE_NAME(void * ), TYPE_NAME(vector));
                                    CHECK_AND_FREE(v);
                                }
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING: {
                                    vector<char *> *v = static_cast<vector<char *> *>(value);
                                    CHECK_CAST(v, TYPE_NAME(void * ), TYPE_NAME(vector));
                                    CHECK_AND_FREE(v);
                                }
                                    break;
                                default:
                                    throw BASE_ERROR("Unsupported list value type. [type=%s]",
                                                     __type_enum_helper::get_datatype(it->get_datatype()).c_str());
                            }
                        }

                        void free_type_list(const __list_type *lt, void *value, __record_mode mode) {
                            const __native_type *it = lt->get_inner_type();
                            CHECK_NOT_NULL(it);
                            POSTCONDITION(it->get_datatype() == __type_def_enum::TYPE_STRUCT);
                            if (mode == __record_mode::RM_READ) {
                                vector<record_struct *> *v = static_cast<vector<record_struct *> *>(value);
                                CHECK_CAST(v, TYPE_NAME(void * ), TYPE_NAME(vector));
                                for (record_struct *r : *v) {
                                    free_type_node(r->get_record_type(), r, mode);
                                }
                                CHECK_AND_FREE(v);
                            } else if (mode == __record_mode::RM_READ) {
                                vector<mutable_record_struct *> *v = static_cast<vector<mutable_record_struct *> *>(value);
                                CHECK_CAST(v, TYPE_NAME(void * ), TYPE_NAME(vector));
                                for (mutable_record_struct *r : *v) {
                                    free_type_node(r->get_record_type(), r, mode);
                                }
                                CHECK_AND_FREE(v);
                            }
                        }

                        void free_type_map(const __map_type *mt, void *value, __record_mode mode) {
                            const __native_type *kt = mt->get_key_type();
                            CHECK_NOT_NULL(kt);
                            switch (kt->get_datatype()) {
                                case __type_def_enum::TYPE_BYTE: {
                                    if (mode == __record_mode::RM_READ) {
                                        unordered_map<uint8_t, record_struct *> *map = static_cast<unordered_map<uint8_t, record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        unordered_map<uint8_t, mutable_record_struct *> *map = static_cast<unordered_map<uint8_t, mutable_record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    }
                                }
                                    break;
                                case __type_def_enum::TYPE_CHAR: {
                                    if (mode == __record_mode::RM_READ) {
                                        unordered_map<char, record_struct *> *map = static_cast<unordered_map<char, record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        unordered_map<char, mutable_record_struct *> *map = static_cast<unordered_map<char, mutable_record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    }
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    if (mode == __record_mode::RM_READ) {
                                        unordered_map<short, record_struct *> *map = static_cast<unordered_map<short, record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        unordered_map<short, mutable_record_struct *> *map = static_cast<unordered_map<short, mutable_record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    }
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    if (mode == __record_mode::RM_READ) {
                                        unordered_map<int, record_struct *> *map = static_cast<unordered_map<int, record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        unordered_map<int, mutable_record_struct *> *map = static_cast<unordered_map<int, mutable_record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    }
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    if (mode == __record_mode::RM_READ) {
                                        unordered_map<long, record_struct *> *map = static_cast<unordered_map<long, record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        unordered_map<long, mutable_record_struct *> *map = static_cast<unordered_map<long, mutable_record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    }
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    if (mode == __record_mode::RM_READ) {
                                        unordered_map<float, record_struct *> *map = static_cast<unordered_map<float, record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        unordered_map<float, mutable_record_struct *> *map = static_cast<unordered_map<float, mutable_record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    }
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    if (mode == __record_mode::RM_READ) {
                                        unordered_map<double, record_struct *> *map = static_cast<unordered_map<double, record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        unordered_map<double, mutable_record_struct *> *map = static_cast<unordered_map<double, mutable_record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    }
                                }
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    if (mode == __record_mode::RM_READ) {
                                        unordered_map<uint64_t, record_struct *> *map = static_cast<unordered_map<uint64_t, record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        unordered_map<uint64_t, mutable_record_struct *> *map = static_cast<unordered_map<uint64_t, mutable_record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    }
                                }
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING: {
                                    if (mode == __record_mode::RM_READ) {
                                        unordered_map<string, record_struct *> *map = static_cast<unordered_map<string, record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    } else if (mode == __record_mode::RM_WRITE) {
                                        unordered_map<string, mutable_record_struct *> *map = static_cast<unordered_map<string, mutable_record_struct *> *>(value);
                                        CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                        for (auto iter = map->begin(); iter != map->end(); iter++) {
                                            if (NOT_NULL(iter->second)) {
                                                free_type_node(iter->second->get_record_type(), iter->second, mode);
                                            }
                                        }
                                        CHECK_AND_FREE(map);
                                    }
                                }
                                    break;
                                default:
                                    throw new BASE_ERROR("Unsupported Map key type. [type=%s]",
                                                         __type_enum_helper::get_datatype(kt->get_datatype()).c_str());
                            }
                        }

                        void free_char_map(const __map_type *mt, void *value) {
                            const __native_type *vt = mt->get_value_type();
                            CHECK_NOT_NULL(vt);
                            switch (vt->get_datatype()) {
                                case __type_def_enum::TYPE_BYTE: {
                                    unordered_map<char, uint8_t *> *map = static_cast<unordered_map<char, uint8_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_CHAR: {
                                    unordered_map<char, char *> *map = static_cast<unordered_map<char, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    unordered_map<char, short *> *map = static_cast<unordered_map<char, short *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    unordered_map<char, int *> *map = static_cast<unordered_map<char, int *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    unordered_map<char, long *> *map = static_cast<unordered_map<char, long *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    unordered_map<char, float *> *map = static_cast<unordered_map<char, float *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    unordered_map<char, double *> *map = static_cast<unordered_map<char, double *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    unordered_map<char, uint64_t *> *map = static_cast<unordered_map<char, uint64_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING: {
                                    unordered_map<char, char *> *map = static_cast<unordered_map<char, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                default:
                                    throw new BASE_ERROR("Unsupported Map key type. [type=%s]",
                                                         __type_enum_helper::get_datatype(vt->get_datatype()).c_str());
                            }
                        }

                        void free_byte_map(const __map_type *mt, void *value) {
                            const __native_type *vt = mt->get_value_type();
                            CHECK_NOT_NULL(vt);
                            switch (vt->get_datatype()) {
                                case __type_def_enum::TYPE_BYTE: {
                                    unordered_map<uint8_t, uint8_t *> *map = static_cast<unordered_map<uint8_t, uint8_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_CHAR: {
                                    unordered_map<uint8_t, char *> *map = static_cast<unordered_map<uint8_t, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    unordered_map<uint8_t, short *> *map = static_cast<unordered_map<uint8_t, short *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    unordered_map<uint8_t, int *> *map = static_cast<unordered_map<uint8_t, int *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    unordered_map<uint8_t, long *> *map = static_cast<unordered_map<uint8_t, long *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    unordered_map<uint8_t, float *> *map = static_cast<unordered_map<uint8_t, float *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    unordered_map<uint8_t, double *> *map = static_cast<unordered_map<uint8_t, double *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    unordered_map<uint8_t, uint64_t *> *map = static_cast<unordered_map<uint8_t, uint64_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING: {
                                    unordered_map<uint8_t, char *> *map = static_cast<unordered_map<uint8_t, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                default:
                                    throw new BASE_ERROR("Unsupported Map key type. [type=%s]",
                                                         __type_enum_helper::get_datatype(vt->get_datatype()).c_str());
                            }
                        }

                        void free_short_map(const __map_type *mt, void *value) {
                            const __native_type *vt = mt->get_value_type();
                            CHECK_NOT_NULL(vt);
                            switch (vt->get_datatype()) {
                                case __type_def_enum::TYPE_BYTE: {
                                    unordered_map<short, uint8_t *> *map = static_cast<unordered_map<short, uint8_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_CHAR: {
                                    unordered_map<short, char *> *map = static_cast<unordered_map<short, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    unordered_map<short, short *> *map = static_cast<unordered_map<short, short *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    unordered_map<short, int *> *map = static_cast<unordered_map<short, int *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    unordered_map<short, long *> *map = static_cast<unordered_map<short, long *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    unordered_map<short, float *> *map = static_cast<unordered_map<short, float *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    unordered_map<short, double *> *map = static_cast<unordered_map<short, double *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    unordered_map<short, uint64_t *> *map = static_cast<unordered_map<short, uint64_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING: {
                                    unordered_map<short, char *> *map = static_cast<unordered_map<short, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                default:
                                    throw new BASE_ERROR("Unsupported Map key type. [type=%s]",
                                                         __type_enum_helper::get_datatype(vt->get_datatype()).c_str());
                            }
                        }

                        void free_int_map(const __map_type *mt, void *value) {
                            const __native_type *vt = mt->get_value_type();
                            CHECK_NOT_NULL(vt);
                            switch (vt->get_datatype()) {
                                case __type_def_enum::TYPE_BYTE: {
                                    unordered_map<int, uint8_t *> *map = static_cast<unordered_map<int, uint8_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_CHAR: {
                                    unordered_map<int, char *> *map = static_cast<unordered_map<int, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    unordered_map<int, short *> *map = static_cast<unordered_map<int, short *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    unordered_map<int, int *> *map = static_cast<unordered_map<int, int *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    unordered_map<int, long *> *map = static_cast<unordered_map<int, long *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    unordered_map<int, float *> *map = static_cast<unordered_map<int, float *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    unordered_map<int, double *> *map = static_cast<unordered_map<int, double *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    unordered_map<int, uint64_t *> *map = static_cast<unordered_map<int, uint64_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING: {
                                    unordered_map<int, char *> *map = static_cast<unordered_map<int, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                default:
                                    throw new BASE_ERROR("Unsupported Map key type. [type=%s]",
                                                         __type_enum_helper::get_datatype(vt->get_datatype()).c_str());
                            }
                        }

                        void free_long_map(const __map_type *mt, void *value) {
                            const __native_type *vt = mt->get_value_type();
                            CHECK_NOT_NULL(vt);
                            switch (vt->get_datatype()) {
                                case __type_def_enum::TYPE_BYTE: {
                                    unordered_map<long, uint8_t *> *map = static_cast<unordered_map<long, uint8_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_CHAR: {
                                    unordered_map<long, char *> *map = static_cast<unordered_map<long, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    unordered_map<long, short *> *map = static_cast<unordered_map<long, short *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    unordered_map<long, int *> *map = static_cast<unordered_map<long, int *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    unordered_map<long, long *> *map = static_cast<unordered_map<long, long *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    unordered_map<long, float *> *map = static_cast<unordered_map<long, float *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    unordered_map<long, double *> *map = static_cast<unordered_map<long, double *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    unordered_map<long, uint64_t *> *map = static_cast<unordered_map<long, uint64_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING: {
                                    unordered_map<long, char *> *map = static_cast<unordered_map<long, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                default:
                                    throw new BASE_ERROR("Unsupported Map key type. [type=%s]",
                                                         __type_enum_helper::get_datatype(vt->get_datatype()).c_str());
                            }
                        }

                        void free_float_map(const __map_type *mt, void *value) {
                            const __native_type *vt = mt->get_value_type();
                            CHECK_NOT_NULL(vt);
                            switch (vt->get_datatype()) {
                                case __type_def_enum::TYPE_BYTE: {
                                    unordered_map<float, uint8_t *> *map = static_cast<unordered_map<float, uint8_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_CHAR: {
                                    unordered_map<float, char *> *map = static_cast<unordered_map<float, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    unordered_map<float, short *> *map = static_cast<unordered_map<float, short *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    unordered_map<float, int *> *map = static_cast<unordered_map<float, int *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    unordered_map<float, long *> *map = static_cast<unordered_map<float, long *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    unordered_map<float, float *> *map = static_cast<unordered_map<float, float *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    unordered_map<float, double *> *map = static_cast<unordered_map<float, double *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    unordered_map<float, uint64_t *> *map = static_cast<unordered_map<float, uint64_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING: {
                                    unordered_map<float, char *> *map = static_cast<unordered_map<float, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                default:
                                    throw new BASE_ERROR("Unsupported Map key type. [type=%s]",
                                                         __type_enum_helper::get_datatype(vt->get_datatype()).c_str());
                            }
                        }

                        void free_double_map(const __map_type *mt, void *value) {
                            const __native_type *vt = mt->get_value_type();
                            CHECK_NOT_NULL(vt);
                            switch (vt->get_datatype()) {
                                case __type_def_enum::TYPE_BYTE: {
                                    unordered_map<double, uint8_t *> *map = static_cast<unordered_map<double, uint8_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_CHAR: {
                                    unordered_map<double, char *> *map = static_cast<unordered_map<double, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    unordered_map<double, short *> *map = static_cast<unordered_map<double, short *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    unordered_map<double, int *> *map = static_cast<unordered_map<double, int *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    unordered_map<double, long *> *map = static_cast<unordered_map<double, long *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    unordered_map<double, float *> *map = static_cast<unordered_map<double, float *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    unordered_map<double, double *> *map = static_cast<unordered_map<double, double *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    unordered_map<double, uint64_t *> *map = static_cast<unordered_map<double, uint64_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING: {
                                    unordered_map<double, char *> *map = static_cast<unordered_map<double, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                default:
                                    throw new BASE_ERROR("Unsupported Map key type. [type=%s]",
                                                         __type_enum_helper::get_datatype(vt->get_datatype()).c_str());
                            }
                        }

                        void free_timestamp_map(const __map_type *mt, void *value) {
                            const __native_type *vt = mt->get_value_type();
                            CHECK_NOT_NULL(vt);
                            switch (vt->get_datatype()) {
                                case __type_def_enum::TYPE_BYTE: {
                                    unordered_map<uint64_t, uint8_t *> *map = static_cast<unordered_map<uint64_t, uint8_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_CHAR: {
                                    unordered_map<uint64_t, char *> *map = static_cast<unordered_map<uint64_t, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    unordered_map<uint64_t, short *> *map = static_cast<unordered_map<uint64_t, short *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    unordered_map<uint64_t, int *> *map = static_cast<unordered_map<uint64_t, int *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    unordered_map<uint64_t, long *> *map = static_cast<unordered_map<uint64_t, long *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    unordered_map<uint64_t, float *> *map = static_cast<unordered_map<uint64_t, float *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    unordered_map<uint64_t, double *> *map = static_cast<unordered_map<uint64_t, double *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    unordered_map<uint64_t, uint64_t *> *map = static_cast<unordered_map<uint64_t, uint64_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING: {
                                    unordered_map<uint64_t, char *> *map = static_cast<unordered_map<uint64_t, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                default:
                                    throw new BASE_ERROR("Unsupported Map key type. [type=%s]",
                                                         __type_enum_helper::get_datatype(vt->get_datatype()).c_str());
                            }
                        }

                        void free_string_map(const __map_type *mt, void *value) {
                            const __native_type *vt = mt->get_value_type();
                            CHECK_NOT_NULL(vt);
                            switch (vt->get_datatype()) {
                                case __type_def_enum::TYPE_BYTE: {
                                    unordered_map<string, uint8_t *> *map = static_cast<unordered_map<string, uint8_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_CHAR: {
                                    unordered_map<string, char *> *map = static_cast<unordered_map<string, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    unordered_map<string, short *> *map = static_cast<unordered_map<string, short *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    unordered_map<string, int *> *map = static_cast<unordered_map<string, int *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    unordered_map<string, long *> *map = static_cast<unordered_map<string, long *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    unordered_map<string, float *> *map = static_cast<unordered_map<string, float *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    unordered_map<string, double *> *map = static_cast<unordered_map<string, double *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    unordered_map<string, uint64_t *> *map = static_cast<unordered_map<string, uint64_t *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING: {
                                    unordered_map<string, char *> *map = static_cast<unordered_map<string, char *> *>(value);
                                    CHECK_CAST(map, TYPE_NAME(void * ), TYPE_NAME(unordered_map));
                                    CHECK_AND_FREE(map);
                                }
                                    break;
                                default:
                                    throw new BASE_ERROR("Unsupported Map key type. [type=%s]",
                                                         __type_enum_helper::get_datatype(vt->get_datatype()).c_str());
                            }
                        }

                        void free_native_map(const __map_type *mt, void *value) {
                            const __native_type *kt = mt->get_key_type();
                            CHECK_NOT_NULL(kt);
                            switch (kt->get_datatype()) {
                                case __type_def_enum::TYPE_BYTE:
                                    free_byte_map(mt, value);
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    free_char_map(mt, value);
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    free_short_map(mt, value);
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    free_int_map(mt, value);
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    free_long_map(mt, value);
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    free_float_map(mt, value);
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    free_double_map(mt, value);
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    free_timestamp_map(mt, value);
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING:
                                    free_string_map(mt, value);
                                    break;
                                default:
                                    throw new BASE_ERROR("Unsupported Map key type. [type=%s]",
                                                         __type_enum_helper::get_datatype(kt->get_datatype()).c_str());
                            }
                        }

                    public:
                        void read(__native_type *parent, void *buffer, uint64_t offset, uint8_t count,
                                  vector<__native_type *> *fields,
                                  uint64_t *size) override {
                            uint64_t r_size = 0;
                            for (uint8_t ii = 0; ii < count; ii++) {
                                // Read the field type.
                                void *ptr = buffer_utils::increment_data_ptr(buffer, (offset + r_size));
                                uint8_t *ft = static_cast<uint8_t *>(ptr);
                                __field_type type = __field_type_helper::get_type(*ft);
                                if (type == __field_type::NATIVE) {
                                    __native_type *ptr = __type_init_utils::read_inner_type(parent, buffer,
                                                                                            (offset + r_size),
                                                                                            &r_size);
                                    CHECK_NOT_NULL(ptr);
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
                                } else if (type == __field_type::COMPLEX) {
                                    __complex_type *ptr = new __complex_type(parent);
                                    CHECK_ALLOC(ptr, TYPE_NAME(__complex_type));
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
                        get_array_type_handler(__type_def_enum inner_type_enum,
                                               __native_type *inner_type, __record_mode
                                               mode) override {
                            string key = __type_defs_utils::create_list_key(__type_def_enum::TYPE_LIST,
                                                                            inner_type_enum);
                            __base_datatype_io *handler = __type_defs_utils::get_type_handler(key);
                            if (IS_NULL(handler)) {
                                if (inner_type_enum != __type_def_enum::TYPE_STRUCT) {
                                    __base_datatype_io *ah = __list_init_utils::create_handler(inner_type_enum,
                                                                                               nullptr,
                                                                                               mode);
                                    CHECK_NOT_NULL(ah);
                                    if (!__type_defs_utils::add_external_handler(key, ah)) {
                                        CHECK_AND_FREE(ah);
                                    }
                                    handler = __type_defs_utils::get_type_handler(key);
                                } else {
                                    CHECK_NOT_NULL(inner_type);
                                    __base_datatype_io *ah = __list_init_utils::create_handler(inner_type_enum,
                                                                                               inner_type,
                                                                                               mode);
                                    CHECK_NOT_NULL(ah);
                                    handler = ah;
                                }
                            }
                            return handler;
                        }

                        virtual __base_datatype_io *
                        get_map_type_handler(__type_def_enum key_type_enum, __type_def_enum value_type_enum,
                                             __native_type *value_type, __record_mode
                                             mode) override {

                            string key = __type_defs_utils::create_map_key(__type_def_enum::TYPE_MAP,
                                                                           key_type_enum, value_type_enum);
                            __base_datatype_io *handler = __type_defs_utils::get_type_handler(key);
                            if (IS_NULL(handler)) {
                                if (value_type_enum != __type_def_enum::TYPE_STRUCT) {
                                    __base_datatype_io *ah = __map_init_utils::create_handler(key_type_enum,
                                                                                              value_type_enum,
                                                                                              value_type, mode);
                                    CHECK_NOT_NULL(ah);
                                    if (!__type_defs_utils::add_external_handler(key, ah)) {
                                        CHECK_AND_FREE(ah);
                                    }
                                    handler = __type_defs_utils::get_type_handler(key);
                                } else {
                                    CHECK_NOT_NULL(value_type);
                                    __base_datatype_io *ah = __map_init_utils::create_handler(key_type_enum,
                                                                                              value_type_enum,
                                                                                              value_type, mode);
                                    CHECK_NOT_NULL(ah);
                                    handler = ah;
                                }
                            }
                            return handler;
                        }

                        virtual void
                        free_type_node(const __native_type *type, void *node, __record_mode mode) override {
                            if (IS_NULL(node)) {
                                return;
                            }
                            if (type->get_datatype() == __type_def_enum::TYPE_STRUCT) {
                                if (mode == __record_mode::RM_WRITE) {
                                    mutable_record_struct *st = static_cast<mutable_record_struct *>(node);
                                    CHECK_CAST(st, TYPE_NAME(void * ), TYPE_NAME(mutable_record_struct));
                                    CHECK_AND_FREE(st);
                                } else if (mode == __record_mode::RM_READ) {
                                    record_struct *st = static_cast<record_struct *>(node);
                                    CHECK_CAST(st, TYPE_NAME(void * ), TYPE_NAME(record_struct));
                                    CHECK_AND_FREE(st);
                                }
                            } else if (type->get_type() == __field_type::LIST) {
                                const __list_type *lt = dynamic_cast<const __list_type *>(type);
                                CHECK_CAST(lt, TYPE_NAME(__native_type), TYPE_NAME(__list_type));
                                if (lt->get_inner_type()->get_datatype() == __type_def_enum::TYPE_STRUCT) {
                                    free_type_list(lt, node, mode);
                                } else {
                                    // free_native_list(lt, node);
                                }
                            } else if (type->get_type() == __field_type::MAP) {
                                const __map_type *mt = dynamic_cast<const __map_type *>(type);
                                CHECK_CAST(mt, TYPE_NAME(__native_type), TYPE_NAME(__map_type));
                                if (mt->get_value_type()->get_datatype() == __type_def_enum::TYPE_STRUCT) {
                                    free_type_map(mt, node, mode);
                                } else {
                                    // free_native_map(mt, node);
                                }
                            }
                        }
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_TYPE_HELPERS_H
