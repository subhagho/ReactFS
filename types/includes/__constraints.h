
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
#include "common/includes/buffer_utils.h"
#include "core/includes/core.h"

#include "types_common.h"
#include "schema_common.h"

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
                            throw BASE_ERROR("Unknown constraint type. [type=%d]", type);
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
                                    return "gt";
                                case __constraint_type::CONSTRAINT_LT:
                                    return "lt";
                                case __constraint_type::CONSTRAINT_GTEQ:
                                    return "gteq";
                                case __constraint_type::CONSTRAINT_LTEQ:
                                    return "lteq";
                            }
                            throw BASE_ERROR("Unknown constraint type. [type=%d]", type);
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
                            uint8_t type = __constraint_type_utils::get_number_value(
                                    __constraint_type::CONSTRAINT_NOT_NULLABLE);
                            return buffer_utils::write<uint8_t>(buffer, &offset, type);
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
                            uint8_t *type = nullptr;
                            uint64_t r_size = buffer_utils::read<uint8_t>(buffer, &offset, &type);
                            CHECK_NOT_NULL(type);
                            POSTCONDITION(*type ==
                                          __constraint_type_utils::get_number_value(
                                                  __constraint_type::CONSTRAINT_NOT_NULLABLE));

                            return r_size;
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

                        void print() const override {
                            string n = (is_not ? "NOT" : "");
                            LOG_DEBUG("\tconstraint : %s nullable", n.c_str());
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
                            uint64_t off = offset;
                            uint8_t type = __constraint_type_utils::get_number_value(
                                    __constraint_type::CONSTRAINT_REGEX);
                            uint16_t w_size = buffer_utils::write<uint8_t>(buffer, &off, type);
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
                            uint64_t off = offset;
                            uint8_t *type = nullptr;
                            uint16_t r_size = buffer_utils::read<uint8_t>(buffer, &off, &type);
                            CHECK_NOT_NULL(type);
                            POSTCONDITION(*type ==
                                          __constraint_type_utils::get_number_value(
                                                  __constraint_type::CONSTRAINT_REGEX));
                            string *value = nullptr;
                            r_size += handler->read(buffer, &value, (offset + r_size), ULONG_MAX);
                            CHECK_NOT_NULL(value);
                            set_pattern(*value);
                            CHECK_AND_FREE(value);

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

                        void print() const override {
                            string n = (is_not ? "NOT" : "");
                            LOG_DEBUG("\t constraint: %s regex(%s)", n.c_str(), pattern.c_str());
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
                    protected:
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

                            uint64_t off = offset;
                            uint8_t type = __constraint_type_utils::get_number_value(
                                    __constraint_type::CONSTRAINT_RANGE);
                            uint64_t w_size = buffer_utils::write<uint8_t>(buffer, &off, type);

                            // Write the min value first.
                            off = (offset + w_size);
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

                            uint64_t off = offset;
                            uint8_t *type = nullptr;
                            uint16_t r_size = buffer_utils::read<uint8_t>(buffer, &off, &type);
                            CHECK_NOT_NULL(type);
                            POSTCONDITION(*type == __constraint_type_utils::get_number_value(
                                    __constraint_type::CONSTRAINT_RANGE));

                            // Read the min value first.
                            off = offset + r_size;
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

                        void print() const override {
                            string n = (is_not ? "NOT" : "");
                            string t = __type_enum_helper::get_type_string(this->value_type);
                            string rmin = __type_enum_helper::get_string_value(&min_value, this->value_type);
                            string rmax = __type_enum_helper::get_string_value(&max_value, this->value_type);

                            LOG_DEBUG("\tconstraint : %s range[%s, %s] datatype=%s", n.c_str(), rmin.c_str(),
                                     rmax.c_str(), t.c_str());
                        }
                    };

                    class string_range_constraint : public __range_constraint<string, __type_def_enum::TYPE_STRING> {
                    public:
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

                            uint64_t off = offset;
                            uint8_t *type = nullptr;
                            uint16_t r_size = buffer_utils::read<uint8_t>(buffer, &off, &type);
                            CHECK_NOT_NULL(type);
                            POSTCONDITION(*type == __constraint_type_utils::get_number_value(
                                    __constraint_type::CONSTRAINT_RANGE));

                            // Read the min value first.
                            off = offset + r_size;
                            string *t = nullptr;
                            r_size += handler->read(buffer, &t, off, ULONG_MAX);
                            CHECK_NOT_NULL(t);
                            min_value = string(*t);
                            CHECK_AND_FREE(t);

                            // Read the max value next.
                            t = nullptr;
                            off = offset + r_size;
                            r_size += handler->read(buffer, &t, off, ULONG_MAX);
                            CHECK_NOT_NULL(t);
                            max_value = string(*t);
                            CHECK_AND_FREE(t);

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


                    template<typename __T, __type_def_enum __type, __constraint_operator __operator>
                    class __oper_constraint : public __constraint {
                    protected:
                        __constraint_operator oper = __operator;
                        __type_def_enum value_type = __type;
                        __T value;
                        __base_datatype_io *handler = nullptr;

                        string get_oper_string(__constraint_operator oper) const {
                            switch (oper) {
                                case __constraint_operator::EQ:
                                    return "=";
                                case __constraint_operator::GT:
                                    return ">";
                                case __constraint_operator::GTEQ:
                                    return ">=";
                                case __constraint_operator::LT:
                                    return "<";
                                case __constraint_operator::LTEQ:
                                    return "=<";
                            }
                            return common_consts::EMPTY_STRING;
                        }

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

                            uint64_t off = offset;
                            uint8_t type = __constraint_type_utils::get_number_value(get_constraint_type());
                            uint64_t w_size = buffer_utils::write<uint8_t>(buffer, &off, type);

                            off = (offset + w_size);
                            w_size += handler->write(buffer, &value, off,
                                                     ULONG_MAX); // Note: Don't need to worry about buffer size here.

                            return w_size;
                        }

                        virtual uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            uint64_t off = offset;
                            uint8_t *type = nullptr;
                            uint64_t r_size = buffer_utils::read<uint8_t>(buffer, &off, &type);
                            CHECK_NOT_NULL(type);
                            POSTCONDITION(*type == __constraint_type_utils::get_number_value(get_constraint_type()));

                            off = offset + r_size;
                            __T *t = nullptr;
                            r_size += handler->read(buffer, &t, off, ULONG_MAX);
                            CHECK_NOT_NULL(t);
                            value = *t;

                            return r_size;
                        }

                        void print() const override {
                            string n = (is_not ? "NOT" : "");
                            string t = __type_enum_helper::get_type_string(this->value_type);
                            string v = __type_enum_helper::get_string_value(&(this->value), this->value_type);
                            string o = get_oper_string(this->oper);
                            LOG_DEBUG("\tconstraint : %s compare(%s %s) datatype=%s", n.c_str(), o.c_str(), v.c_str(),
                                     t.c_str());
                        }
                    };

                    template<__constraint_operator __operator>
                    class string_oper_constraint
                            : public __oper_constraint<string, __type_def_enum::TYPE_STRING, __operator> {
                    public:
                        virtual uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(this->handler);

                            uint64_t off = offset;
                            uint8_t *type = nullptr;
                            uint64_t r_size = buffer_utils::read<uint8_t>(buffer, &off, &type);
                            CHECK_NOT_NULL(type);
                            POSTCONDITION(
                                    *type == __constraint_type_utils::get_number_value(this->get_constraint_type()));

                            off = offset + r_size;
                            string *t = nullptr;
                            r_size += this->handler->read(buffer, &t, off, ULONG_MAX);
                            CHECK_NOT_NULL(t);
                            this->value = string(*t);
                            CHECK_AND_FREE(t);

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

                    typedef string_oper_constraint<__constraint_operator::GT> string_gt_constraint;
                    typedef string_oper_constraint<__constraint_operator::GTEQ> string_gteq_constraint;
                    typedef string_oper_constraint<__constraint_operator::LT> string_lt_constraint;
                    typedef string_oper_constraint<__constraint_operator::LTEQ> string_lteq_constraint;

                    template<typename __T, __type_def_enum __type>
                    class __value_constraint : public __constraint {
                    protected:
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

                            uint64_t off = offset;
                            uint8_t type = __constraint_type_utils::get_number_value(__constraint_type::CONSTRAINT_IN);
                            uint64_t w_size = buffer_utils::write<uint8_t>(buffer, &off, type);

                            uint16_t count = values.size();
                            w_size += buffer_utils::write<uint16_t>(buffer, &off, count);
                            for (__T t : values) {
                                off = (offset + w_size);
                                w_size += handler->write(buffer, &t, off,
                                                         ULONG_MAX); // Note: Don't need to worry about buffer size here.
                            }
                            return w_size;
                        }

                        virtual uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            uint64_t off = offset;
                            uint8_t *type = nullptr;
                            uint64_t r_size = buffer_utils::read<uint8_t>(buffer, &off, &type);
                            CHECK_NOT_NULL(type);
                            POSTCONDITION(*type ==
                                          __constraint_type_utils::get_number_value(__constraint_type::CONSTRAINT_IN));
                            uint16_t *count = nullptr;
                            r_size += buffer_utils::read<uint16_t>(buffer, &off, &count);
                            CHECK_NOT_NULL(count);
                            if (*count > 0) {
                                for (uint16_t ii = 0; ii < *count; ii++) {
                                    off = offset + r_size;
                                    __T *t = nullptr;
                                    r_size += handler->read(buffer, &t, off, ULONG_MAX);
                                    CHECK_NOT_NULL(t);

                                    values.push_back(*t);
                                }
                            }
                            return r_size;
                        }

                        void print() const override {
                            string n = (is_not ? "NOT" : "");
                            string t = __type_enum_helper::get_type_string(this->value_type);
                            string v;
                            for (__T t : values) {
                                string tv = __type_enum_helper::get_string_value(&t, this->value_type);
                                if (!IS_EMPTY(v)) {
                                    v.append(", ");
                                }
                                v.append(tv);
                            }
                            LOG_DEBUG("\tconstraint : %s in(%s) datatype=%s", n.c_str(), v.c_str(),
                                     t.c_str());
                        }
                    };

                    class string_value_constraint : public __value_constraint<string, __type_def_enum::TYPE_STRING> {
                    public:
                        virtual uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            uint64_t off = offset;
                            uint8_t *type = nullptr;
                            uint64_t r_size = buffer_utils::read<uint8_t>(buffer, &off, &type);
                            CHECK_NOT_NULL(type);
                            POSTCONDITION(*type ==
                                          __constraint_type_utils::get_number_value(__constraint_type::CONSTRAINT_IN));
                            uint16_t *count = nullptr;
                            r_size += buffer_utils::read<uint16_t>(buffer, &off, &count);
                            CHECK_NOT_NULL(count);
                            if (*count > 0) {
                                for (uint16_t ii = 0; ii < *count; ii++) {
                                    off = offset + r_size;
                                    string *t = nullptr;
                                    r_size += handler->read(buffer, &t, off, ULONG_MAX);
                                    CHECK_NOT_NULL(t);

                                    values.push_back(string(*t));
                                    CHECK_AND_FREE(t);
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

                }
REACTFS_NS_CORE_END
#endif //REACTFS_CONSTRAINTS_H
