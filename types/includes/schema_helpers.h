
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

#ifndef REACTFS_SCHEMA_HELPERS_H
#define REACTFS_SCHEMA_HELPERS_H

#include "core/includes/core.h"

#include "schema_common.h"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace types {
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
                            CHECK_NOT_NULL(value);
                            __default *d = nullptr;
                            switch (datatype) {
                                case __type_def_enum::TYPE_BOOL: {
                                    __default_bool *dd = new __default_bool();
                                    CHECK_ALLOC(dd, TYPE_NAME(__default_bool));
                                    bool bv = common_utils::parse_bool(*value);
                                    dd->set_value(bv);

                                    d = dd;
                                }
                                    break;
                                case __type_def_enum::TYPE_CHAR: {
                                    __default_char *dd = new __default_char();
                                    CHECK_ALLOC(dd, TYPE_NAME(__default_char));
                                    char c = (*value)[0];
                                    dd->set_value(c);

                                    d = dd;
                                }
                                    break;
                                case __type_def_enum::TYPE_SHORT: {
                                    __default_short *dd = new __default_short();
                                    CHECK_ALLOC(dd, TYPE_NAME(__default_short));
                                    int is = std::stoi(*value);
                                    POSTCONDITION(is >= std::numeric_limits<short>::min() &&
                                                  is <= std::numeric_limits<short>::max());
                                    short s = (short) is;
                                    dd->set_value(s);

                                    d = dd;
                                }
                                    break;
                                case __type_def_enum::TYPE_INTEGER: {
                                    __default_int *dd = new __default_int();
                                    CHECK_ALLOC(dd, TYPE_NAME(__default_int));
                                    int ii = std::stoi(*value);
                                    dd->set_value(ii);

                                    d = dd;
                                }
                                    break;
                                case __type_def_enum::TYPE_LONG: {
                                    __default_long *dd = new __default_long();
                                    CHECK_ALLOC(dd, TYPE_NAME(__default_long));
                                    long ll = std::stol(*value);
                                    dd->set_value(ll);

                                    d = dd;
                                }
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                case __type_def_enum::TYPE_TIMESTAMP: {
                                    __default_timestamp *dd = new __default_timestamp();
                                    CHECK_ALLOC(dd, TYPE_NAME(__default_timestamp));
                                    long lu = std::stol(*value);
                                    uint64_t ul = (uint64_t) lu;
                                    dd->set_value(ul);

                                    d = dd;
                                }
                                    break;
                                case __type_def_enum::TYPE_FLOAT: {
                                    __default_float *dd = new __default_float();
                                    CHECK_ALLOC(dd, TYPE_NAME(__default_float));
                                    float fv = std::stof(*value);
                                    dd->set_value(fv);

                                    d = dd;
                                }
                                    break;
                                case __type_def_enum::TYPE_DOUBLE: {
                                    __default_double *dd = new __default_double();
                                    CHECK_ALLOC(dd, TYPE_NAME(__default_double));
                                    double dv = std::stod(*value);
                                    dd->set_value(dv);

                                    d = dd;
                                }
                                    break;
                                case __type_def_enum::TYPE_STRING: {
                                    __default_string *dd = new __default_string();
                                    CHECK_ALLOC(dd, TYPE_NAME(__default_string));
                                    dd->set_value(*value);

                                    d = dd;
                                }
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
#endif //REACTFS_SCHEMA_HELPERS_H
