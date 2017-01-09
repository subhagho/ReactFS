
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
// Created by Subhabrata Ghosh on 12/12/16.
//

#ifndef REACTFS_TYPES_COMMON_H
#define REACTFS_TYPES_COMMON_H

#include <unordered_map>
#include <mutex>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/time_utils.h"
#include "common/includes/base_error.h"
#include "common/includes/buffer_utils.h"
#include "common/includes/log_utils.h"
#include "core/includes/core.h"

#define SIZE_MAX_TYPE_STRING 256

#define BIT_TYPE_CONSTRAINT 1
#define BIT_TYPE_DEFAULT_VALUE 2

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace types {
                    typedef struct __pos__ {
                        uint64_t offset = 0;
                        uint64_t size = 0;
                    } __pos;

                    /*!
                     * Enum defines the supported data types.
                     */
                    typedef enum __type_def_enum__ {
                        /// Unknown data type.
                                TYPE_UNKNOWN = 0,
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
                        /// List of Basic datatypes. (no size limit)
                                TYPE_LIST,
                        /// Key/Value Map of basic types.
                                TYPE_MAP,
                        /// A complex structure.
                                TYPE_STRUCT
                    } __type_def_enum;

                    /*!
                     * Helper class for datatype enums.
                     */
                    class __type_enum_helper {
                    public:
                        static string get_string_value(const void *value, __type_def_enum type) {
                            switch (type) {
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    uint64_t *v = (uint64_t *) value;
                                    return to_string(*v);
                                }
                                    break;
                                case __type_def_enum::TYPE_BOOL: {
                                    bool *v = (bool *) value;
                                    if (*v) {
                                        return BOOL_TRUE;
                                    } else {
                                        return BOOL_FALSE;
                                    }
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    double *v = (double *) value;
                                    return to_string(*v);
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    short *v = (short *) value;
                                    return to_string(*v);
                                }
                                    break;
                                case __type_def_enum::TYPE_CHAR: {
                                    char *v = (char *) value;
                                    return to_string(*v);
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    float *v = (float *) value;
                                    return to_string(*v);
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    int *v = (int *) value;
                                    return to_string(*v);
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    long *v = (long *) value;
                                    return to_string(*v);
                                }
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                case __type_def_enum::TYPE_STRING: {
                                    string *v = (string *) value;
                                    return *v;
                                }
                                    break;
                                default:
                                    break;
                            }
                            return common_consts::EMPTY_STRING;
                        }

                        /*!
                         * Check if the type enum represents a native type.
                         *
                         * Note: String is also considered a native type.
                         *
                         * @param type - Data type enum to check
                         * @return - Is native type?
                         */
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

                        /*!
                         * Check if the specified type enum represents a type that can be used as an
                         * inner type for lists and maps.
                         *
                         * @param type - Data type enum to check
                         * @return - Can be inner type?
                         */
                        static bool is_inner_type_valid(__type_def_enum type) {
                            if (is_native(type)) {
                                return true;
                            } else if (type == __type_def_enum::TYPE_STRUCT || type == __type_def_enum::TYPE_TEXT) {
                                return true;
                            }
                            return false;
                        }

                        /*!
                         * Get the string value of the datatype enum.
                         *
                         * @param type - Datatype enum.
                         * @return - String value of the enum.
                         */
                        static string get_datatype(__type_def_enum type) {
                            switch (type) {
                                case __type_def_enum::TYPE_BYTE:
                                    return "byte";
                                case __type_def_enum::TYPE_CHAR:
                                    return "char";
                                case __type_def_enum::TYPE_DOUBLE:
                                    return "double";
                                case __type_def_enum::TYPE_FLOAT:
                                    return "float";
                                case __type_def_enum::TYPE_INTEGER:
                                    return "int";
                                case __type_def_enum::TYPE_LIST:
                                    return "vector";
                                case __type_def_enum::TYPE_LONG:
                                    return "long";
                                case __type_def_enum::TYPE_MAP:
                                    return "unordered_map";
                                case __type_def_enum::TYPE_SHORT:
                                    return "short";
                                case __type_def_enum::TYPE_STRING:
                                case __type_def_enum::TYPE_TEXT:
                                    return "char";
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    return "uint64_t";
                                case __type_def_enum::TYPE_BOOL:
                                    return "bool";
                                default:
                                    return "unkown";
                            }
                        }

                        /*!
                         * Get the string value of the datatype enum.
                         *
                         * @param type - Datatype enum.
                         * @return - String value of the enum.
                         */
                        static string get_type_string(__type_def_enum type) {
                            switch (type) {
                                case __type_def_enum::TYPE_BYTE:
                                    return "byte";
                                case __type_def_enum::TYPE_CHAR:
                                    return "char";
                                case __type_def_enum::TYPE_DOUBLE:
                                    return "double";
                                case __type_def_enum::TYPE_FLOAT:
                                    return "float";
                                case __type_def_enum::TYPE_INTEGER:
                                    return "integer";
                                case __type_def_enum::TYPE_LIST:
                                    return "list";
                                case __type_def_enum::TYPE_LONG:
                                    return "long";
                                case __type_def_enum::TYPE_MAP:
                                    return "map";
                                case __type_def_enum::TYPE_SHORT:
                                    return "short";
                                case __type_def_enum::TYPE_STRING:
                                    return "string";
                                case __type_def_enum::TYPE_STRUCT:
                                    return "type";
                                case __type_def_enum::TYPE_TEXT:
                                    return "text";
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    return "timestamp";
                                case __type_def_enum::TYPE_BOOL:
                                    return "boolean";
                                case __type_def_enum::TYPE_DATETIME:
                                    return "datetime";
                                default:
                                    return "unkown";
                            }
                        }

                        /*!
                         * Parse the string value as a datatype enum. The parse is case-insensitive.
                         *
                         * @param type - String value of enum.
                         * @return - Parsed datatype enum.
                         */
                        static __type_def_enum parse_type(const string &type) {
                            CHECK_NOT_EMPTY(type);
                            string t = string_utils::tolower(type);
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
                            } else if (t == get_type_string(__type_def_enum::TYPE_STRUCT)) {
                                return __type_def_enum::TYPE_STRUCT;
                            } else if (t == get_type_string(__type_def_enum::TYPE_DATETIME)) {
                                return __type_def_enum::TYPE_DATETIME;
                            }
                            return __type_def_enum::TYPE_UNKNOWN;
                        }

                        /*!
                         * Parse the numeric value as a datatype enum.
                         *
                         * @param type - Numeric value of enum.
                         * @return - Parsed datatype enum.
                         */
                        static __type_def_enum parse_type(const uint8_t &t) {
                            if (t == __type_def_enum::TYPE_STRING) {
                                return __type_def_enum::TYPE_STRING;
                            } else if (t == __type_def_enum::TYPE_INTEGER) {
                                return __type_def_enum::TYPE_INTEGER;
                            } else if (t == __type_def_enum::TYPE_LONG) {
                                return __type_def_enum::TYPE_LONG;
                            } else if (t == __type_def_enum::TYPE_DOUBLE) {
                                return __type_def_enum::TYPE_DOUBLE;
                            } else if (t == __type_def_enum::TYPE_TIMESTAMP) {
                                return __type_def_enum::TYPE_TIMESTAMP;
                            } else if (t == __type_def_enum::TYPE_TEXT) {
                                return __type_def_enum::TYPE_TEXT;
                            } else if (t == __type_def_enum::TYPE_BYTE) {
                                return __type_def_enum::TYPE_BYTE;
                            } else if (t == __type_def_enum::TYPE_CHAR) {
                                return __type_def_enum::TYPE_CHAR;
                            } else if (t == __type_def_enum::TYPE_FLOAT) {
                                return __type_def_enum::TYPE_FLOAT;
                            } else if (t == __type_def_enum::TYPE_LIST) {
                                return __type_def_enum::TYPE_LIST;
                            } else if (t == __type_def_enum::TYPE_MAP) {
                                return __type_def_enum::TYPE_MAP;
                            } else if (t == __type_def_enum::TYPE_SHORT) {
                                return __type_def_enum::TYPE_SHORT;
                            } else if (t == __type_def_enum::TYPE_BOOL) {
                                return __type_def_enum::TYPE_BOOL;
                            } else if (t == __type_def_enum::TYPE_STRUCT) {
                                return __type_def_enum::TYPE_STRUCT;
                            } else if (t == __type_def_enum::TYPE_DATETIME) {
                                return __type_def_enum::TYPE_DATETIME;
                            }
                            return __type_def_enum::TYPE_UNKNOWN;
                        }
                    };


                    typedef enum __constraint_operator__ {
                        /// Greater than operator
                                GT = 0,
                        /// Greater than/equals operator
                                GTEQ,
                        /// Less than operator
                                LT,
                        /// Less than/equals operator
                                LTEQ,
                        /// Equals operator
                                EQ
                    } __constraint_operator;

                    /*!
                     * Base class for defining data type handlers. Handlers implement the interfaces
                     * defined to read/write data into binary formats.
                     *
                     * The interfaces defined are type unsafe, as they use (void *) pointers. Callers must be careful
                     * to instantiate the correct datatype pointers before calling these methods.
                     */
                    class __base_datatype_io {
                    protected:
                        /// Data type this type handler manages.
                        __type_def_enum type = __type_def_enum::TYPE_UNKNOWN;

                    public:
                        /*!
                         * Get the data time enum of this handler.
                         *
                         * @return - Data type enum.
                         */
                        __type_def_enum get_type() {
                            return this->type;
                        }

                        virtual ~__base_datatype_io() {

                        }

                        /*!
                         * Read (de-serialize) data from the binary format for the represented data type.
                         *
                         * @param buffer - Source data buffer (binary data)
                         * @param t - Pointer to map the output data to.
                         * @param offset - Start offset where the buffer is to be read from.
                         * @param max_length - Max length of the data in the buffer.
                         * @return - Total bytes consumed by this read.
                         */
                        virtual uint64_t read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) =0;

                        /*!
                         * Write (serialize) data for the represented data type to the binary output buffer.
                         *
                         * @param buffer - Output data buffer the data is to be copied to.
                         * @param value - Data value pointer to copy from.
                         * @param offset - Offset in the output buffer to start writing from.
                         * @param max_length - Max lenght of the output buffer.
                         * @return - Total number of bytes written.
                         */
                        virtual uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...)  = 0;

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type.
                         * @return - Return storage size.
                         */
                        virtual uint64_t compute_size(const void *data, int size) = 0;

                        /*!
                         * Estimate the storage size of the given type value.
                         *
                         * @return - Return estimated storage size.
                         */
                        virtual uint32_t estimate_size() = 0;

                        /*!
                         * Compare the source and target base on the specified operator.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool compare(const void *target, void *source, __constraint_operator oper) = 0;

                        virtual void print(const void *value) const = 0;
                    };

                    /*!
                     * Typed template for representing native type handlers.
                     *
                     *
                     * @tparam __T - supported data type.
                     */
                    template<typename __T>
                    class __datatype_io : public __base_datatype_io {
                    protected:

                        /*!<constructor
                         * Base constructor for native type handlers.
                         *
                         * @param type - Data type enum.
                         */
                        __datatype_io(__type_def_enum type) {
                            PRECONDITION(type != __type_def_enum::TYPE_UNKNOWN);
                            this->type = type;
                        }

                        /*!
                         * Check and increment the buffer pointer.
                         *
                         * @param source - Source/Target data buffer.
                         * @param size - Size of the requested bytes.
                         * @param offset - Start offset in the data buffer.
                         * @param max_length - Max length of the data buffer.
                         * @return - Void pointer pointing to the incremented offset.
                         */
                        void *get_data_ptr(void *source, uint64_t size, uint64_t offset, uint64_t max_length) {
                            CHECK_NOT_NULL(source);
                            PRECONDITION(size <= max_length);
                            return buffer_utils::increment_data_ptr(source, offset);
                        }

                    public:

                        /*!
                         * Read (de-serialize) data from the binary format for the represented native data type.
                         *
                         * @param buffer - Source data buffer (binary data)
                         * @param t - Pointer to map the output data to.
                         * @param offset - Start offset where the buffer is to be read from.
                         * @param max_length - Max length of the data in the buffer.
                         * @return - Total bytes consumed by this read.
                         */
                        virtual uint64_t
                        read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override =0;

                        /*!
                         * Write (serialize) data for the represented native data type to the binary output buffer.
                         *
                         * @param buffer - Output data buffer the data is to be copied to.
                         * @param value - Data value pointer to copy from.
                         * @param offset - Offset in the output buffer to start writing from.
                         * @param max_length - Max lenght of the output buffer.
                         * @return - Total number of bytes written.
                         */
                        virtual uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override = 0;

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type.
                         * @return - Return storage size.
                         */
                        virtual uint64_t compute_size(const void *data, int size) override = 0;

                        /*!
                         * Compare the source and target base on the specified operator.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *target, void *source, __constraint_operator oper) override = 0;

                        /*!
                         * Estimate the storage size of the given type value.
                         *
                         * @return - Return estimated storage size.
                         */
                        virtual uint32_t estimate_size() override {
                            return sizeof(__T);
                        }

                    };


                    /*!
                     * Data handler for single byte data type. (uint8_t)
                     */
                    class __dt_byte : public __datatype_io<uint8_t> {

                    public:
                        /*!<constructor
                         *
                         * Default construtor.
                         */
                        __dt_byte() : __datatype_io(__type_def_enum::TYPE_BYTE) {
                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the byte data type.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        uint64_t read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            uint8_t **T = (uint8_t **) t;

                            void *ptr = get_data_ptr(buffer, sizeof(uint8_t), offset, max_length);
                            *T = (uint8_t *) ptr;
                            return sizeof(uint8_t);
                        }

                        /*!
                         * Write (serialize) data for the byte data type to the binary output buffer.
                         *
                         * @param buffer - Output data buffer the data is to be copied to.
                         * @param value - Data value pointer to copy from.
                         * @param offset - Offset in the output buffer to start writing from.
                         * @param max_length - Max lenght of the output buffer.
                         * @return - Total number of bytes written.
                         */
                        uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(value);
                            void *ptr = get_data_ptr(buffer, sizeof(uint8_t), offset, max_length);
                            memcpy(ptr, value, sizeof(uint8_t));
                            return sizeof(uint8_t);
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (uint8_t).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            return sizeof(uint8_t);
                        }

                        /*!
                         * Compare the source and target base on the specified operator.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *target, void *source, __constraint_operator oper) override {
                            CHECK_NOT_NULL(source);
                            CHECK_NOT_NULL(target);
                            const uint8_t *s = static_cast<const uint8_t *>(source);
                            const uint8_t *t = static_cast<const uint8_t *>(target);
                            switch (oper) {
                                case __constraint_operator::LT:
                                    return (*s < *t);
                                case __constraint_operator::EQ:
                                    return (*s == *t);
                                case __constraint_operator::GT:
                                    return (*s > *t);
                                case __constraint_operator::GTEQ:
                                    return (*s >= *t);
                                case __constraint_operator::LTEQ:
                                    return (*s <= *t);
                                default:
                                    return false;
                            }
                        }

                        void print(const void *value) const override {
                            const uint8_t *s = static_cast<const uint8_t *>(value);
                            string ss = to_string(*s);
                            TRACE("[byte] %s", ss.c_str());
                        }
                    };

                    /*!
                     * Data handler for single character data type. (char)
                     */
                    class __dt_char : public __datatype_io<char> {

                    public:
                        /*!<constructor
                         *
                         * Default construtor.
                         */
                        __dt_char() : __datatype_io(__type_def_enum::TYPE_CHAR) {

                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the char data type.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        uint64_t read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            char **T = (char **) t;

                            void *ptr = get_data_ptr(buffer, sizeof(char), offset, max_length);
                            *T = (char *) ptr;
                            return sizeof(char);
                        }

                        /*!
                         * Write (serialize) data for the char data type to the binary output buffer.
                         *
                         * @param buffer - Output data buffer the data is to be copied to.
                         * @param value - Data value pointer to copy from.
                         * @param offset - Offset in the output buffer to start writing from.
                         * @param max_length - Max lenght of the output buffer.
                         * @return - Total number of bytes written.
                         */
                        uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(value);
                            void *ptr = get_data_ptr(buffer, sizeof(char), offset, max_length);
                            memcpy(ptr, value, sizeof(char));
                            return sizeof(char);
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (char).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            return sizeof(char);
                        }

                        /*!
                         * Compare the source and target base on the specified operator.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *target, void *source, __constraint_operator oper) override {
                            CHECK_NOT_NULL(source);
                            CHECK_NOT_NULL(target);
                            const char *s = static_cast<const char *>(source);
                            const char *t = static_cast<const char *>(target);
                            switch (oper) {
                                case __constraint_operator::LT:
                                    return (*s < *t);
                                case __constraint_operator::EQ:
                                    return (*s == *t);
                                case __constraint_operator::GT:
                                    return (*s > *t);
                                case __constraint_operator::GTEQ:
                                    return (*s >= *t);
                                case __constraint_operator::LTEQ:
                                    return (*s <= *t);
                                default:
                                    return false;
                            }
                        }

                        void print(const void *value) const override {
                            const char *s = static_cast<const char *>(value);
                            string ss = to_string(*s);
                            TRACE("[char] %s", ss.c_str());
                        }
                    };

                    /*!
                     * Data handler for boolean data type. (bool)
                     */
                    class __dt_bool : public __datatype_io<bool> {

                    public:
                        /*!<constructor
                         *
                         * Default construtor.
                         */
                        __dt_bool() : __datatype_io(__type_def_enum::TYPE_BOOL) {

                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the boolean data type.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        uint64_t read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            bool **T = (bool **) t;

                            void *ptr = get_data_ptr(buffer, sizeof(bool), offset, max_length);
                            *T = (bool *) ptr;
                            return sizeof(bool);
                        }

                        /*!
                         * Write (serialize) data for the boolean data type to the binary output buffer.
                         *
                         * @param buffer - Output data buffer the data is to be copied to.
                         * @param value - Data value pointer to copy from.
                         * @param offset - Offset in the output buffer to start writing from.
                         * @param max_length - Max lenght of the output buffer.
                         * @return - Total number of bytes written.
                         */
                        uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(value);
                            void *ptr = get_data_ptr(buffer, sizeof(bool), offset, max_length);
                            memcpy(ptr, value, sizeof(bool));
                            return sizeof(bool);
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (bool).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            return sizeof(bool);
                        }

                        /*!
                         * Compare the source and target base on the specified operator.
                         * (Only equals supported for bool)
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *target, void *source, __constraint_operator oper) override {
                            CHECK_NOT_NULL(source);
                            CHECK_NOT_NULL(target);
                            const bool *s = static_cast<const bool *>(source);
                            const bool *t = static_cast<const bool *>(target);
                            if (oper == __constraint_operator::EQ)
                                return (*s == *t);
                            return false;
                        }

                        void print(const void *value) const override {
                            const bool *s = static_cast<const bool *>(value);
                            string ss = to_string(*s);
                            TRACE("[bool] %s", ss.c_str());
                        }
                    };

                    /*!
                     * Data handler for SHORT data type. (short)
                     */
                    class __dt_short : public __datatype_io<short> {

                    public:
                        /*!<constructor
                         *
                         * Default construtor.
                         */
                        __dt_short() : __datatype_io(__type_def_enum::TYPE_SHORT) {

                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the short data type.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        uint64_t read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            short **T = (short **) t;

                            void *ptr = get_data_ptr(buffer, sizeof(short), offset, max_length);
                            *T = (short *) ptr;
                            return sizeof(short);
                        }

                        /*!
                         * Write (serialize) data for the short data type to the binary output buffer.
                         *
                         * @param buffer - Output data buffer the data is to be copied to.
                         * @param value - Data value pointer to copy from.
                         * @param offset - Offset in the output buffer to start writing from.
                         * @param max_length - Max lenght of the output buffer.
                         * @return - Total number of bytes written.
                         */
                        uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(value);
                            void *ptr = get_data_ptr(buffer, sizeof(short), offset, max_length);
                            memcpy(ptr, value, sizeof(short));
                            return sizeof(short);
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (short).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            return sizeof(short);
                        }

                        /*!
                         * Compare the source and target base on the specified operator.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *target, void *source, __constraint_operator oper) override {
                            CHECK_NOT_NULL(source);
                            CHECK_NOT_NULL(target);
                            const short *s = static_cast<const short *>(source);
                            const short *t = static_cast<const short *>(target);
                            switch (oper) {
                                case __constraint_operator::LT:
                                    return (*s < *t);
                                case __constraint_operator::EQ:
                                    return (*s == *t);
                                case __constraint_operator::GT:
                                    return (*s > *t);
                                case __constraint_operator::GTEQ:
                                    return (*s >= *t);
                                case __constraint_operator::LTEQ:
                                    return (*s <= *t);
                                default:
                                    return false;
                            }
                        }

                        void print(const void *value) const override {
                            const short *s = static_cast<const short *>(value);
                            string ss = to_string(*s);
                            TRACE("[short] %s", ss.c_str());
                        }
                    };

                    /*!
                     * Data handler for INTEGER data type. (int)
                     */
                    class __dt_int : public __datatype_io<int> {

                    public:
                        /*!<constructor
                         *
                         * Default construtor.
                         */
                        __dt_int() : __datatype_io(__type_def_enum::TYPE_INTEGER) {

                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the integer data type.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        uint64_t read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            int **T = (int **) t;

                            void *ptr = get_data_ptr(buffer, sizeof(int), offset, max_length);
                            *T = (int *) ptr;
                            return sizeof(int);
                        }

                        /*!
                         * Write (serialize) data for the integer data type to the binary output buffer.
                         *
                         * @param buffer - Output data buffer the data is to be copied to.
                         * @param value - Data value pointer to copy from.
                         * @param offset - Offset in the output buffer to start writing from.
                         * @param max_length - Max lenght of the output buffer.
                         * @return - Total number of bytes written.
                         */
                        uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(value);
                            void *ptr = get_data_ptr(buffer, sizeof(int), offset, max_length);
                            memcpy(ptr, value, sizeof(int));
                            return sizeof(int);
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (int).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            return sizeof(int);
                        }

                        /*!
                         * Compare the source and target base on the specified operator.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *target, void *source, __constraint_operator oper) override {
                            CHECK_NOT_NULL(source);
                            CHECK_NOT_NULL(target);
                            const int *s = static_cast<const int *>(source);
                            const int *t = static_cast<const int *>(target);
                            switch (oper) {
                                case __constraint_operator::LT:
                                    return (*s < *t);
                                case __constraint_operator::EQ:
                                    return (*s == *t);
                                case __constraint_operator::GT:
                                    return (*s > *t);
                                case __constraint_operator::GTEQ:
                                    return (*s >= *t);
                                case __constraint_operator::LTEQ:
                                    return (*s <= *t);
                                default:
                                    return false;
                            }
                        }

                        void print(const void *value) const override {
                            const int *s = static_cast<const int *>(value);
                            string ss = to_string(*s);
                            TRACE("[int] %s", ss.c_str());
                        }
                    };

                    /*!
                     * Data handler for LONG data type. (long)
                     */
                    class __dt_long : public __datatype_io<long> {

                    public:
                        /*!<constructor
                         *
                         * Default construtor.
                         */
                        __dt_long() : __datatype_io(__type_def_enum::TYPE_LONG) {

                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the long data type.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        uint64_t read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            long **T = (long **) t;

                            void *ptr = get_data_ptr(buffer, sizeof(long), offset, max_length);
                            *T = (long *) ptr;
                            return sizeof(long);
                        }

                        /*!
                        * Write (serialize) data for the long data type to the binary output buffer.
                        *
                        * @param buffer - Output data buffer the data is to be copied to.
                        * @param value - Data value pointer to copy from.
                        * @param offset - Offset in the output buffer to start writing from.
                        * @param max_length - Max lenght of the output buffer.
                        * @return - Total number of bytes written.
                        */
                        uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(value);
                            void *ptr = get_data_ptr(buffer, sizeof(long), offset, max_length);
                            memcpy(ptr, value, sizeof(long));
                            return sizeof(long);
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (long).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            return sizeof(long);
                        }

                        /*!
                         * Compare the source and target base on the specified operator.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *target, void *source, __constraint_operator oper) override {
                            CHECK_NOT_NULL(source);
                            CHECK_NOT_NULL(target);
                            const long *s = static_cast<const long *>(source);
                            const long *t = static_cast<const long *>(target);
                            switch (oper) {
                                case __constraint_operator::LT:
                                    return (*s < *t);
                                case __constraint_operator::EQ:
                                    return (*s == *t);
                                case __constraint_operator::GT:
                                    return (*s > *t);
                                case __constraint_operator::GTEQ:
                                    return (*s >= *t);
                                case __constraint_operator::LTEQ:
                                    return (*s <= *t);
                                default:
                                    return false;
                            }
                        }

                        void print(const void *value) const override {
                            const long *s = static_cast<const long *>(value);
                            string ss = to_string(*s);
                            TRACE("[long] %s", ss.c_str());
                        }
                    };

                    /*!
                     * Data handler for TIMESTAMP data type. (uint64_t)
                     */
                    class __dt_timestamp : public __datatype_io<uint64_t> {

                    public:
                        /*!<constructor
                         *
                         * Default construtor.
                         */
                        __dt_timestamp() : __datatype_io(__type_def_enum::TYPE_TIMESTAMP) {

                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the timestamp data type.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        uint64_t read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            uint64_t **T = (uint64_t **) t;

                            void *ptr = get_data_ptr(buffer, sizeof(uint64_t), offset, max_length);
                            *T = (uint64_t *) ptr;
                            return sizeof(uint64_t);
                        }

                        /*!
                        * Write (serialize) data for the timestamp data type to the binary output buffer.
                        *
                        * @param buffer - Output data buffer the data is to be copied to.
                        * @param value - Data value pointer to copy from.
                        * @param offset - Offset in the output buffer to start writing from.
                        * @param max_length - Max lenght of the output buffer.
                        * @return - Total number of bytes written.
                        */
                        uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(value);
                            void *ptr = get_data_ptr(buffer, sizeof(uint64_t), offset, max_length);
                            memcpy(ptr, value, sizeof(uint64_t));
                            return sizeof(uint64_t);
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (uint64_t).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            return sizeof(uint64_t);
                        }

                        /*!
                         * Compare the source and target base on the specified operator.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *target, void *source, __constraint_operator oper) override {
                            CHECK_NOT_NULL(source);
                            CHECK_NOT_NULL(target);
                            const uint64_t *s = static_cast<const uint64_t *>(source);
                            const uint64_t *t = static_cast<const uint64_t *>(target);
                            switch (oper) {
                                case __constraint_operator::LT:
                                    return (*s < *t);
                                case __constraint_operator::EQ:
                                    return (*s == *t);
                                case __constraint_operator::GT:
                                    return (*s > *t);
                                case __constraint_operator::GTEQ:
                                    return (*s >= *t);
                                case __constraint_operator::LTEQ:
                                    return (*s <= *t);
                                default:
                                    return false;
                            }
                        }

                        void print(const void *value) const override {
                            const uint64_t *s = static_cast<const uint64_t *>(value);
                            string ss = to_string(*s);
                            TRACE("[timestamp] %s", ss.c_str());
                        }
                    };

                    /*!
                     * Data handler for DATETIME data type. (uint64_t)
                     */
                    class __dt_datetime : public __dt_timestamp {

                    public:
                        /*!<constructor
                         *
                         * Default construtor.
                         */
                        __dt_datetime() {

                        }

                        /*!
                         * Read the date/time field into the specified string buffer.
                         *
                         * Note: This method assumes the string buffer has been allocated.
                         *
                         * @param buffer - Source data buffer (binary data)
                         * @param str - Pre-allocated string buffer to copy into.
                         * @param offset - Start offset where the buffer is to be read from.
                         * @param max_length - Max length of the data in the buffer.
                         * @param format - Date/Time format to format the output string.
                         *                  (if EMPTY will use the global format)
                         * @return - Total bytes consumed by this read.
                         */
                        uint64_t readstr(void *buffer, string *str, uint64_t offset, uint64_t max_length,
                                         const string &format = common_consts::EMPTY_STRING) {
                            CHECK_NOT_NULL(str);
                            uint64_t *ts = (uint64_t *) malloc(sizeof(uint64_t));
                            CHECK_ALLOC(ts, TYPE_NAME(uint64_t));

                            uint64_t r = read(buffer, &ts, offset, max_length);
                            POSTCONDITION(r == sizeof(uint64_t));
                            if (!IS_EMPTY(format))
                                str->assign(time_utils::get_time_string(*ts, format));
                            else
                                str->assign(time_utils::get_time_string(*ts));
                            FREE_PTR(ts);
                            return r;
                        }

                        /*!
                         * Write the date/time field value by parsing the specified date/time string.
                         *
                         * @param buffer - Output data buffer the data is to be copied to.
                         * @param value - Date/Time string buffer to copy from.
                         * @param offset - Offset in the output buffer to start writing from.
                         * @param max_length - Max lenght of the output buffer.
                         * @param format - Date/Time format to parse the input string.
                         *                  (if EMPTY will use the global format)
                         * @return - Total number of bytes written.
                         */
                        uint64_t writestr(void *buffer, const string *value, uint64_t offset, uint64_t max_length,
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

                    /*!
                     * Data handler for FLOAT data type. (float)
                     */
                    class __dt_float : public __datatype_io<float> {

                    public:
                        /*!<constructor
                         *
                         * Default construtor.
                         */
                        __dt_float() : __datatype_io(__type_def_enum::TYPE_FLOAT) {

                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the float data type.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        uint64_t read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            float **T = (float **) t;

                            void *ptr = get_data_ptr(buffer, sizeof(float), offset, max_length);
                            *T = (float *) ptr;
                            return sizeof(float);
                        }

                        /*!
                        * Write (serialize) data for the float data type to the binary output buffer.
                        *
                        * @param buffer - Output data buffer the data is to be copied to.
                        * @param value - Data value pointer to copy from.
                        * @param offset - Offset in the output buffer to start writing from.
                        * @param max_length - Max lenght of the output buffer.
                        * @return - Total number of bytes written.
                        */
                        uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(value);
                            void *ptr = get_data_ptr(buffer, sizeof(float), offset, max_length);
                            memcpy(ptr, value, sizeof(float));
                            return sizeof(float);
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (float).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            return sizeof(float);
                        }

                        /*!
                         * Compare the source and target base on the specified operator.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *target, void *source, __constraint_operator oper) override {
                            CHECK_NOT_NULL(source);
                            CHECK_NOT_NULL(target);
                            const float *s = static_cast<const float *>(source);
                            const float *t = static_cast<const float *>(target);
                            switch (oper) {
                                case __constraint_operator::LT:
                                    return (*s < *t);
                                case __constraint_operator::EQ:
                                    return (*s == *t);
                                case __constraint_operator::GT:
                                    return (*s > *t);
                                case __constraint_operator::GTEQ:
                                    return (*s >= *t);
                                case __constraint_operator::LTEQ:
                                    return (*s <= *t);
                                default:
                                    return false;
                            }
                        }

                        void print(const void *value) const override {
                            const float *s = static_cast<const float *>(value);
                            string ss = to_string(*s);
                            TRACE("[float] %s", ss.c_str());
                        }
                    };

                    /*!
                     * Data handler for DOUBLE data type. (double)
                     */
                    class __dt_double : public __datatype_io<double> {

                    public:
                        /*!<constructor
                         *
                         * Default construtor.
                         */
                        __dt_double() : __datatype_io(__type_def_enum::TYPE_DOUBLE) {

                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the double data type.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        uint64_t read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            double **T = (double **) t;

                            void *ptr = get_data_ptr(buffer, sizeof(double), offset, max_length);
                            *T = (double *) ptr;
                            return sizeof(double);
                        }

                        /*!
                        * Write (serialize) data for the double data type to the binary output buffer.
                        *
                        * @param buffer - Output data buffer the data is to be copied to.
                        * @param value - Data value pointer to copy from.
                        * @param offset - Offset in the output buffer to start writing from.
                        * @param max_length - Max lenght of the output buffer.
                        * @return - Total number of bytes written.
                        */
                        uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(value);
                            void *ptr = get_data_ptr(buffer, sizeof(double), offset, max_length);
                            memcpy(ptr, value, sizeof(double));
                            return sizeof(double);
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (double).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            return sizeof(double);
                        }

                        /*!
                         * Compare the source and target base on the specified operator.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *target, void *source, __constraint_operator oper) override {
                            CHECK_NOT_NULL(source);
                            CHECK_NOT_NULL(target);
                            const double *s = static_cast<const double *>(source);
                            const double *t = static_cast<const double *>(target);
                            switch (oper) {
                                case __constraint_operator::LT:
                                    return (*s < *t);
                                case __constraint_operator::EQ:
                                    return (*s == *t);
                                case __constraint_operator::GT:
                                    return (*s > *t);
                                case __constraint_operator::GTEQ:
                                    return (*s >= *t);
                                case __constraint_operator::LTEQ:
                                    return (*s <= *t);
                                default:
                                    return false;
                            }
                        }

                        void print(const void *value) const override {
                            const double *s = static_cast<const double *>(value);
                            string ss = to_string(*s);
                            TRACE("[double] %s", ss.c_str());
                        }
                    };

                    /*!
                     * Data handler for TEXT data type. (string)
                     *
                     * Note: The difference between TEXT and STRING types is that STRING
                     * has to be defined with a max-size and is limited to a max of USHRT_MAX.
                     */
                    class __dt_text : public __datatype_io<CHARBUFF> {

                    public:
                        /*!<constructor
                         *
                         * Default construtor.
                         */
                        __dt_text() : __datatype_io(__type_def_enum::TYPE_STRING) {

                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the string data type.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        virtual uint64_t
                        read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            CHARBUFF *T = (CHARBUFF *) t;
                            uint64_t r_size = buffer_utils::read_32str(buffer, &offset, T);
                            return r_size;
                        }

                        /*!
                        * Write (serialize) data for the string data type to the binary output buffer.
                        *
                        * @param buffer - Output data buffer the data is to be copied to.
                        * @param value - Data value pointer to copy from.
                        * @param offset - Offset in the output buffer to start writing from.
                        * @param max_length - Max lenght of the output buffer.
                        * @return - Total number of bytes written.
                        */
                        virtual uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(value);
                            CHARBUFF s = (CHARBUFF) value;
                            uint32_t size = strlen(s);
                            PRECONDITION(max_length >= (sizeof(uint32_t) + size));
                            uint64_t w_size = buffer_utils::write_32str(buffer, &offset, s, size);
                            return w_size;
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (size(long) + string length).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            const string *d = static_cast<const string *>(data);
                            CHECK_NOT_NULL(data);
                            return (sizeof(uint64_t) + (sizeof(char) * d->length()));
                        }

                        /*!
                         * Compare the source and target base on the specified operator.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *target, void *source, __constraint_operator oper) override {
                            CHECK_NOT_NULL(source);
                            CHECK_NOT_NULL(target);
                            const char *s = static_cast<const char * >(source);
                            const char *t = static_cast<const char * >(target);
                            switch (oper) {
                                case __constraint_operator::LT: {
                                    int r = strcmp(s, t);
                                    return (r < 0);
                                }
                                case __constraint_operator::EQ: {
                                    int r = strcmp(s, t);
                                    return (r == 0);
                                }
                                case __constraint_operator::GT: {
                                    int r = strcmp(s, t);
                                    return (r > 0);
                                }
                                case __constraint_operator::GTEQ: {
                                    int r = strcmp(s, t);
                                    return (r >= 0);
                                }
                                case __constraint_operator::LTEQ: {
                                    int r = strcmp(s, t);
                                    return (r <= 0);
                                }
                                default:
                                    return false;
                            }
                        }

                        void print(const void *value) const override {
                            const char *s = static_cast<const char *>(value);
                            string ss(s);
                            TRACE("[string] %s", ss.c_str());
                        }
                    };

                    /*!
                     * Data handler for STRING data type. (string)
                     *
                     * Note: The difference between TEXT and STRING types is that STRING
                     * has to be defined with a max-size and is limited to a max of (SIZE_MAX_TYPE_STRING - 1) = 255.
                     */
                    class __dt_string : public __dt_text {

                    public:

                        /*!
                        * Read (de-serialize) data from the binary format for the string data type.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        virtual uint64_t
                        read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            CHARBUFF *T = (CHARBUFF *) t;
                            uint64_t r_size = buffer_utils::read_8str(buffer, &offset, T);
                            return r_size;
                        }

                        /*!
                        * Write (serialize) data for the string data type to the binary output buffer.
                        *
                        * @param buffer - Output data buffer the data is to be copied to.
                        * @param value - Data value pointer to copy from.
                        * @param offset - Offset in the output buffer to start writing from.
                        * @param max_length - Max lenght of the output buffer.
                        * @return - Total number of bytes written.
                        */
                        virtual uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(value);
                            CHARBUFF s = (CHARBUFF) value;
                            uint8_t size = strlen(s);
                            PRECONDITION(max_length >= (sizeof(uint8_t) + size));
                            uint64_t w_size = buffer_utils::write_8str(buffer, &offset, s, size);
                            return w_size;
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (size(uint8_t) + string length).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            const string *d = static_cast<const string *>(data);
                            CHECK_NOT_NULL(data);
                            return (sizeof(uint8_t) + (sizeof(char) * d->length()));
                        }
                    };

                }
REACTFS_NS_CORE_END
#endif //REACTFS_TYPES_COMMON_H
