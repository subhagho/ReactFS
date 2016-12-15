
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
// Created by Subhabrata Ghosh on 06/12/16.
//

#ifndef REACTFS_CONSTRAINTS_H
#define REACTFS_CONSTRAINTS_H

#include <regex>
#include <vector>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/time_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"

#include "types_common.h"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace types {
                    /*!
                     * Enum defines the types of constraints supported.
                     */
                    typedef enum __constraint_type__ {
                        /// Check if a value is not null.
                                CONSTRAINT_NOT_NULLABLE = 0,
                        /// Check if the value is within the constraint range.
                                CONSTRAINT_RANGE = 1,
                        /// Check if the value is in the constraint list.
                                CONSTRAINT_IN = 2,
                        /// Check if the value matches the constraint regex.
                                CONSTRAINT_REGEX = 3,
                        /// Check if the value is greater than the constraint value
                                CONSTRAINT_GT = 4,
                        /// Check if the value is lesser than the constraint value
                                CONSTRAINT_LT = 5,
                        /// Check if the value is greater than the constraint value
                                CONSTRAINT_GTEQ = 6,
                        /// Check if the value is lesser than the constraint value
                                CONSTRAINT_LTEQ = 7,
                    } __constraint_type;

                    /*!
                     * Utility class for constraint types.
                     */
                    class __constraint_type_utils {
                    public:
                        /*!
                         * Get the numerical value of this constraint type.
                         *
                         * @param type - Constraint type enum.
                         * @return - Numerical value.
                         */
                        static uint8_t get_number_value(__constraint_type type) {
                            switch (type) {
                                case __constraint_type::CONSTRAINT_NOT_NULLABLE:
                                    return 0;
                                case __constraint_type::CONSTRAINT_RANGE:
                                    return 1;
                                case __constraint_type::CONSTRAINT_IN:
                                    return 2;
                                case __constraint_type::CONSTRAINT_REGEX:
                                    return 3;
                                case __constraint_type::CONSTRAINT_GT:
                                    return 4;
                                case __constraint_type::CONSTRAINT_LT:
                                    return 5;
                                case __constraint_type::CONSTRAINT_GTEQ:
                                    return 6;
                                case __constraint_type::CONSTRAINT_LTEQ:
                                    return 7;
                            }
                        }

                        /*!
                         * Get the string for the specified constraint type.
                         *
                         * @param type - Constraint type enum.
                         * @return - String value of the enum.
                         */
                        static string get_string(__constraint_type type) {
                            switch (type) {
                                case __constraint_type::CONSTRAINT_NOT_NULLABLE:
                                    return "not nullable";
                                case __constraint_type::CONSTRAINT_RANGE:
                                    return "between";
                                case __constraint_type::CONSTRAINT_IN:
                                    return "in";
                                case __constraint_type::CONSTRAINT_REGEX:
                                    return "regex";
                                case __constraint_type::CONSTRAINT_GT:
                                    return ">";
                                case __constraint_type::CONSTRAINT_LT:
                                    return "<";
                                case __constraint_type::CONSTRAINT_GTEQ:
                                    return ">=";
                                case __constraint_type::CONSTRAINT_LTEQ:
                                    return "<=";
                            }
                        }

                        static __constraint_type parse(string type) {
                            type = string_utils::tolower(type);
                            if (type ==
                                __constraint_type_utils::get_string(__constraint_type::CONSTRAINT_NOT_NULLABLE)) {
                                return __constraint_type::CONSTRAINT_NOT_NULLABLE;
                            } else if (type ==
                                       __constraint_type_utils::get_string(
                                               __constraint_type::CONSTRAINT_RANGE)) {
                                return __constraint_type::CONSTRAINT_RANGE;
                            } else if (type ==
                                       __constraint_type_utils::get_string(
                                               __constraint_type::CONSTRAINT_IN)) {
                                return __constraint_type::CONSTRAINT_IN;
                            } else if (type ==
                                       __constraint_type_utils::get_string(
                                               __constraint_type::CONSTRAINT_REGEX)) {
                                return __constraint_type::CONSTRAINT_REGEX;
                            } else if (type ==
                                       __constraint_type_utils::get_string(
                                               __constraint_type::CONSTRAINT_GT)) {
                                return __constraint_type::CONSTRAINT_GT;
                            } else if (type ==
                                       __constraint_type_utils::get_string(
                                               __constraint_type::CONSTRAINT_LT)) {
                                return __constraint_type::CONSTRAINT_LT;
                            } else if (type ==
                                       __constraint_type_utils::get_string(
                                               __constraint_type::CONSTRAINT_GTEQ)) {
                                return __constraint_type::CONSTRAINT_GTEQ;
                            } else if (type ==
                                       __constraint_type_utils::get_string(
                                               __constraint_type::CONSTRAINT_LTEQ)) {
                                return __constraint_type::CONSTRAINT_LTEQ;
                            }
                            throw BASE_ERROR("Unknown constraint type. [type=%s]", type.c_str());
                        }
                    };

                    /*!
                     * Not null contraint definition.
                     */
                    class __notnull : public __constraint {
                    public:
                        /*!
                         * Validate the data is not null.
                         *
                         * @param value - Data value to validate
                         * @return - Constraint passed?
                         */
                        virtual bool validate(void *value) const override {
                            if (is_not)
                                return IS_NULL(value);
                            return NOT_NULL(value);
                        }

                        /*!
                         * Write (serialize) this constraint instance.
                         *
                         * @param buffer - Output buffer to write the constraint to.
                         * @param offset - Offset in the buffer to start writing.
                         * @return - Number of byte written.
                         */
                        virtual uint32_t write(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);

                            uint8_t type = __constraint_type_utils::get_number_value(
                                    __constraint_type::CONSTRAINT_NOT_NULLABLE);
                            memcpy(ptr, &type, sizeof(uint8_t));

                            return sizeof(uint8_t);
                        }

                        /*!
                         * Read (de-serialize) the constraint instance.
                         *
                         * @param buffer - Input buffer to read data from.
                         * @param offset - Offset in the input buffer to read from.
                         * @return - Number of bytes consumed.
                         */
                        virtual uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t *type = static_cast<uint8_t *>(ptr);
                            POSTCONDITION(*type ==
                                          __constraint_type_utils::get_number_value(
                                                  __constraint_type::CONSTRAINT_NOT_NULLABLE));

                            return sizeof(uint8_t);
                        }

                        /*!
                         * Get the datatype enum that this constraint instance supports.
                         * All types are supported for nut null constraint.
                         *
                         * @return - Supported datatype enum.
                         */
                        __type_def_enum get_datatype() override {
                            return __type_def_enum::TYPE_UNKNOWN;
                        }
                    };

                    /*!
                     * Regular expression based constraint definition. Only applicable
                     * for STRING datatype.
                     */
                    class __regex : public __constraint {
                    private:
                        /// Regular expression string.
                        string pattern;
                        /// Regex definition
                        std::regex regex_;
                        /// IO handler for this regex data.
                        __base_datatype_io *handler = nullptr;

                    public:
                        /*!<constructor
                         * Default empty constructor.
                         */
                        __regex() {
                            handler = __type_defs_utils::get_type_handler(__type_def_enum::TYPE_STRING);
                            CHECK_NOT_NULL(handler);
                        }

                        /*!
                         * Set the string value of the regex pattern.
                         *
                         * @param value - Regex pattern string.
                         */
                        void set_pattern(string &pattern) {
                            this->pattern = string(pattern);
                            this->regex_ = regex(pattern);
                        }

                        /*!
                         * Validate the input string against the regex pattern.
                         *
                         * @param value - Input string to validate.
                         * @return - Constraint passed?
                         */
                        virtual bool validate(void *value) const override {
                            if (NOT_NULL(value)) {
                                const string *ss = static_cast<const string *>(value);
                                CHECK_NOT_NULL(ss);
                                // return std::regex_match(ss, this->regex_);
                            }
                            return false;
                        }

                        /*!
                         * Write (serialize) this constraint instance.
                         *
                         * @param buffer - Output buffer to write the constraint to.
                         * @param offset - Offset in the buffer to start writing.
                         * @return - Number of byte written.
                         */
                        virtual uint32_t write(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);

                            uint8_t type = __constraint_type_utils::get_number_value(
                                    __constraint_type::CONSTRAINT_REGEX);
                            memcpy(ptr, &type, sizeof(uint8_t));
                            uint16_t w_size = sizeof(uint8_t);
                            w_size += handler->write(buffer, &pattern, (offset + w_size), ULONG_MAX);

                            return w_size;
                        }

                        /*!
                        * Read (de-serialize) the constraint instance.
                        *
                        * @param buffer - Input buffer to read data from.
                        * @param offset - Offset in the input buffer to read from.
                        * @return - Number of bytes consumed.
                        */
                        virtual uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t *type = static_cast<uint8_t *>(ptr);
                            POSTCONDITION(*type ==
                                          __constraint_type_utils::get_number_value(
                                                  __constraint_type::CONSTRAINT_REGEX));
                            string *value = nullptr;
                            uint16_t r_size = sizeof(uint8_t);
                            r_size += handler->read(buffer, &value, (offset + r_size), ULONG_MAX);
                            CHECK_NOT_NULL(value);
                            set_pattern(*value);

                            return r_size;
                        }

                        /*!
                         * Get the datatype enum that this constraint instance supports.
                         * Only string datatype supported.
                         *
                         * @return - Supported datatype enum.
                         */
                        __type_def_enum get_datatype() override {
                            return __type_def_enum::TYPE_STRING;
                        }
                    };

                    /*!
                     * Template definition of a range based constraint definition.
                     * Range constraints are inclusive (both for lower and upper bounds).
                     *
                     * @tparam __T - Datatype of the range definition.
                     * @tparam __type - Datatype enum.
                     */
                    template<typename __T, __type_def_enum __type>
                    class __range_constraint : public __constraint {
                    private:
                        /// Datatype enum of the supported range type.
                        __type_def_enum value_type = __type;
                        /// Min value of the range
                        __T min_value;
                        /// Max value of the range.
                        __T max_value;
                        /// Datatype IO handler.
                        __base_datatype_io *handler = nullptr;

                    public:
                        /*!<constructor
                         * Default empty constructor.
                         */
                        __range_constraint() {
                            PRECONDITION(__type_enum_helper::is_native(value_type));
                            handler = __type_defs_utils::get_type_handler(value_type);
                            CHECK_NOT_NULL(handler);
                        }

                        virtual ~__range_constraint() {

                        }

                        /*!
                         * Set the min bounds of this range.
                         *
                         * @param min_value - Minimum value.
                         */
                        void set_min_value(__T min_value) {
                            this->min_value = min_value;
                        }

                        /*!
                         * Set the max bounds of this range.
                         *
                         * @param max_value - Maximum value.
                         */
                        void set_max_value(__T max_value) {
                            this->max_value = max_value;
                        }

                        /*!
                         * Get the datatype enum that this constraint instance supports.
                         * All basic datatypes are supported.
                         *
                         * @return - Supported datatype enum.
                         */
                        __type_def_enum get_datatype() override {
                            return this->value_type;
                        }

                        /*!
                         * Validate the input is bounded by this range definition.
                         * Range constraints are inclusive (both for lower and upper bounds).
                         *
                         * @param value - Input data value to validate.
                         * @return - Constraint passed?
                         */
                        virtual bool validate(void *value) const override {
                            if (NOT_NULL(value)) {
                                __T *t = (__T *) value;
                                CHECK_NOT_NULL(t);
                                bool r = (*t >= min_value && *t <= max_value);
                                if (is_not)
                                    return !r;
                                return r;
                            }
                            return false;
                        }

                        /*!
                         * Write (serialize) this constraint instance.
                         *
                         * @param buffer - Output buffer to write the constraint to.
                         * @param offset - Offset in the buffer to start writing.
                         * @return - Number of byte written.
                         */
                        virtual uint32_t write(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t type = __constraint_type_utils::get_number_value(
                                    __constraint_type::CONSTRAINT_RANGE);
                            memcpy(ptr, &type, sizeof(uint8_t));

                            uint64_t w_size = sizeof(uint8_t);
                            // Write the min value first.
                            uint64_t off = (offset + w_size);
                            w_size += handler->write(buffer, &min_value, off,
                                                     ULONG_MAX); // Note: Don't need to worry about buffer size here.
                            // Write the max value next.
                            off = (offset + w_size);
                            w_size += handler->write(buffer, &max_value, off,
                                                     ULONG_MAX); // Note: Don't need to worry about buffer size here.
                            return w_size;
                        }

                        /*!
                        * Read (de-serialize) the constraint instance.
                        *
                        * @param buffer - Input buffer to read data from.
                        * @param offset - Offset in the input buffer to read from.
                        * @return - Number of bytes consumed.
                        */
                        virtual uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t *type = static_cast<uint8_t *>(ptr);
                            POSTCONDITION(*type == __constraint_type_utils::get_number_value(
                                    __constraint_type::CONSTRAINT_RANGE));

                            uint64_t r_size = sizeof(uint8_t);

                            // Read the min value first.
                            uint64_t off = offset + r_size;
                            __T *t = nullptr;
                            r_size += handler->read(buffer, &t, off, ULONG_MAX);
                            CHECK_NOT_NULL(t);
                            min_value = *t;

                            // Read the max value next.
                            t = nullptr;
                            off = offset + r_size;
                            r_size += handler->read(buffer, &t, off, ULONG_MAX);
                            CHECK_NOT_NULL(t);
                            max_value = *t;

                            return r_size;
                        }
                    };

                    typedef __range_constraint<char, __type_def_enum::TYPE_CHAR> char_range_constraint;
                    typedef __range_constraint<short, __type_def_enum::TYPE_SHORT> short_range_constraint;
                    typedef __range_constraint<int, __type_def_enum::TYPE_INTEGER> int_range_constraint;
                    typedef __range_constraint<long, __type_def_enum::TYPE_LONG> long_range_constraint;
                    typedef __range_constraint<float, __type_def_enum::TYPE_FLOAT> float_range_constraint;
                    typedef __range_constraint<double, __type_def_enum::TYPE_DOUBLE> double_range_constraint;
                    typedef __range_constraint<uint64_t, __type_def_enum::TYPE_TIMESTAMP> timestamp_range_constraint;
                    typedef __range_constraint<string, __type_def_enum::TYPE_STRING> string_range_constraint;


                    template<typename __T, __type_def_enum __type, __constraint_operator __operator>
                    class __oper_constraint : public __constraint {
                    private:
                        __constraint_operator oper = __operator;
                        __type_def_enum value_type = __type;
                        __T value;
                        __base_datatype_io *handler = nullptr;

                    protected:

                        __constraint_type get_constraint_type() {
                            switch (oper) {
                                case __constraint_operator::LTEQ:
                                    return __constraint_type::CONSTRAINT_LTEQ;
                                case __constraint_operator::LT:
                                    return __constraint_type::CONSTRAINT_LT;
                                case __constraint_operator::GTEQ:
                                    return __constraint_type::CONSTRAINT_GTEQ;
                                case __constraint_operator::GT:
                                    return __constraint_type::CONSTRAINT_GT;
                                default:
                                    throw BASE_ERROR("Unsupported constraint operator. [operator=%d]", oper);
                            }
                        }

                    public:

                        __oper_constraint() {
                            PRECONDITION(__type_enum_helper::is_native(value_type));
                            handler = __type_defs_utils::get_type_handler(value_type);
                            CHECK_NOT_NULL(handler);
                        }

                        virtual ~__oper_constraint() {

                        }

                        void set_value(__T value) {
                            this->value = value;
                        }

                        __type_def_enum get_datatype() override {
                            return this->value_type;
                        }

                        virtual bool validate(void *value) const override {
                            if (NOT_NULL(value)) {
                                CHECK_NOT_NULL(handler);

                                __T *t = (__T *) value;
                                CHECK_NOT_NULL(t);
                                bool r = (handler->compare(&(this->value), t, oper));
                                if (is_not)
                                    return !r;
                                return r;
                            }
                            return false;
                        }

                        virtual uint32_t write(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t type = __constraint_type_utils::get_number_value(get_constraint_type());
                            memcpy(ptr, &type, sizeof(uint8_t));

                            uint64_t w_size = sizeof(uint8_t);
                            uint64_t off = (offset + w_size);
                            w_size += handler->write(buffer, &value, off,
                                                     ULONG_MAX); // Note: Don't need to worry about buffer size here.

                            return w_size;
                        }

                        virtual uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t *type = static_cast<uint8_t *>(ptr);
                            POSTCONDITION(*type == __constraint_type_utils::get_number_value(get_constraint_type()));
                            ptr = common_utils::increment_data_ptr(ptr, sizeof(uint8_t));

                            uint64_t r_size = sizeof(uint8_t);

                            uint64_t off = offset + r_size;
                            __T *t = nullptr;
                            r_size += handler->read(buffer, &t, off, ULONG_MAX);
                            CHECK_NOT_NULL(t);
                            value = *t;

                            return r_size;
                        }
                    };

                    typedef __oper_constraint<char, __type_def_enum::TYPE_CHAR,
                            __constraint_operator::GT> char_gt_constraint;
                    typedef __oper_constraint<char, __type_def_enum::TYPE_CHAR,
                            __constraint_operator::GTEQ> char_gteq_constraint;
                    typedef __oper_constraint<char, __type_def_enum::TYPE_CHAR,
                            __constraint_operator::LT> char_lt_constraint;
                    typedef __oper_constraint<char, __type_def_enum::TYPE_CHAR,
                            __constraint_operator::LTEQ> char_lteq_constraint;

                    typedef __oper_constraint<short, __type_def_enum::TYPE_SHORT,
                            __constraint_operator::GT> short_gt_constraint;
                    typedef __oper_constraint<short, __type_def_enum::TYPE_SHORT,
                            __constraint_operator::GTEQ> short_gteq_constraint;
                    typedef __oper_constraint<short, __type_def_enum::TYPE_SHORT,
                            __constraint_operator::LT> short_lt_constraint;
                    typedef __oper_constraint<short, __type_def_enum::TYPE_SHORT,
                            __constraint_operator::LTEQ> short_lteq_constraint;


                    typedef __oper_constraint<int, __type_def_enum::TYPE_INTEGER,
                            __constraint_operator::GT> int_gt_constraint;
                    typedef __oper_constraint<int, __type_def_enum::TYPE_INTEGER,
                            __constraint_operator::GTEQ> int_gteq_constraint;
                    typedef __oper_constraint<int, __type_def_enum::TYPE_INTEGER,
                            __constraint_operator::LT> int_lt_constraint;
                    typedef __oper_constraint<int, __type_def_enum::TYPE_INTEGER,
                            __constraint_operator::LTEQ> int_lteq_constraint;

                    typedef __oper_constraint<long, __type_def_enum::TYPE_LONG,
                            __constraint_operator::GT> long_gt_constraint;
                    typedef __oper_constraint<long, __type_def_enum::TYPE_LONG,
                            __constraint_operator::GTEQ> long_gteq_constraint;
                    typedef __oper_constraint<long, __type_def_enum::TYPE_LONG,
                            __constraint_operator::LT> long_lt_constraint;
                    typedef __oper_constraint<long, __type_def_enum::TYPE_LONG,
                            __constraint_operator::LTEQ> long_lteq_constraint;

                    typedef __oper_constraint<float, __type_def_enum::TYPE_FLOAT,
                            __constraint_operator::GT> float_gt_constraint;
                    typedef __oper_constraint<float, __type_def_enum::TYPE_FLOAT,
                            __constraint_operator::GTEQ> float_gteq_constraint;
                    typedef __oper_constraint<float, __type_def_enum::TYPE_FLOAT,
                            __constraint_operator::LT> float_lt_constraint;
                    typedef __oper_constraint<float, __type_def_enum::TYPE_FLOAT,
                            __constraint_operator::LTEQ> float_lteq_constraint;

                    typedef __oper_constraint<double, __type_def_enum::TYPE_DOUBLE,
                            __constraint_operator::GT> double_gt_constraint;
                    typedef __oper_constraint<double, __type_def_enum::TYPE_DOUBLE,
                            __constraint_operator::GTEQ> double_gteq_constraint;
                    typedef __oper_constraint<double, __type_def_enum::TYPE_DOUBLE,
                            __constraint_operator::LT> double_lt_constraint;
                    typedef __oper_constraint<double, __type_def_enum::TYPE_DOUBLE,
                            __constraint_operator::LTEQ> double_lteq_constraint;

                    typedef __oper_constraint<uint64_t, __type_def_enum::TYPE_TIMESTAMP,
                            __constraint_operator::GT> timestamp_gt_constraint;
                    typedef __oper_constraint<uint64_t, __type_def_enum::TYPE_TIMESTAMP,
                            __constraint_operator::GTEQ> timestamp_gteq_constraint;
                    typedef __oper_constraint<uint64_t, __type_def_enum::TYPE_TIMESTAMP,
                            __constraint_operator::LT> timestamp_lt_constraint;
                    typedef __oper_constraint<uint64_t, __type_def_enum::TYPE_TIMESTAMP,
                            __constraint_operator::LTEQ> timestamp_lteq_constraint;

                    typedef __oper_constraint<string, __type_def_enum::TYPE_STRING,
                            __constraint_operator::GT> string_gt_constraint;
                    typedef __oper_constraint<string, __type_def_enum::TYPE_STRING,
                            __constraint_operator::GTEQ> string_gteq_constraint;
                    typedef __oper_constraint<string, __type_def_enum::TYPE_STRING,
                            __constraint_operator::LT> string_lt_constraint;
                    typedef __oper_constraint<string, __type_def_enum::TYPE_STRING,
                            __constraint_operator::LTEQ> string_lteq_constraint;

                    template<typename __T, __type_def_enum __type>
                    class __value_constraint : public __constraint {
                    private:
                        __type_def_enum value_type = __type;
                        vector<__T> values;
                        __base_datatype_io *handler = nullptr;

                    public:
                        __value_constraint() {
                            PRECONDITION(__type_enum_helper::is_native(value_type));
                            handler = __type_defs_utils::get_type_handler(value_type);
                            CHECK_NOT_NULL(handler);
                        }

                        virtual ~__value_constraint() {
                            values.clear();
                        }

                        void add_value(__T t) {
                            values.push_back(t);
                        }

                        __type_def_enum get_datatype() override {
                            return this->value_type;
                        }

                        virtual bool validate(void *value) const override {
                            if (NOT_NULL(value)) {
                                __T *t = (__T *) value;
                                CHECK_NOT_NULL(t);
                                for (auto iter = values.begin(); iter != values.end(); iter++) {
                                    if (*t == (*iter)) {
                                        if (is_not)
                                            return false;
                                        return true;
                                    }
                                }
                            }
                            return false;
                        }

                        virtual uint32_t write(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t type = __constraint_type_utils::get_number_value(__constraint_type::CONSTRAINT_IN);
                            memcpy(ptr, &type, sizeof(uint8_t));

                            uint16_t count = values.size();
                            uint64_t w_size = sizeof(uint8_t);
                            memcpy(ptr, &count, sizeof(uint16_t));
                            w_size += sizeof(uint16_t);
                            for (__T t : values) {
                                uint64_t off = (offset + w_size);
                                w_size += handler->write(buffer, &t, off,
                                                         ULONG_MAX); // Note: Don't need to worry about buffer size here.
                            }
                            return w_size;
                        }

                        virtual uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t *type = static_cast<uint8_t *>(ptr);
                            POSTCONDITION(*type ==
                                          __constraint_type_utils::get_number_value(__constraint_type::CONSTRAINT_IN));
                            ptr = common_utils::increment_data_ptr(ptr, sizeof(uint8_t));

                            uint16_t *count = static_cast<uint16_t *>(ptr);
                            uint64_t r_size = sizeof(uint16_t) + sizeof(uint8_t);
                            if (count > 0) {
                                for (uint16_t ii = 0; ii < *count; ii++) {
                                    uint64_t off = offset + r_size;
                                    __T *t = nullptr;
                                    r_size += handler->read(buffer, &t, off, ULONG_MAX);
                                    CHECK_NOT_NULL(t);

                                    values.push_back(*t);
                                }
                            }
                            return r_size;
                        }
                    };

                    typedef __value_constraint<char, __type_def_enum::TYPE_CHAR> char_value_constraint;
                    typedef __value_constraint<short, __type_def_enum::TYPE_SHORT> short_value_constraint;
                    typedef __value_constraint<int, __type_def_enum::TYPE_INTEGER> int_value_constraint;
                    typedef __value_constraint<long, __type_def_enum::TYPE_LONG> long_value_constraint;
                    typedef __value_constraint<float, __type_def_enum::TYPE_FLOAT> float_value_constraint;
                    typedef __value_constraint<uint64_t, __type_def_enum::TYPE_TIMESTAMP> timestamp_value_constraint;
                    typedef __value_constraint<double, __type_def_enum::TYPE_DOUBLE> double_value_constraint;
                    typedef __value_constraint<string, __type_def_enum::TYPE_STRING> string_value_constraint;


                    class __constraint_loader {
                    private:
                        static __constraint *
                        read_operator_gt(void *buffer, uint64_t offset, __type_def_enum datatype, uint64_t *size) {
                            __constraint *c = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_CHAR:
                                    c = new char_gt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(char_gt_constraint));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    c = new short_gt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(short_gt_constraint));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    c = new int_gt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(int_gt_constraint));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    c = new long_gt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(long_gt_constraint));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    c = new float_gt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(float_gt_constraint));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    c = new double_gt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(double_gt_constraint));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    c = new timestamp_gt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(timestamp_gt_constraint));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    c = new string_gt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(string_gt_constraint));
                                    break;
                                default:
                                    throw BASE_ERROR("Constraints can only be defined for basic types.");
                            }
                            CHECK_NOT_NULL(c);
                            *size = c->read(buffer, offset);
                            return c;
                        }

                        static __constraint *
                        create_operator_gt(string *values, __type_def_enum datatype) {
                            CHECK_NOT_EMPTY_P(values);
                            __constraint *c = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_CHAR: {
                                    char_gt_constraint *cc = new char_gt_constraint();
                                    CHECK_ALLOC(cc, TYPE_NAME(char_gt_constraint));
                                    char ch = (*values)[0];
                                    cc->set_value(ch);
                                    c = cc;
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    short_gt_constraint *cs = new short_gt_constraint();
                                    CHECK_ALLOC(cs, TYPE_NAME(short_gt_constraint));
                                    int is = std::stoi(*values);
                                    POSTCONDITION(is >= std::numeric_limits<short>::min() &&
                                                  is <= std::numeric_limits<short>::max());
                                    short s = (short) is;
                                    cs->set_value(s);
                                    c = cs;
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    int_gt_constraint *ci = new int_gt_constraint();
                                    CHECK_ALLOC(ci, TYPE_NAME(int_gt_constraint));
                                    int ii = std::stoi(*values);
                                    ci->set_value(ii);
                                    c = ci;
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    long_gt_constraint *cl = new long_gt_constraint();
                                    CHECK_ALLOC(cl, TYPE_NAME(long_gt_constraint));
                                    long ll = std::stol(*values);
                                    cl->set_value(ll);
                                    c = cl;
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    float_gt_constraint *cf = new float_gt_constraint();
                                    CHECK_ALLOC(cf, TYPE_NAME(float_gt_constraint));
                                    float fv = std::stof(*values);
                                    cf->set_value(fv);
                                    c = cf;
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    double_gt_constraint *cd = new double_gt_constraint();
                                    CHECK_ALLOC(cd, TYPE_NAME(double_gt_constraint));
                                    double dd = std::stod(*values);
                                    cd->set_value(dd);
                                    c = cd;
                                }
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    timestamp_gt_constraint *ct = new timestamp_gt_constraint();
                                    CHECK_ALLOC(ct, TYPE_NAME(timestamp_gt_constraint));
                                    long lu = std::stol(*values);
                                    uint64_t ul = (uint64_t) lu;
                                    ct->set_value(ul);
                                    c = ct;
                                }
                                    break;
                                case __type_def_enum::TYPE_STRING: {
                                    string_gt_constraint *cst = new string_gt_constraint();
                                    CHECK_ALLOC(cst, TYPE_NAME(string_gt_constraint));
                                    cst->set_value(*values);
                                    c = cst;
                                }
                                    break;
                                default:
                                    throw BASE_ERROR("Constraints can only be defined for basic types.");
                            }
                            CHECK_NOT_NULL(c);
                            return c;
                        }

                        static __constraint *
                        read_operator_gteq(void *buffer, uint64_t offset, __type_def_enum datatype, uint64_t *size) {
                            __constraint *c = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_CHAR:
                                    c = new char_gteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(char_gteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    c = new short_gteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(short_gteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    c = new int_gteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(int_gteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    c = new long_gteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(long_gteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    c = new float_gteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(float_gteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    c = new double_gteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(double_gteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    c = new timestamp_gteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(timestamp_gteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    c = new string_gteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(string_gteq_constraint));
                                    break;
                                default:
                                    throw BASE_ERROR("Constraints can only be defined for basic types.");
                            }
                            CHECK_NOT_NULL(c);
                            *size = c->read(buffer, offset);
                            return c;
                        }

                        static __constraint *
                        create_operator_gteq(string *values, __type_def_enum datatype) {
                            __constraint *c = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_CHAR: {
                                    char_gteq_constraint *cc = new char_gteq_constraint();
                                    CHECK_ALLOC(cc, TYPE_NAME(char_gteq_constraint));
                                    char ch = (*values)[0];
                                    cc->set_value(ch);
                                    c = cc;
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    short_gteq_constraint *cs = new short_gteq_constraint();
                                    CHECK_ALLOC(cs, TYPE_NAME(short_gteq_constraint));
                                    int is = std::stoi(*values);
                                    POSTCONDITION(is >= std::numeric_limits<short>::min() &&
                                                  is <= std::numeric_limits<short>::max());
                                    short s = (short) is;
                                    cs->set_value(s);
                                    c = cs;
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    int_gteq_constraint *ci = new int_gteq_constraint();
                                    CHECK_ALLOC(ci, TYPE_NAME(int_gteq_constraint));
                                    int ii = std::stoi(*values);
                                    ci->set_value(ii);
                                    c = ci;
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    long_gteq_constraint *cl = new long_gteq_constraint();
                                    CHECK_ALLOC(cl, TYPE_NAME(long_gteq_constraint));
                                    long ll = std::stol(*values);
                                    cl->set_value(ll);
                                    c = cl;
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    float_gteq_constraint *cf = new float_gteq_constraint();
                                    CHECK_ALLOC(cf, TYPE_NAME(float_gteq_constraint));
                                    float fv = std::stof(*values);
                                    cf->set_value(fv);
                                    c = cf;
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    double_gteq_constraint *cd = new double_gteq_constraint();
                                    CHECK_ALLOC(cd, TYPE_NAME(double_gteq_constraint));
                                    double dd = std::stod(*values);
                                    cd->set_value(dd);
                                    c = cd;
                                }
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    timestamp_gteq_constraint *ct = new timestamp_gteq_constraint();
                                    CHECK_ALLOC(ct, TYPE_NAME(timestamp_gteq_constraint));
                                    long lu = std::stol(*values);
                                    uint64_t ul = (uint64_t) lu;
                                    ct->set_value(ul);
                                    c = ct;
                                }
                                    break;
                                case __type_def_enum::TYPE_STRING: {
                                    string_gteq_constraint *cst = new string_gteq_constraint();
                                    CHECK_ALLOC(cst, TYPE_NAME(string_gteq_constraint));
                                    cst->set_value(*values);
                                    c = cst;
                                }
                                    break;
                                default:
                                    throw BASE_ERROR("Constraints can only be defined for basic types.");
                            }
                            CHECK_NOT_NULL(c);
                            return c;
                        }

                        static __constraint *
                        read_operator_lt(void *buffer, uint64_t offset, __type_def_enum datatype, uint64_t *size) {
                            __constraint *c = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_CHAR:
                                    c = new char_lt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(char_lt_constraint));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    c = new short_lt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(short_lt_constraint));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    c = new int_lt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(int_lt_constraint));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    c = new long_lt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(long_lt_constraint));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    c = new float_lt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(float_lt_constraint));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    c = new double_lt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(double_lt_constraint));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    c = new timestamp_lt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(timestamp_lt_constraint));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    c = new string_lt_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(string_lt_constraint));
                                    break;
                                default:
                                    throw BASE_ERROR("Constraints can only be defined for basic types.");
                            }
                            CHECK_NOT_NULL(c);
                            *size = c->read(buffer, offset);
                            return c;
                        }

                        static __constraint *
                        create_operator_lt(string *values, __type_def_enum datatype) {
                            __constraint *c = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_CHAR: {
                                    char_lt_constraint *cc = new char_lt_constraint();
                                    CHECK_ALLOC(cc, TYPE_NAME(char_lt_constraint));
                                    char ch = (*values)[0];
                                    cc->set_value(ch);
                                    c = cc;
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    short_lt_constraint *cs = new short_lt_constraint();
                                    CHECK_ALLOC(cs, TYPE_NAME(short_lt_constraint));
                                    int is = std::stoi(*values);
                                    POSTCONDITION(is >= std::numeric_limits<short>::min() &&
                                                  is <= std::numeric_limits<short>::max());
                                    short s = (short) is;
                                    cs->set_value(s);
                                    c = cs;
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    int_lt_constraint *ci = new int_lt_constraint();
                                    CHECK_ALLOC(ci, TYPE_NAME(int_lt_constraint));
                                    int ii = std::stoi(*values);
                                    ci->set_value(ii);
                                    c = ci;
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    long_lt_constraint *cl = new long_lt_constraint();
                                    CHECK_ALLOC(cl, TYPE_NAME(long_lt_constraint));
                                    long ll = std::stol(*values);
                                    cl->set_value(ll);
                                    c = cl;
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    float_lt_constraint *cf = new float_lt_constraint();
                                    CHECK_ALLOC(cf, TYPE_NAME(float_lt_constraint));
                                    float fv = std::stof(*values);
                                    cf->set_value(fv);
                                    c = cf;
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    double_lt_constraint *cd = new double_lt_constraint();
                                    CHECK_ALLOC(cd, TYPE_NAME(double_lt_constraint));
                                    double dd = std::stod(*values);
                                    cd->set_value(dd);
                                    c = cd;
                                }
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    timestamp_lt_constraint *ct = new timestamp_lt_constraint();
                                    CHECK_ALLOC(ct, TYPE_NAME(timestamp_lt_constraint));
                                    long lu = std::stol(*values);
                                    uint64_t ul = (uint64_t) lu;
                                    ct->set_value(ul);
                                    c = ct;
                                }
                                    break;
                                case __type_def_enum::TYPE_STRING: {
                                    string_lt_constraint *cst = new string_lt_constraint();
                                    CHECK_ALLOC(cst, TYPE_NAME(string_lt_constraint));
                                    cst->set_value(*values);
                                    c = cst;
                                }
                                    break;
                                default:
                                    throw BASE_ERROR("Constraints can only be defined for basic types.");
                            }
                            CHECK_NOT_NULL(c);
                            return c;
                        }

                        static __constraint *
                        read_operator_lteq(void *buffer, uint64_t offset, __type_def_enum datatype, uint64_t *size) {
                            __constraint *c = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_CHAR:
                                    c = new char_lteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(char_lteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    c = new short_lteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(short_lteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    c = new int_lteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(int_lteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    c = new long_lteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(long_lteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    c = new float_lteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(float_lteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    c = new double_lteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(double_lteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    c = new timestamp_lteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(timestamp_lteq_constraint));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    c = new string_lteq_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(string_lteq_constraint));
                                    break;
                                default:
                                    throw BASE_ERROR("Constraints can only be defined for basic types.");
                            }
                            CHECK_NOT_NULL(c);
                            *size = c->read(buffer, offset);
                            return c;
                        }

                        static __constraint *
                        create_operator_lteq(string *values, __type_def_enum datatype) {
                            __constraint *c = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_CHAR: {
                                    char_lteq_constraint *cc = new char_lteq_constraint();
                                    CHECK_ALLOC(cc, TYPE_NAME(char_lteq_constraint));
                                    char ch = (*values)[0];
                                    cc->set_value(ch);
                                    c = cc;
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    short_lteq_constraint *cs = new short_lteq_constraint();
                                    CHECK_ALLOC(cs, TYPE_NAME(short_lteq_constraint));
                                    int is = std::stoi(*values);
                                    POSTCONDITION(is >= std::numeric_limits<short>::min() &&
                                                  is <= std::numeric_limits<short>::max());
                                    short s = (short) is;
                                    cs->set_value(s);
                                    c = cs;
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    int_lteq_constraint *ci = new int_lteq_constraint();
                                    CHECK_ALLOC(ci, TYPE_NAME(int_lteq_constraint));
                                    int ii = std::stoi(*values);
                                    ci->set_value(ii);
                                    c = ci;
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    long_lteq_constraint *cl = new long_lteq_constraint();
                                    CHECK_ALLOC(cl, TYPE_NAME(long_lteq_constraint));
                                    long ll = std::stol(*values);
                                    cl->set_value(ll);
                                    c = cl;
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    float_lteq_constraint *cf = new float_lteq_constraint();
                                    CHECK_ALLOC(cf, TYPE_NAME(float_lteq_constraint));
                                    float fv = std::stof(*values);
                                    cf->set_value(fv);
                                    c = cf;
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    double_lteq_constraint *cd = new double_lteq_constraint();
                                    CHECK_ALLOC(cd, TYPE_NAME(double_lteq_constraint));
                                    double dd = std::stod(*values);
                                    cd->set_value(dd);
                                    c = cd;
                                }
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    timestamp_lteq_constraint *ct = new timestamp_lteq_constraint();
                                    CHECK_ALLOC(ct, TYPE_NAME(timestamp_lteq_constraint));
                                    long lu = std::stol(*values);
                                    uint64_t ul = (uint64_t) lu;
                                    ct->set_value(ul);
                                    c = ct;
                                }
                                    break;
                                case __type_def_enum::TYPE_STRING: {
                                    string_lteq_constraint *cst = new string_lteq_constraint();
                                    CHECK_ALLOC(cst, TYPE_NAME(string_lteq_constraint));
                                    cst->set_value(*values);
                                    c = cst;
                                }
                                    break;
                                default:
                                    throw BASE_ERROR("Constraints can only be defined for basic types.");
                            }
                            CHECK_NOT_NULL(c);
                            return c;
                        }

                    public:
                        static __constraint *
                        read(void *buffer, uint64_t offset, __type_def_enum datatype, uint64_t *size) {
                            CHECK_NOT_NULL(buffer);
                            __constraint *c = nullptr;
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t *type = static_cast<uint8_t *>(ptr);
                            if (*type == __constraint_type_utils::get_number_value(__constraint_type::CONSTRAINT_IN)) {
                                c = read_value(buffer, offset, datatype, size);
                            } else if (*type ==
                                       __constraint_type_utils::get_number_value(__constraint_type::CONSTRAINT_RANGE)) {
                                c = read_range(buffer, offset, datatype, size);
                            } else if (*type ==
                                       __constraint_type_utils::get_number_value(__constraint_type::CONSTRAINT_REGEX)) {
                                c = read_regex(buffer, offset, size);
                            } else if (*type == __constraint_type_utils::get_number_value(
                                    __constraint_type::CONSTRAINT_NOT_NULLABLE)) {
                                c = read_nullable(buffer, offset, size);
                            } else if (*type ==
                                       __constraint_type_utils::get_number_value(__constraint_type::CONSTRAINT_LTEQ)) {
                                c = read_operator(buffer, offset, datatype, __constraint_operator::LTEQ, size);
                            } else if (*type ==
                                       __constraint_type_utils::get_number_value(__constraint_type::CONSTRAINT_LT)) {
                                c = read_operator(buffer, offset, datatype, __constraint_operator::LT, size);
                            } else if (*type ==
                                       __constraint_type_utils::get_number_value(__constraint_type::CONSTRAINT_GTEQ)) {
                                c = read_operator(buffer, offset, datatype, __constraint_operator::GTEQ, size);
                            } else if (*type ==
                                       __constraint_type_utils::get_number_value(__constraint_type::CONSTRAINT_LT)) {
                                c = read_operator(buffer, offset, datatype, __constraint_operator::LT, size);
                            }
                            CHECK_NOT_NULL(c);
                            *size += sizeof(uint8_t);
                            return c;
                        }

                        static __constraint *
                        create(string *constraint, string *values, __type_def_enum datatype, bool is_not) {
                            CHECK_NOT_EMPTY_P(constraint);
                            __constraint *c = nullptr;
                            __constraint_type type = __constraint_type_utils::parse(*constraint);
                            switch (type) {
                                case __constraint_type::CONSTRAINT_NOT_NULLABLE:
                                    c = create_nullable();
                                    break;
                                case __constraint_type::CONSTRAINT_REGEX:
                                    c = create_regex(*values);
                                    break;
                                case __constraint_type::CONSTRAINT_GT:
                                    c = create_operator(values, datatype, __constraint_operator::GT);
                                    break;
                                case __constraint_type::CONSTRAINT_LT:
                                    c = create_operator(values, datatype, __constraint_operator::LT);
                                    break;
                                case __constraint_type::CONSTRAINT_GTEQ:
                                    c = create_operator(values, datatype, __constraint_operator::GTEQ);
                                    break;
                                case __constraint_type::CONSTRAINT_LTEQ:
                                    c = create_operator(values, datatype, __constraint_operator::LTEQ);
                                    break;
                                case __constraint_type::CONSTRAINT_IN:
                                    c = create_value(values, datatype);
                                    break;
                                case __constraint_type::CONSTRAINT_RANGE:
                                    c = create_value(values, datatype);
                                default:
                                    throw BASE_ERROR("Constraint type not recognized. [type=%s]", constraint->c_str());

                            }
                            CHECK_NOT_NULL(c);
                            c->set_not(is_not);
                            return c;
                        }

                        static __constraint *create_nullable() {
                            __notnull *c = new __notnull();
                            CHECK_ALLOC(c, TYPE_NAME(__notnull));
                            return c;
                        }

                        static __constraint *read_nullable(void *buffer, uint64_t offset, uint64_t *size) {
                            __notnull *c = new __notnull();
                            CHECK_ALLOC(c, TYPE_NAME(__notnull));
                            *size = c->read(buffer, offset);
                            return c;
                        }

                        static __constraint *create_regex(string pattern) {
                            __regex *c = new __regex();
                            CHECK_ALLOC(c, TYPE_NAME(__regex));
                            c->set_pattern(pattern);
                            return c;
                        }

                        static __constraint *read_regex(void *buffer, uint64_t offset, uint64_t *size) {
                            __regex *c = new __regex();
                            CHECK_ALLOC(c, TYPE_NAME(__regex));
                            *size = c->read(buffer, offset);
                            return c;
                        }

                        static __constraint *
                        read_value(void *buffer, uint64_t offset, __type_def_enum datatype, uint64_t *size) {
                            __constraint *c = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_CHAR:
                                    c = new char_value_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(char_value_constraint));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    c = new short_value_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(short_value_constraint));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    c = new int_value_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(int_value_constraint));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    c = new long_value_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(long_value_constraint));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    c = new float_value_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(float_value_constraint));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    c = new double_value_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(double_value_constraint));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    c = new timestamp_value_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(timestamp_value_constraint));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    c = new string_value_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(string_value_constraint));
                                    break;
                                default:
                                    throw BASE_ERROR("Constraints can only be defined for basic types.");
                            }
                            CHECK_NOT_NULL(c);
                            *size = c->read(buffer, offset);
                            return c;
                        }

                        static __constraint *
                        create_value(string *values, __type_def_enum datatype) {
                            __constraint *c = nullptr;
                            vector<string> vlist;
                            switch (datatype) {
                                case __type_def_enum::TYPE_CHAR: {
                                    char_value_constraint *cc = new char_value_constraint();
                                    CHECK_ALLOC(cc, TYPE_NAME(char_value_constraint));
                                    string_utils::split(*values, ',', &vlist);
                                    CHECK_NOT_EMPTY(vlist);
                                    for (string vs : vlist) {
                                        vs = string_utils::trim(vs);
                                        CHECK_NOT_EMPTY(vs);
                                        char ch = vs[0];
                                        cc->add_value(ch);
                                    }
                                    c = cc;
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    short_value_constraint *cs = new short_value_constraint();
                                    CHECK_ALLOC(cs, TYPE_NAME(short_value_constraint));
                                    string_utils::split(*values, ',', &vlist);
                                    CHECK_NOT_EMPTY(vlist);
                                    for (string vs : vlist) {
                                        vs = string_utils::trim(vs);
                                        CHECK_NOT_EMPTY(vs);
                                        int is = std::stoi(vs);
                                        POSTCONDITION(is >= std::numeric_limits<short>::min() &&
                                                      is <= std::numeric_limits<short>::max());
                                        short s = (short) is;
                                        cs->add_value(s);
                                    }
                                    c = cs;
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    int_value_constraint *ci = new int_value_constraint();
                                    CHECK_ALLOC(ci, TYPE_NAME(int_value_constraint));
                                    string_utils::split(*values, ',', &vlist);
                                    CHECK_NOT_EMPTY(vlist);
                                    for (string vs : vlist) {
                                        vs = string_utils::trim(vs);
                                        CHECK_NOT_EMPTY(vs);
                                        int is = std::stoi(vs);
                                        ci->add_value(is);
                                    }
                                    c = ci;
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    long_value_constraint *cl = new long_value_constraint();
                                    CHECK_ALLOC(cl, TYPE_NAME(long_value_constraint));
                                    string_utils::split(*values, ',', &vlist);
                                    CHECK_NOT_EMPTY(vlist);
                                    for (string vs : vlist) {
                                        vs = string_utils::trim(vs);
                                        CHECK_NOT_EMPTY(vs);
                                        long ll = std::stol(vs);
                                        cl->add_value(ll);
                                    }
                                    c = cl;
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    float_value_constraint *cf = new float_value_constraint();
                                    CHECK_ALLOC(cf, TYPE_NAME(float_value_constraint));
                                    string_utils::split(*values, ',', &vlist);
                                    CHECK_NOT_EMPTY(vlist);
                                    for (string vs : vlist) {
                                        vs = string_utils::trim(vs);
                                        CHECK_NOT_EMPTY(vs);
                                        float fv = std::stof(vs);
                                        cf->add_value(fv);
                                    }
                                    c = cf;
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    double_value_constraint *cd = new double_value_constraint();
                                    CHECK_ALLOC(cd, TYPE_NAME(double_value_constraint));
                                    string_utils::split(*values, ',', &vlist);
                                    CHECK_NOT_EMPTY(vlist);
                                    for (string vs : vlist) {
                                        vs = string_utils::trim(vs);
                                        CHECK_NOT_EMPTY(vs);
                                        double dd = std::stod(vs);
                                        cd->add_value(dd);
                                    }
                                    c = cd;
                                }
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    timestamp_value_constraint *ct = new timestamp_value_constraint();
                                    CHECK_ALLOC(ct, TYPE_NAME(timestamp_value_constraint));
                                    string_utils::split(*values, ',', &vlist);
                                    CHECK_NOT_EMPTY(vlist);
                                    for (string vs : vlist) {
                                        vs = string_utils::trim(vs);
                                        CHECK_NOT_EMPTY(vs);
                                        long lu = std::stol(vs);
                                        uint64_t ul = (uint64_t) lu;
                                        ct->add_value(ul);
                                    }
                                    c = ct;
                                }
                                    break;
                                case __type_def_enum::TYPE_STRING: {
                                    string_value_constraint *cst = new string_value_constraint();
                                    CHECK_ALLOC(cst, TYPE_NAME(string_value_constraint));
                                    string_utils::split(*values, ',', &vlist);
                                    CHECK_NOT_EMPTY(vlist);
                                    for (string vs : vlist) {
                                        vs = string_utils::trim(vs);
                                        CHECK_NOT_EMPTY(vs);
                                        cst->add_value(vs);
                                    }
                                    c = cst;
                                }
                                    break;
                                default:
                                    throw BASE_ERROR("Constraints can only be defined for basic types.");
                            }
                            CHECK_NOT_NULL(c);
                            return c;
                        }

                        static __constraint *
                        read_range(void *buffer, uint64_t offset, __type_def_enum datatype, uint64_t *size) {
                            __constraint *c = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_CHAR:
                                    c = new char_range_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(char_range_constraint));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    c = new short_range_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(short_range_constraint));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    c = new int_range_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(int_range_constraint));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    c = new long_range_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(long_range_constraint));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    c = new float_range_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(float_range_constraint));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    c = new double_range_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(double_range_constraint));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    c = new timestamp_range_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(timestamp_range_constraint));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    c = new string_range_constraint();
                                    CHECK_ALLOC(c, TYPE_NAME(string_range_constraint));
                                    break;
                                default:
                                    throw BASE_ERROR("Constraints can only be defined for basic types.");
                            }
                            CHECK_NOT_NULL(c);
                            *size = c->read(buffer, offset);
                            return c;
                        }

                        static __constraint *
                        create_range(string *values, __type_def_enum datatype) {
                            __constraint *c = nullptr;
                            string array[2];
                            string v_min = common_consts::EMPTY_STRING;
                            string v_max = common_consts::EMPTY_STRING;
                            switch (datatype) {
                                case __type_def_enum::TYPE_CHAR: {
                                    char_range_constraint *cc = new char_range_constraint();
                                    CHECK_ALLOC(cc, TYPE_NAME(char_range_constraint));
                                    string_utils::split(*values, ',', array, 2);
                                    v_min = array[0];
                                    v_max = array[1];
                                    char c_min = std::numeric_limits<char>::min();
                                    char c_max = std::numeric_limits<char>::max();
                                    if (!IS_EMPTY(v_min)) {
                                        c_min = v_min[0];
                                    }
                                    if (!IS_EMPTY(v_max)) {
                                        c_max = v_max[0];
                                    }
                                    cc->set_min_value(c_min);
                                    cc->set_max_value(c_max);
                                    c = cc;
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    short_range_constraint *cs = new short_range_constraint();
                                    CHECK_ALLOC(cs, TYPE_NAME(short_range_constraint));
                                    string_utils::split(*values, ',', array, 2);
                                    v_min = array[0];
                                    v_max = array[1];
                                    short s_min = std::numeric_limits<short>::min();
                                    short s_max = std::numeric_limits<short>::max();
                                    if (!IS_EMPTY(v_min)) {
                                        int is = std::stoi(v_min);
                                        POSTCONDITION(is >= std::numeric_limits<short>::min() &&
                                                      is <= std::numeric_limits<short>::max());
                                        s_min = (short) is;
                                    }
                                    if (!IS_EMPTY(v_max)) {
                                        int is = std::stoi(v_max);
                                        POSTCONDITION(is >= std::numeric_limits<short>::min() &&
                                                      is <= std::numeric_limits<short>::max());
                                        s_max = (short) is;
                                    }
                                    cs->set_min_value(s_min);
                                    cs->set_max_value(s_max);
                                    c = cs;
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    int_range_constraint *ci = new int_range_constraint();
                                    CHECK_ALLOC(ci, TYPE_NAME(int_range_constraint));
                                    string_utils::split(*values, ',', array, 2);
                                    v_min = array[0];
                                    v_max = array[1];
                                    int i_min = std::numeric_limits<int>::min();
                                    int i_max = std::numeric_limits<int>::max();
                                    if (!IS_EMPTY(v_min)) {
                                        i_min = std::stoi(v_min);
                                    }
                                    if (!IS_EMPTY(v_max)) {
                                        i_max = std::stoi(v_max);
                                    }
                                    ci->set_min_value(i_min);
                                    ci->set_max_value(i_max);
                                    c = ci;
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    long_range_constraint *cl = new long_range_constraint();
                                    CHECK_ALLOC(cl, TYPE_NAME(long_range_constraint));
                                    string_utils::split(*values, ',', array, 2);
                                    v_min = array[0];
                                    v_max = array[1];
                                    long l_min = std::numeric_limits<long>::min();
                                    long l_max = std::numeric_limits<long>::max();
                                    if (!IS_EMPTY(v_min)) {
                                        l_min = std::stol(v_min);
                                    }
                                    if (!IS_EMPTY(v_max)) {
                                        l_max = std::stol(v_max);
                                    }
                                    cl->set_min_value(l_min);
                                    cl->set_max_value(l_max);
                                    c = cl;
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    float_range_constraint *cf = new float_range_constraint();
                                    CHECK_ALLOC(cf, TYPE_NAME(float_range_constraint));
                                    string_utils::split(*values, ',', array, 2);
                                    v_min = array[0];
                                    v_max = array[1];
                                    float f_min = std::numeric_limits<float>::min();
                                    float f_max = std::numeric_limits<float>::max();
                                    if (!IS_EMPTY(v_min)) {
                                        f_min = std::stof(v_min);
                                    }
                                    if (!IS_EMPTY(v_max)) {
                                        f_max = std::stof(v_max);
                                    }
                                    cf->set_min_value(f_min);
                                    cf->set_max_value(f_max);
                                    c = cf;
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    double_range_constraint *cd = new double_range_constraint();
                                    CHECK_ALLOC(cd, TYPE_NAME(double_range_constraint));
                                    string_utils::split(*values, ',', array, 2);
                                    v_min = array[0];
                                    v_max = array[1];
                                    double d_min = std::numeric_limits<double>::min();
                                    double d_max = std::numeric_limits<double>::max();
                                    if (!IS_EMPTY(v_min)) {
                                        d_min = std::stod(v_min);
                                    }
                                    if (!IS_EMPTY(v_max)) {
                                        d_max = std::stod(v_max);
                                    }
                                    cd->set_min_value(d_min);
                                    cd->set_max_value(d_max);
                                    c = cd;
                                }
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    timestamp_range_constraint *ct = new timestamp_range_constraint();
                                    CHECK_ALLOC(ct, TYPE_NAME(timestamp_range_constraint));
                                    string_utils::split(*values, ',', array, 2);
                                    v_min = array[0];
                                    v_max = array[1];
                                    uint64_t t_min = std::numeric_limits<uint64_t>::min();
                                    uint64_t t_max = std::numeric_limits<uint64_t>::max();
                                    if (!IS_EMPTY(v_min)) {
                                        t_min = (uint64_t) std::stol(v_min);
                                    }
                                    if (!IS_EMPTY(v_max)) {
                                        t_max = (uint64_t) std::stol(v_max);
                                    }
                                    ct->set_min_value(t_min);
                                    ct->set_max_value(t_max);
                                    c = ct;
                                }
                                    break;
                                case __type_def_enum::TYPE_STRING: {
                                    string_range_constraint *cst = new string_range_constraint();
                                    CHECK_ALLOC(cst, TYPE_NAME(string_range_constraint));
                                    string_utils::split(*values, ',', array, 2);
                                    v_min = array[0];
                                    v_max = array[1];
                                    cst->set_min_value(array[0]);
                                    cst->set_max_value(array[1]);
                                    c = cst;
                                }
                                    break;
                                default:
                                    throw BASE_ERROR("Constraints can only be defined for basic types.");
                            }
                            CHECK_NOT_NULL(c);
                            return c;
                        }

                        static __constraint *read_operator(void *buffer, uint64_t offset, __type_def_enum datatype,
                                                           __constraint_operator oper, uint64_t *size) {
                            switch (oper) {
                                case __constraint_operator::GT:
                                    return read_operator_gt(buffer, offset, datatype, size);
                                case __constraint_operator::GTEQ:
                                    return read_operator_gteq(buffer, offset, datatype, size);
                                case __constraint_operator::LT:
                                    return read_operator_lt(buffer, offset, datatype, size);
                                case __constraint_operator::LTEQ:
                                    return read_operator_lteq(buffer, offset, datatype, size);
                                default:
                                    throw BASE_ERROR("Equals constraint is meaningless.");
                            }
                            return nullptr;
                        }

                        static __constraint *create_operator(string *values, __type_def_enum datatype,
                                                             __constraint_operator oper) {
                            switch (oper) {
                                case __constraint_operator::GT:
                                    return create_operator_gt(values, datatype);
                                case __constraint_operator::GTEQ:
                                    return create_operator_gteq(values, datatype);
                                case __constraint_operator::LT:
                                    return create_operator_lt(values, datatype);
                                case __constraint_operator::LTEQ:
                                    return create_operator_lteq(values, datatype);
                                default:
                                    throw BASE_ERROR("Equals constraint is meaningless.");
                            }
                            return nullptr;
                        }
                    };

                }
REACTFS_NS_CORE_END
#endif //REACTFS_CONSTRAINTS_H
