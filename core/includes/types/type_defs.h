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

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace types {
                    /*!
                     * Enum defines the supported data types.
                     */
                    typedef enum __type_def_enum__ {
                        /// Unknown data type.
                                TYPE_UNKNOWN,
                        /// Boolean data type.
                                TYPE_BOOL,
                        /// Basic byte data type (8-bits)
                                TYPE_BYTE,
                        /// Basic char data type (8-bits)
                                TYPE_CHAR,
                        /// Short data type (16-bits)
                                TYPE_SHORT,
                        /// Integer data type (32-bits)
                                TYPE_INTEGER,
                        /// Long data type (64-bits)
                                TYPE_LONG,
                        /// Float data type
                                TYPE_FLOAT,
                        /// Double data type
                                TYPE_DOUBLE,
                        /// Timestamp data type (LONG)
                                TYPE_TIMESTAMP,
                        /// Date/time data type.
                                TYPE_DATETIME,
                        /// String data type (max size limited)
                                TYPE_STRING,
                        /// Text string datatype (no size limit)
                                TYPE_TEXT,
                        /// Array of Basic datatypes. (max size limited)
                                TYPE_ARRAY,
                        /// List of Basic datatypes. (no size limit)
                                TYPE_LIST,
                        /// Key/Value Map of basic types.
                                TYPE_MAP,
                        /// Set of basic datatypes.
                                TYPE_SET,
                        /// A complex structure.
                                TYPE_STRUCT
                    } __type_def_enum;

                    class __type_enum_helper {
                    public:
                        static bool is_native(__type_def_enum type) {
                            switch (type) {
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_BOOL:
                                case __type_def_enum::TYPE_BYTE:
                                case __type_def_enum::TYPE_DOUBLE:
                                case __type_def_enum::TYPE_SHORT:
                                case __type_def_enum::TYPE_CHAR:
                                case __type_def_enum::TYPE_FLOAT:
                                case __type_def_enum::TYPE_INTEGER:
                                case __type_def_enum::TYPE_LONG:
                                case __type_def_enum::TYPE_STRING:
                                case __type_def_enum::TYPE_DATETIME:
                                    return true;
                                default:
                                    return false;
                            }
                        }

                        static bool is_inner_type_valid(__type_def_enum type) {
                            if (is_native(type)) {
                                return true;
                            } else if (type == __type_def_enum::TYPE_STRUCT) {
                                return true;
                            }
                            return false;
                        }
                    };

                    template<typename __T>
                    class __datatype {
                    protected:
                        __type_def_enum type = __type_def_enum::TYPE_UNKNOWN;

                        __datatype(__type_def_enum type) {
                            PRECONDITION(type != __type_def_enum::TYPE_UNKNOWN);
                            this->type = type;
                        }

                        void *get_data_ptr(void *source, uint64_t size, uint64_t offset, uint64_t max_length) {
                            CHECK_NOT_NULL(source);
                            PRECONDITION((offset + size) < max_length);
                            return common_utils::increment_data_ptr(source, offset);
                        }

                    public:

                        virtual uint64_t read(void *buffer, __T *t, uint64_t offset, uint64_t max_length, ...) =0;

                        virtual uint64_t
                        write(void *buffer, __T *value, uint64_t offset, uint64_t max_length, ...)  = 0;

                        virtual uint64_t get_size(__T *value) = 0;
                    };

                    class __dt_byte : public __datatype<uint8_t> {

                    public:
                        __dt_byte(__type_def_enum type) : __datatype(type) {
                            PRECONDITION(type == __type_def_enum::TYPE_BYTE || type == __type_def_enum::TYPE_CHAR ||
                                         type == __type_def_enum::TYPE_BOOL);
                        }

                        uint64_t read(void *buffer, uint8_t *t, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(uint8_t), offset, max_length);
                            t = (uint8_t *) ptr;
                            return sizeof(uint8_t);
                        }

                        uint64_t
                        write(void *buffer, uint8_t *value, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(uint8_t), offset, max_length);
                            memcpy(ptr, value, sizeof(uint8_t));
                            return sizeof(uint8_t);
                        }

                        uint64_t get_size(uint8_t *value) override {
                            return sizeof(uint8_t);
                        }
                    };

                    class __dt_char : public __datatype<char> {

                    public:
                        __dt_char() : __datatype(__type_def_enum::TYPE_CHAR) {

                        }

                        uint64_t read(void *buffer, char *t, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(char), offset, max_length);
                            t = (char *) ptr;
                            return sizeof(char);
                        }

                        uint64_t write(void *buffer, char *value, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(char), offset, max_length);
                            memcpy(ptr, value, sizeof(char));
                            return sizeof(char);
                        }

                        uint64_t get_size(char *value) override {
                            return sizeof(char);
                        }
                    };

                    class __dt_bool : public __datatype<bool> {

                    public:
                        __dt_bool() : __datatype(__type_def_enum::TYPE_BOOL) {

                        }

                        uint64_t read(void *buffer, bool *t, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(bool), offset, max_length);
                            t = (bool *) ptr;
                            return sizeof(bool);
                        }

                        uint64_t write(void *buffer, bool *value, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(bool), offset, max_length);
                            memcpy(ptr, value, sizeof(bool));
                            return sizeof(bool);
                        }

                        uint64_t get_size(bool *value) override {
                            return sizeof(bool);
                        }
                    };

                    class __dt_short : public __datatype<short> {

                    public:
                        __dt_short() : __datatype(__type_def_enum::TYPE_SHORT) {

                        }

                        uint64_t read(void *buffer, short *t, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(short), offset, max_length);
                            t = (short *) ptr;
                            return sizeof(short);
                        }

                        uint64_t write(void *buffer, short *value, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(short), offset, max_length);
                            memcpy(ptr, value, sizeof(short));
                            return sizeof(short);
                        }

                        uint64_t get_size(short *value) override {
                            return sizeof(short);
                        }
                    };

                    class __dt_int : public __datatype<int> {

                    public:
                        __dt_int() : __datatype(__type_def_enum::TYPE_INTEGER) {

                        }

                        uint64_t read(void *buffer, int *t, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(int), offset, max_length);
                            t = (int *) ptr;
                            return sizeof(int);
                        }

                        uint64_t write(void *buffer, int *value, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(int), offset, max_length);
                            memcpy(ptr, value, sizeof(int));
                            return sizeof(int);
                        }

                        uint64_t get_size(int *value) override {
                            return sizeof(int);
                        }
                    };

                    class __dt_long : public __datatype<long> {

                    public:
                        __dt_long() : __datatype(__type_def_enum::TYPE_LONG) {

                        }

                        uint64_t read(void *buffer, long *t, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(long), offset, max_length);
                            t = (long *) ptr;
                            return sizeof(long);
                        }

                        uint64_t write(void *buffer, long *value, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(long), offset, max_length);
                            memcpy(ptr, value, sizeof(long));
                            return sizeof(long);
                        }

                        uint64_t get_size(long *value) override {
                            return sizeof(long);
                        }
                    };

                    class __dt_timestamp : public __datatype<uint64_t> {

                    public:
                        __dt_timestamp() : __datatype(__type_def_enum::TYPE_TIMESTAMP) {

                        }

                        uint64_t read(void *buffer, uint64_t *t, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(uint64_t), offset, max_length);
                            t = (uint64_t *) ptr;
                            return sizeof(uint64_t);
                        }

                        uint64_t
                        write(void *buffer, uint64_t *value, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(uint64_t), offset, max_length);
                            memcpy(ptr, value, sizeof(uint64_t));
                            return sizeof(uint64_t);
                        }

                        uint64_t get_size(uint64_t *value) override {
                            return sizeof(uint64_t);
                        }
                    };

                    class __dt_datetime : public __dt_timestamp {

                    public:
                        __dt_datetime() {

                        }

                        uint64_t readstr(void *buffer, string *str, uint64_t offset, uint64_t max_length,
                                         const string &format = common_consts::EMPTY_STRING) {
                            uint64_t ts;
                            uint64_t r = read(buffer, &ts, offset, max_length);
                            POSTCONDITION(r == sizeof(uint64_t));
                            if (!IS_EMPTY(format))
                                str->assign(time_utils::get_time_string(ts, format));
                            else
                                str->assign(time_utils::get_time_string(ts));
                            return r;
                        }

                        uint64_t writestr(void *buffer, string *value, uint64_t offset, uint64_t max_length,
                                          const string &format = common_consts::EMPTY_STRING) {
                            CHECK_NOT_EMPTY_P(value);
                            uint64_t ts = 0;
                            if (IS_EMPTY(format))
                                ts = time_utils::parse_time(*value);
                            else
                                ts = time_utils::parse_time(*value, format);
                            return write(buffer, &ts, offset, max_length);
                        }
                    };

                    class __dt_float : public __datatype<float> {

                    public:
                        __dt_float() : __datatype(__type_def_enum::TYPE_FLOAT) {

                        }

                        uint64_t read(void *buffer, float *t, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(float), offset, max_length);
                            t = (float *) ptr;
                            return sizeof(float);
                        }

                        uint64_t write(void *buffer, float *value, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(float), offset, max_length);
                            memcpy(ptr, value, sizeof(float));
                            return sizeof(float);
                        }

                        uint64_t get_size(float *value) override {
                            return sizeof(float);
                        }
                    };

                    class __dt_double : public __datatype<double> {

                    public:
                        __dt_double() : __datatype(__type_def_enum::TYPE_DOUBLE) {

                        }

                        uint64_t read(void *buffer, double *t, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(double), offset, max_length);
                            t = (double *) ptr;
                            return sizeof(double);
                        }

                        uint64_t
                        write(void *buffer, double *value, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(double), offset, max_length);
                            memcpy(ptr, value, sizeof(double));
                            return sizeof(double);
                        }

                        uint64_t get_size(double *value) override {
                            return sizeof(double);
                        }
                    };

                    class __dt_string : public __datatype<string> {

                    public:
                        __dt_string() : __datatype(__type_def_enum::TYPE_STRING) {

                        }

                        uint64_t read(void *buffer, string *t, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(uint64_t), offset, max_length);
                            uint64_t size = 0;
                            memcpy(&size, ptr, sizeof(uint64_t));
                            if (size > 0) {
                                uint64_t d_size = (size * sizeof(char));
                                ptr = common_utils::increment_data_ptr(ptr, sizeof(uint64_t));
                                PRECONDITION((offset + d_size + sizeof(uint64_t)) < max_length);
                                char *cptr = (char *) ptr;
                                t->append(cptr, d_size);
                                return (d_size + sizeof(uint64_t));
                            }
                            return sizeof(uint64_t);
                        }

                        uint64_t
                        write(void *buffer, string *value, uint64_t offset, uint64_t max_length, ...) override {
                            void *ptr = get_data_ptr(buffer, sizeof(uint64_t), offset, max_length);
                            uint64_t size = (value->length() * sizeof(char));
                            memcpy(ptr, &size, sizeof(uint64_t));
                            if (size > 0) {
                                PRECONDITION((offset + sizeof(uint64_t) + size) < max_length);
                                ptr = common_utils::increment_data_ptr(ptr, sizeof(uint64_t));
                                memcpy(ptr, value->c_str(), value->length());
                                return (size + sizeof(uint64_t));
                            }
                            return sizeof(uint64_t);
                        }

                        uint64_t get_size(string *value) override {
                            CHECK_NOT_NULL(value);
                            return (sizeof(uint64_t) + (value->length() * sizeof(char)));
                        }
                    };

                    template<typename __T, typename __S>
                    class __dt_collection : public __datatype<__S> {
                    private:
                        __type_def_enum inner_type;

                    public:
                        __dt_collection(__type_def_enum c_type, __type_def_enum inner_type) : __datatype<__S>(
                                c_type) {
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(inner_type));
                            this->inner_type = inner_type;
                        }

                        virtual uint64_t read(void *buffer, __S *t, uint64_t offset, uint64_t max_length, ...) = 0;

                        virtual uint64_t write(void *buffer, __S *value, uint64_t offset, uint64_t max_length, ...) = 0;
                    };

                    template<typename __T>
                    class __dt_array : public __dt_collection<__T, __T *> {
                    public:
                        __dt_array(__type_def_enum inner_type) : __dt_collection<__T, __T *>(
                                __type_def_enum::TYPE_ARRAY,
                                inner_type) {

                        }

                        virtual uint64_t read(void *buffer, __T **t, uint64_t offset, uint64_t max_length, ...) {
                            return 0;
                        }

                        virtual uint64_t write(void *buffer, __T **value, uint64_t offset, uint64_t max_length, ...) {
                            return 0;
                        }
                    };

                    class __type_defs_utils {
                    private:
                        unordered_map<__type_def_enum, __datatype *> type_handlers;

                    public:

                        static string get_type_string(__type_def_enum type) {
                            switch (type) {
                                case __type_def_enum::TYPE_ARRAY:
                                    return "ARRAY";
                                case __type_def_enum::TYPE_BYTE:
                                    return "BYTE";
                                case __type_def_enum::TYPE_CHAR:
                                    return "CHAR";
                                case __type_def_enum::TYPE_DOUBLE:
                                    return "DOUBLE";
                                case __type_def_enum::TYPE_FLOAT:
                                    return "FLOAT";
                                case __type_def_enum::TYPE_INTEGER:
                                    return "INTEGER";
                                case __type_def_enum::TYPE_LIST:
                                    return "LIST";
                                case __type_def_enum::TYPE_LONG:
                                    return "LONG";
                                case __type_def_enum::TYPE_MAP:
                                    return "MAP";
                                case __type_def_enum::TYPE_SET:
                                    return "SET";
                                case __type_def_enum::TYPE_SHORT:
                                    return "SHORT";
                                case __type_def_enum::TYPE_STRING:
                                    return "STRING";
                                case __type_def_enum::TYPE_STRUCT:
                                    return "STRUCT";
                                case __type_def_enum::TYPE_TEXT:
                                    return "TEXT";
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    return "TIMESTAMP";
                                case __type_def_enum::TYPE_BOOL:
                                    return "BOOLEAN";
                                case __type_def_enum::TYPE_DATETIME:
                                    return "DATETIME";
                                default:
                                    return "EMPTY_STRING";
                            }
                        }

                        static __type_def_enum parse_type(const string &type) {
                            CHECK_NOT_EMPTY(type);
                            string t = string_utils::toupper(type);
                            if (t == get_type_string(__type_def_enum::TYPE_STRING)) {
                                return __type_def_enum::TYPE_STRING;
                            } else if (t == get_type_string(__type_def_enum::TYPE_INTEGER)) {
                                return __type_def_enum::TYPE_INTEGER;
                            } else if (t == get_type_string(__type_def_enum::TYPE_LONG)) {
                                return __type_def_enum::TYPE_LONG;
                            } else if (t == get_type_string(__type_def_enum::TYPE_DOUBLE)) {
                                return __type_def_enum::TYPE_DOUBLE;
                            } else if (t == get_type_string(__type_def_enum::TYPE_TIMESTAMP)) {
                                return __type_def_enum::TYPE_TIMESTAMP;
                            } else if (t == get_type_string(__type_def_enum::TYPE_TEXT)) {
                                return __type_def_enum::TYPE_TEXT;
                            } else if (t == get_type_string(__type_def_enum::TYPE_ARRAY)) {
                                return __type_def_enum::TYPE_ARRAY;
                            } else if (t == get_type_string(__type_def_enum::TYPE_BYTE)) {
                                return __type_def_enum::TYPE_BYTE;
                            } else if (t == get_type_string(__type_def_enum::TYPE_CHAR)) {
                                return __type_def_enum::TYPE_CHAR;
                            } else if (t == get_type_string(__type_def_enum::TYPE_FLOAT)) {
                                return __type_def_enum::TYPE_FLOAT;
                            } else if (t == get_type_string(__type_def_enum::TYPE_LIST)) {
                                return __type_def_enum::TYPE_LIST;
                            } else if (t == get_type_string(__type_def_enum::TYPE_MAP)) {
                                return __type_def_enum::TYPE_MAP;
                            } else if (t == get_type_string(__type_def_enum::TYPE_SHORT)) {
                                return __type_def_enum::TYPE_SHORT;
                            } else if (t == get_type_string(__type_def_enum::TYPE_BOOL)) {
                                return __type_def_enum::TYPE_BOOL;
                            } else if (t == get_type_string(__type_def_enum::TYPE_SET)) {
                                return __type_def_enum::TYPE_SET;
                            } else if (t == get_type_string(__type_def_enum::TYPE_STRUCT)) {
                                return __type_def_enum::TYPE_STRUCT;
                            } else if (t == get_type_string(__type_def_enum::TYPE_DATETIME)) {
                                return __type_def_enum::TYPE_DATETIME;
                            }
                            return __type_def_enum::TYPE_UNKNOWN;
                        }
                    };

                }
REACTFS_NS_CORE_END

#endif //REACTFS_TYPE_DEFS_H
