//
// Created by Subhabrata Ghosh on 06/12/16.
//

#ifndef REACTFS_CONSTRAINTS_H
#define REACTFS_CONSTRAINTS_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/time_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"

#include "core/includes/types/type_defs.h"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace types {

                    class __notnull : public __constraint {
                    public:
                        virtual bool validate(void *value) const {
                            return NOT_NULL(value);
                        }
                    };

                    template<typename __T>
                    class __value_constraint : public __constraint {
                    private:
                        vector<__T> values;

                    public:
                        void add_value(__T t) {
                            values.push_back(t);
                        }

                        virtual bool validate(void *value) const {
                            if (NOT_NULL(value)) {
                                __T *t = (__T *) value;
                                vector<__T>::iterator iter;
                                for (iter = values.begin(); iter != values.end(); iter++) {
                                    if (*t == (*iter)) {
                                        return true;
                                    }
                                }
                            }
                            return false;
                        }
                    };

                    typedef __value_constraint<char> char_value_constraint;
                    typedef __value_constraint<short> short_value_constraint;
                    typedef __value_constraint<int> int_value_constraint;
                    typedef __value_constraint<long> long_value_constraint;
                    typedef __value_constraint<float> float_value_constraint;
                    typedef __value_constraint<uint64_t> timestamp_value_constraint;
                    typedef __value_constraint<string> string_value_constraint;

                    template<typename __T>
                    class __range_constraint : public __constraint {
                    private:
                        __T min_value;
                        __T max_value;
                    public:
                        __range_constraint(__T min_value, __T max_value) {
                            this->max_value = max_value;
                            this->min_value = min_value;
                        }

                        virtual bool validate(void *value) const {
                            if (NOT_NULL(value)) {
                                __T *t = (__T *) value;
                                return (*t >= min_value && *t <= max_value);
                            }
                            return false;
                        }
                    };

                    typedef __range_constraint<char> char_range_constraint;
                    typedef __range_constraint<short> short_range_constraint;
                    typedef __range_constraint<int> int_range_constraint;
                    typedef __range_constraint<long> long_range_constraint;
                    typedef __range_constraint<float> float_range_constraint;
                    typedef __range_constraint<uint64_t> timestamp_range_constraint;
                    typedef __range_constraint<string> string_range_constraint;

                }
REACTFS_NS_CORE_END
#endif //REACTFS_CONSTRAINTS_H
