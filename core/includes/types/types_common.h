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
#include "core/includes/core.h"

#define SIZE_MAX_TYPE_STRING 256

#define BIT_TYPE_CONSTRAINT 1
#define BIT_TYPE_DEFAULT_VALUE 2

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace types {
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
                        /// Array of Basic datatypes. (max size limited)
                                TYPE_ARRAY,
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
                        static string get_type_string(__type_def_enum type) {
                            switch (type) {
                                case __type_def_enum::TYPE_ARRAY:
                                    return "array";
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
                            } else if (t == __type_def_enum::TYPE_ARRAY) {
                                return __type_def_enum::TYPE_ARRAY;
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
                         * Compare the source and target base on the specified operator.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool compare(const void *source, void *target, __constraint_operator oper) = 0;
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
                            PRECONDITION((offset + size) < max_length);
                            return common_utils::increment_data_ptr(source, offset);
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
                        compare(const void *source, void *target, __constraint_operator oper) override = 0;
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
                        compare(const void *source, void *target, __constraint_operator oper) override {
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
                        compare(const void *source, void *target, __constraint_operator oper) override {
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
                        compare(const void *source, void *target, __constraint_operator oper) override {
                            CHECK_NOT_NULL(source);
                            CHECK_NOT_NULL(target);
                            const bool *s = static_cast<const bool *>(source);
                            const bool *t = static_cast<const bool *>(target);
                            if (oper == __constraint_operator::EQ)
                                return (*s == *t);
                            return false;
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
                        compare(const void *source, void *target, __constraint_operator oper) override {
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
                        compare(const void *source, void *target, __constraint_operator oper) override {
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
                        compare(const void *source, void *target, __constraint_operator oper) override {
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
                        compare(const void *source, void *target, __constraint_operator oper) override {
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
                            uint64_t *ts = nullptr;
                            uint64_t r = read(buffer, &ts, offset, max_length);
                            CHECK_NOT_NULL(ts);
                            POSTCONDITION(r == sizeof(uint64_t));
                            if (!IS_EMPTY(format))
                                str->assign(time_utils::get_time_string(*ts, format));
                            else
                                str->assign(time_utils::get_time_string(*ts));
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
                        compare(const void *source, void *target, __constraint_operator oper) override {
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
                        compare(const void *source, void *target, __constraint_operator oper) override {
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
                    };

                    /*!
                     * Data handler for TEXT data type. (string)
                     *
                     * Note: The difference between TEXT and STRING types is that STRING
                     * has to be defined with a max-size and is limited to a max of USHRT_MAX.
                     */
                    class __dt_text : public __datatype_io<string> {

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
                            string **T = (string **) t;
                            void *ptr = get_data_ptr(buffer, sizeof(uint64_t), offset, max_length);
                            // Get the size of the string data (uint64_t)
                            uint64_t size = 0;
                            memcpy(&size, ptr, sizeof(uint64_t));
                            // If size if 0, then the string is null.
                            if (size > 0) {
                                uint64_t d_size = (size * sizeof(char));
                                ptr = common_utils::increment_data_ptr(ptr, sizeof(uint64_t));
                                PRECONDITION((offset + d_size + sizeof(uint64_t)) < max_length);
                                char *cptr = (char *) ptr;
                                *T = new string(cptr, d_size);
                                CHECK_ALLOC(*T, TYPE_NAME(string));
                                return (d_size + sizeof(uint64_t));
                            }
                            return sizeof(uint64_t);
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
                            void *ptr = get_data_ptr(buffer, sizeof(uint64_t), offset, max_length);
                            string *s = (string *) value;
                            uint64_t size = (s->length() * sizeof(char));
                            // Write the size of the string buffer. (uint64_t)
                            memcpy(ptr, &size, sizeof(uint64_t));
                            // If size is greater than 0, write the string buffer.
                            if (size > 0) {
                                PRECONDITION((offset + sizeof(uint64_t) + size) < max_length);
                                ptr = common_utils::increment_data_ptr(ptr, sizeof(uint64_t));
                                memcpy(ptr, s->c_str(), s->length());
                                return (size + sizeof(uint64_t));
                            }
                            return sizeof(uint64_t);
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
                        compare(const void *source, void *target, __constraint_operator oper) override {
                            CHECK_NOT_NULL(source);
                            CHECK_NOT_NULL(target);
                            const string *s = static_cast<const string *>(source);
                            const string *t = static_cast<const string *>(target);
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
                            string **T = (string **) t;
                            void *ptr = get_data_ptr(buffer, sizeof(uint64_t), offset, max_length);
                            uint16_t size = 0;
                            // Get the size of the string data (uint16_t)
                            memcpy(&size, ptr, sizeof(uint16_t));
                            // If size if 0, then the string is null.
                            if (size > 0) {
                                uint64_t d_size = (size * sizeof(char));
                                ptr = common_utils::increment_data_ptr(ptr, sizeof(uint64_t));
                                PRECONDITION((offset + d_size + sizeof(uint64_t)) < max_length);
                                char *cptr = (char *) ptr;
                                *T = new string(cptr, d_size);
                                CHECK_ALLOC(*T, TYPE_NAME(string));
                                return (d_size + sizeof(uint16_t));
                            }
                            return sizeof(uint16_t);
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
                            void *ptr = get_data_ptr(buffer, sizeof(uint64_t), offset, max_length);
                            string *s = (string *) value;
                            PRECONDITION(s->length() < SIZE_MAX_TYPE_STRING);
                            uint16_t size = (s->length() * sizeof(char));
                            // Write the size of the string buffer. (uint16_t)
                            memcpy(ptr, &size, sizeof(uint16_t));
                            // If size if 0, then the string is null.
                            if (size > 0) {
                                PRECONDITION((offset + sizeof(uint16_t) + size) < max_length);
                                ptr = common_utils::increment_data_ptr(ptr, sizeof(uint16_t));
                                memcpy(ptr, s->c_str(), s->length());
                                return (size + sizeof(uint16_t));
                            }
                            return sizeof(uint16_t);
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
                            return (sizeof(uint16_t) + (sizeof(char) * d->length()));
                        }
                    };

                    /*!
                     * Singleton utility class for managing data type IO handlers.
                     *
                     * Native type handlers are automatically added. Other type handlers
                     * should be added externally.
                     *
                     * Note:: Needs to be disposed prior to application existing.
                     */
                    class __type_defs_utils {
                    private:
                        /// Thread lock mutex for creating handlers.
                        static mutex thread_mutex;

                        /// Static map of the registered data type IO handlers.
                        static unordered_map<string, __base_datatype_io *> type_handlers;


                        /*!
                         * Create a new instance of the IO handler.
                         *
                         * @param type - Datatype enum
                         * @return - Datatype IO handler
                         */
                        static __base_datatype_io *create_type_handler(__type_def_enum type) {
                            lock_guard<std::mutex> lock(thread_mutex);
                            string type_n = __type_enum_helper::get_type_string(type);
                            unordered_map<string, __base_datatype_io *>::iterator iter = type_handlers.find(type_n);
                            if (iter != type_handlers.end()) {
                                return nullptr;
                            }
                            __base_datatype_io *t = nullptr;
                            switch (type) {
                                case __type_def_enum::TYPE_BYTE:
                                    t = new __dt_byte();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_byte));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    t = new __dt_char();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_char));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    t = new __dt_double();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_double));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    t = new __dt_float();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_float));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    t = new __dt_int();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_int));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    t = new __dt_long();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_long));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    t = new __dt_short();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_short));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    t = new __dt_string();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_string));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    t = new __dt_byte();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_byte));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    t = new __dt_text();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_text));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    t = new __dt_timestamp();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_timestamp));
                                    break;
                                case __type_def_enum::TYPE_BOOL:
                                    t = new __dt_bool();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_bool));
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                    t = new __dt_datetime();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_datetime));
                                    break;
                                default:
                                    throw BASE_ERROR("Type not supported as basic type handler. [type=%s]",
                                                     __type_enum_helper::get_type_string(type).c_str());
                            }
                            return t;
                        }

                    public:

                        static string create_list_key(__type_def_enum type, __type_def_enum inner_type) {
                            return common_utils::format("%s::%s", __type_enum_helper::get_type_string(type).c_str(),
                                                        __type_enum_helper::get_type_string(inner_type).c_str());
                        }

                        static string
                        create_map_key(__type_def_enum type, __type_def_enum key_type, __type_def_enum value_type) {
                            return common_utils::format("%s::%s::%s", __type_enum_helper::get_type_string(type).c_str(),
                                                        __type_enum_helper::get_type_string(key_type).c_str(),
                                                        __type_enum_helper::get_type_string(value_type).c_str());
                        }

                        /*!
                         * Get the datatype IO handler for the specified datatype enum.
                         *
                         * Note: Only valid inner type datatype handlers are auto-created, others should be
                         * added explicitly.
                         *
                         * @param type - Datatype enum
                         * @return - Datatype IO handler
                         */
                        static __base_datatype_io *get_type_handler(__type_def_enum type) {
                            PRECONDITION(__type_enum_helper::is_native(type) || type == __type_def_enum::TYPE_TEXT);
                            string type_n = __type_enum_helper::get_type_string(type);
                            unordered_map<string, __base_datatype_io *>::iterator iter = type_handlers.find(type_n);
                            if (iter != type_handlers.end()) {
                                return iter->second;
                            } else {
                                if (__type_enum_helper::is_native(type) || type == __type_def_enum::TYPE_TEXT) {
                                    // If not available and native type, create a new instance.
                                    __base_datatype_io *ptr = create_type_handler(type);
                                    if (IS_NULL(ptr)) {
                                        // Another thread should have added this type handler.
                                        iter = type_handlers.find(type_n);
                                        if (iter != type_handlers.end()) {
                                            return iter->second;
                                        }
                                    } else {
                                        type_handlers.insert({type_n, ptr});
                                        return ptr;
                                    }
                                }
                            }
                            return nullptr;
                        }

                        /*!
                         * Get the datatype IO handler for the specified datatype key.
                         *
                         * Note: Only valid inner type datatype handlers are auto-created, others should be
                         * added explicitly.
                         *
                         * @param type - Datatype enum
                         * @return - Datatype IO handler
                         */
                        static __base_datatype_io *get_type_handler(string &key) {
                            CHECK_NOT_EMPTY(key);
                            unordered_map<string, __base_datatype_io *>::iterator iter = type_handlers.find(key);
                            if (iter != type_handlers.end()) {
                                return iter->second;
                            }
                            return nullptr;
                        }

                        /*!
                         * Add a non-native IO handler that was instantiated externally.
                         *
                         *
                         * @param key - String key for array/list/map types
                         * @param handler - Handler instance.
                         * @return - Has been added? Will return false if handler already registered.
                         */
                        static bool add_external_handler(string &key, __base_datatype_io *handler) {
                            CHECK_NOT_NULL(handler);
                            CHECK_NOT_EMPTY(key);
                            lock_guard<std::mutex> lock(thread_mutex);
                            unordered_map<string, __base_datatype_io *>::iterator iter = type_handlers.find(key);
                            if (iter != type_handlers.end()) {
                                return false;
                            }
                            type_handlers.insert({key, handler});
                            return true;
                        }

                        /*!
                         * Add a non-native IO handler that was instantiated externally.
                         *
                         *
                         * @param type - Datatype enum
                         * @param handler - Handler instance.
                         * @return - Has been added? Will return false if handler already registered.
                         */
                        static bool add_external_handler(__type_def_enum type, __base_datatype_io *handler) {
                            CHECK_NOT_NULL(handler);
                            lock_guard<std::mutex> lock(thread_mutex);
                            string type_n = __type_enum_helper::get_type_string(type);
                            unordered_map<string, __base_datatype_io *>::iterator iter = type_handlers.find(type_n);
                            if (iter != type_handlers.end()) {
                                return false;
                            }
                            type_handlers.insert({type_n, handler});
                            return true;
                        }

                        /*!
                         * Dispose all the registered instances of the IO handlers.
                         */
                        static void dispose() {
                            lock_guard<std::mutex> lock(thread_mutex);
                            unordered_map<string, __base_datatype_io *>::iterator iter;
                            for (iter = type_handlers.begin(); iter != type_handlers.end(); iter++) {
                                CHECK_AND_FREE(iter->second);
                            }
                            type_handlers.clear();
                        }
                    };

                    /*!
                     * Absract base class for defining type constraints.
                     */
                    class __constraint {
                    protected:
                        bool is_not = false;
                    public:
                        /*!
                         * Virtual base desctructor.
                         */
                        virtual ~__constraint() {}

                        /*!
                         * Validate the data by applying this constraint instance.
                         *
                         * @param value - Data value to validate
                         * @return - Constraint passed?
                         */
                        virtual bool validate(void *value) const = 0;

                        /*!
                         * Write (serialize) this constraint instance.
                         *
                         * @param buffer - Output buffer to write the constraint to.
                         * @param offset - Offset in the buffer to start writing.
                         * @return - Number of byte written.
                         */
                        virtual uint32_t write(void *buffer, uint64_t offset) = 0;

                        /*!
                         * Read (de-serialize) the constraint instance.
                         *
                         * @param buffer - Input buffer to read data from.
                         * @param offset - Offset in the input buffer to read from.
                         * @return - Number of bytes consumed.
                         */
                        virtual uint32_t read(void *buffer, uint64_t offset) = 0;

                        /*!
                         * Get the datatype enum that this constraint instance supports.
                         *
                         * @return - Supported datatype enum.
                         */
                        virtual __type_def_enum get_datatype() = 0;

                        /*!
                         * Set if this constraint should return the negative of the constraint definition.
                         *
                         * @param is_not - Should be reversed?
                         */
                        void set_not(bool is_not) {
                            this->is_not = is_not;
                        }
                    };

                    /*!
                     * Abstract base class for defining default values.
                     */
                    class __default {
                    protected:
                        /// Datatype IO handler.
                        __base_datatype_io *handler = nullptr;

                    public:

                        /*!<destructor
                         * Base virtual destructor.
                         */
                        virtual ~__default() {

                        }

                        /*!
                         * Write (serialize) this default value instance.
                         *
                         * @param buffer - Output buffer to write the constraint to.
                         * @param offset - Offset in the buffer to start writing.
                         * @return - Number of byte written.
                         */
                        virtual uint32_t write(void *buffer, uint64_t offset) = 0;

                        /*!
                         * Read (de-serialize) the default value instance.
                         *
                         * @param buffer - Input buffer to read data from.
                         * @param offset - Offset in the input buffer to read from.
                         * @return - Number of bytes consumed.
                         */
                        virtual uint32_t read(void *buffer, uint64_t offset) = 0;

                        /*!
                         * Get the datatype enum that this default value instance supports.
                         *
                         * @return - Supported datatype enum.
                         */
                        virtual __type_def_enum get_datatype() = 0;

                        /*!
                         * Set the passed pointer to the default value.
                         *
                         * @param value - Value pointer.
                         */
                        virtual void set_default(void *value) const = 0;
                    };

                    /*!
                     * Template base class for defining typed default value instances.
                     *
                     * @tparam __T - Datatype of the value.
                     * @tparam __type - Datatype enum of the value.
                     */
                    template<typename __T, __type_def_enum __type>
                    class __typed_default : public __default {
                    private:
                        /// Datatype enum of the value.
                        __type_def_enum datatype = __type;
                        /// Typed default value
                        __T value;
                    public:
                        /*!<constructor
                         * Default empty constructor.
                         */
                        __typed_default() {
                            PRECONDITION(__type_enum_helper::is_native(datatype));
                            handler = __type_defs_utils::get_type_handler(this->datatype);
                            CHECK_NOT_NULL(handler);
                        }

                        /*!
                         * Get the default value.
                         *
                         * @return - Typed default value.
                         */
                        const __T get_value() const {
                            CHECK_NOT_NULL(value);
                            return value;
                        }

                        /*!
                         * Set the default value.
                         *
                         * @param value - Typed default value.
                         */
                        void set_value(__T value) {
                            this->value = value;
                        }

                        /*!
                         * Set the passed pointer to the default value.
                         *
                         * @param value - Value pointer.
                         */
                        void set_default(void *value) const override {
                            __T *t = (__T *) value;
                            CHECK_NOT_NULL(t);
                            *t = this->value;
                        }

                        /*!
                         * Get the datatype enum of the supported value type.
                         *
                         * @return - Supported datatype enum.
                         */
                        __type_def_enum get_datatype() override {
                            return this->datatype;
                        }

                        /*!
                         * Write (serialize) this default value instance.
                         *
                         * @param buffer - Output buffer to write the constraint to.
                         * @param offset - Offset in the buffer to start writing.
                         * @return - Number of byte written.
                         */
                        uint32_t write(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            return handler->write(buffer, &value, offset, ULONG_MAX);
                        }

                        /*!
                         * Read (de-serialize) the default value instance.
                         *
                         * @param buffer - Input buffer to read data from.
                         * @param offset - Offset in the input buffer to read from.
                         * @return - Number of bytes consumed.
                         */
                        uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            __T *t = nullptr;
                            uint64_t r_size = handler->read(buffer, &t, offset, ULONG_MAX);
                            CHECK_NOT_NULL(t);

                            this->value = *t;

                            return r_size;
                        }
                    };

                    typedef __typed_default<char, __type_def_enum::TYPE_CHAR> __default_char;
                    typedef __typed_default<bool, __type_def_enum::TYPE_BOOL> __default_bool;
                    typedef __typed_default<short, __type_def_enum::TYPE_SHORT> __default_short;
                    typedef __typed_default<int, __type_def_enum::TYPE_INTEGER> __default_int;
                    typedef __typed_default<long, __type_def_enum::TYPE_LONG> __default_long;
                    typedef __typed_default<uint64_t, __type_def_enum::TYPE_TIMESTAMP> __default_timestamp;
                    typedef __typed_default<float, __type_def_enum::TYPE_FLOAT> __default_float;
                    typedef __typed_default<double, __type_def_enum::TYPE_DOUBLE> __default_double;
                    typedef __typed_default<string, __type_def_enum::TYPE_STRING> __default_string;

                    class __defaults_loader {
                    public:
                        static __default *
                        read(void *buffer, uint64_t offset, __type_def_enum datatype, uint64_t *size) {
                            __default *d = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_BOOL:
                                    d = new __default_bool();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_bool));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    d = new __default_char();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_char));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    d = new __default_short();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_short));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    d = new __default_int();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_int));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    d = new __default_long();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_long));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    d = new __default_timestamp();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_timestamp));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    d = new __default_float();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_float));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    d = new __default_double();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_double));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    d = new __default_string();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_string));
                                    break;
                                default:
                                    throw BASE_ERROR("No default value handler defined for datatype. [type=%d]",
                                                     datatype);
                            }
                            CHECK_NOT_NULL(d);
                            *size = d->read(buffer, offset);
                            return d;
                        }

                        static __default *
                        create(string *value, __type_def_enum datatype) {
                            __default *d = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_BOOL:
                                    d = new __default_bool();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_bool));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    d = new __default_char();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_char));
                                    {
                                        char c = (*value)[0];
                                        d->set_default(&c);
                                    }
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    d = new __default_short();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_short));
                                    {
                                        int is = std::stoi(*value);
                                        POSTCONDITION(is >= std::numeric_limits<short>::min() &&
                                                      is <= std::numeric_limits<short>::max());
                                        short s = (short) is;
                                        d->set_default(&s);
                                    }
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    d = new __default_int();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_int));
                                    {
                                        int ii = std::stoi(*value);
                                        d->set_default(&ii);
                                    }
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    d = new __default_long();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_long));
                                    {
                                        long ll = std::stol(*value);
                                        d->set_default(&ll);
                                    }
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    d = new __default_timestamp();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_timestamp));
                                    {
                                        long lu = std::stol(*value);
                                        uint64_t ul = (uint64_t) lu;
                                        d->set_default(&ul);
                                    }
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    d = new __default_float();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_float));
                                    {
                                        float fv = std::stof(*value);
                                        d->set_default(&fv);
                                    }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    d = new __default_double();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_double));
                                    {
                                        double dd = std::stod(*value);
                                        d->set_default(&dd);
                                    }
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    d = new __default_string();
                                    CHECK_ALLOC(d, TYPE_NAME(__default_string));
                                    d->set_default(value);
                                    break;
                                default:
                                    throw BASE_ERROR("No default value handler defined for datatype. [type=%d]",
                                                     datatype);
                            }
                            return d;
                        }
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_TYPES_COMMON_H
