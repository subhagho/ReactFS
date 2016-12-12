//
// Created by Subhabrata Ghosh on 11/12/16.
//

#ifndef REACTFS_SCHEMA_DEF_H
#define REACTFS_SCHEMA_DEF_H

#include "common/includes/common.h"
#include "core/includes/core.h"

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
                            } else if (type == __type_def_enum::TYPE_STRUCT) {
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
                                    return "struct";
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

                        /*!
                         * Protected constructor.
                         *
                         * @param type - Data type enum.
                         */
                        __base_datatype_io(__type_def_enum type) {
                            PRECONDITION(type != __type_def_enum::TYPE_UNKNOWN);
                            this->type = type;
                        }

                    public:
                        /*!
                         * Get the data time enum of this handler.
                         *
                         * @return - Data type enum.
                         */
                        __type_def_enum get_type() {
                            return this->type;
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
                        write(void *buffer, void *value, uint64_t offset, uint64_t max_length, ...)  = 0;

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
                        virtual bool compare(const void *source, const void *target, __constraint_operator oper) = 0;
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
                        __datatype_io(__type_def_enum type) : __base_datatype_io(type) {
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
                        write(void *buffer, void *value, uint64_t offset, uint64_t max_length, ...) override = 0;

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
                        compare(const void *source, const void *target, __constraint_operator oper) override = 0;
                    };


                    class __constraint {
                    public:
                        virtual ~__constraint() {}

                        virtual bool validate(const void *value) const = 0;

                        virtual uint32_t write(void *buffer, uint64_t offset) = 0;

                        virtual uint32_t read(void *buffer, uint64_t offset) = 0;
                    };

                    class __native_type {
                    protected:
                        string name;
                        uint8_t index;
                        __type_def_enum datatype;
                        __constraint *constraint = nullptr;
                        void *default_value = nullptr;

                    public:
                        __native_type(const uint8_t index, const string &name, const __type_def_enum datatype) {
                            this->index = index;
                            this->name = string(name);
                            this->datatype = datatype;
                        }

                        __native_type(const uint8_t index, const __type_def_enum datatype) {
                            this->index = index;
                            this->datatype = datatype;
                        }

                        virtual ~__native_type() {
                            CHECK_AND_FREE(constraint);
                            FREE_PTR(default_value);
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

                        void set_default_value(void *default_value) {
                            this->default_value = default_value;
                        }

                        const void *get_default_value() const {
                            return this->default_value;
                        }

                        bool is_valid_value(void *value) {
                            if (NOT_NULL(constraint)) {
                                return constraint->validate(value);
                            }
                            return true;
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
                        }

                        __sized_type(const uint8_t index, const __type_def_enum datatype,
                                     const uint32_t max_size)
                                : __native_type(index, datatype) {
                            PRECONDITION(max_size > 0);
                            this->max_size = max_size;
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

                        }

                        __complex_type(const uint8_t index) : __native_type(index,
                                                                            __type_def_enum::TYPE_STRUCT) {

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
