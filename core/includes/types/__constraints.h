
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

                    class __constraint_type_utils {
                    public:
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
                                    return "greater than";
                                case __constraint_type::CONSTRAINT_LT:
                                    return "less than";
                                case __constraint_type::CONSTRAINT_GTEQ:
                                    return "greater than equals";
                                case __constraint_type::CONSTRAINT_LTEQ:
                                    return "less than equals";
                            }
                        }
                    };

                    class __notnull : public __constraint {
                    public:
                        virtual bool validate(const void *value) const override {
                            return NOT_NULL(value);
                        }

                        virtual uint32_t write(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);

                            uint8_t type = __constraint_type_utils::get_number_value(
                                    __constraint_type::CONSTRAINT_NOT_NULLABLE);
                            memcpy(ptr, &type, sizeof(uint8_t));

                            return sizeof(uint8_t);
                        }

                        virtual uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t *type = static_cast<uint8_t *>(ptr);
                            POSTCONDITION(*type ==
                                          __constraint_type_utils::get_number_value(
                                                  __constraint_type::CONSTRAINT_NOT_NULLABLE));

                            return sizeof(uint8_t);
                        }

                        __type_def_enum get_datatype() override {
                            return __type_def_enum::TYPE_UNKNOWN;
                        }
                    };

                    class __regex : public __constraint {
                    private:
                        string value;
                        std::regex regex_;
                        __base_datatype_io *handler = nullptr;

                    public:
                        __regex() {
                            handler = __type_defs_utils::get_type_handler(__type_def_enum::TYPE_STRING);
                            CHECK_NOT_NULL(handler);
                        }

                        void set_value(string &value) {
                            this->value = string(value);
                            this->regex_ = regex(value);
                        }

                        virtual bool validate(const void *value) const override {
                            if (NOT_NULL(value)) {
                                const string *ss = static_cast<const string *>(value);
                                CHECK_NOT_NULL(ss);
                                // return std::regex_match(ss, this->regex_);
                            }
                            return false;
                        }

                        virtual uint32_t write(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);

                            uint8_t type = __constraint_type_utils::get_number_value(
                                    __constraint_type::CONSTRAINT_REGEX);
                            memcpy(ptr, &type, sizeof(uint8_t));
                            uint16_t w_size = sizeof(uint8_t);
                            w_size += handler->write(buffer, &value, (offset + w_size), ULONG_MAX);

                            return w_size;
                        }

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
                            set_value(*value);

                            return r_size;
                        }

                        __type_def_enum get_datatype() override {
                            return __type_def_enum::TYPE_STRING;
                        }
                    };

                    template<typename __T, __type_def_enum __type>
                    class __range_constraint : public __constraint {
                    private:
                        __type_def_enum value_type = __type;
                        __T min_value;
                        __T max_value;
                        __base_datatype_io *handler = nullptr;

                    public:
                        __range_constraint() {
                            PRECONDITION(__type_enum_helper::is_native(value_type));
                            handler = __type_defs_utils::get_type_handler(value_type);
                            CHECK_NOT_NULL(handler);
                        }

                        virtual ~__range_constraint() {

                        }

                        void set_min_value(__T min_value) {
                            this->min_value = min_value;
                        }

                        void set_max_value(__T max_value) {
                            this->max_value = max_value;
                        }

                        __type_def_enum get_datatype() override {
                            return this->value_type;
                        }

                        virtual bool validate(const void *value) const override {
                            if (NOT_NULL(value)) {
                                __T *t = (__T *) value;
                                CHECK_NOT_NULL(t);
                                return (*t >= min_value && *t <= max_value);
                            }
                            return false;
                        }

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

                        virtual bool validate(const void *value) const override {
                            if (NOT_NULL(value)) {
                                CHECK_NOT_NULL(handler);

                                __T *t = (__T *) value;
                                CHECK_NOT_NULL(t);
                                return (handler->compare(&(this->value), value, oper));
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

                        virtual bool validate(const void *value) const override {
                            if (NOT_NULL(value)) {
                                __T *t = (__T *) value;
                                CHECK_NOT_NULL(t);
                                for (auto iter = values.begin(); iter != values.end(); iter++) {
                                    if (*t == (*iter)) {
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
                    typedef __value_constraint<double, __type_def_enum::TYPE_DOUBLE> douable_value_constraint;
                    typedef __value_constraint<string, __type_def_enum::TYPE_STRING> string_value_constraint;


                    class __constraint_loader {
                    public:
                        static __constraint *read(void *buffer, uint64_t offset, __type_def_enum datatype) {
                            return nullptr;
                        }
                    };

                }
REACTFS_NS_CORE_END
#endif //REACTFS_CONSTRAINTS_H
