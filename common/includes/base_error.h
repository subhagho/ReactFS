/* 
 * Copyright [2016] [Subhabrata Ghosh] 
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 *       limitations under the License. 
 * 
 */
//
// Created by Subhabrata Ghosh on 08/09/16.
//

#ifndef WATERGATE_BASE_ERROR_H
#define WATERGATE_BASE_ERROR_H

#include "common_utils.h"

#define CONST_NOTF_ERROR_PREFIX "Data Not Found : "
#define CONST_ALLOC_ERROR_PREFIX "Error allocating memory : "
#define CONST_CAST_ERROR_PREFIX "Error casting types : "

REACTFS_NS_COMMON
                /*!
                 * Base exception class to be used, added functionality of capturing source information.
                 */
                class base_error : public exception {
                private:
                    /// Error message associated with this exception.
                    string mesg;
                    /// Source filename where the exception was defined.
                    string file;
                    /// Source line no. where the exception was defined.
                    int lineno;
                    /// Processed message with the added source infromation.
                    string r_mesg;

                    /*!
                     * Construct the exception message.
                     */
                    void set_r_mesg() {
                        stringstream ss;
                        if (!IS_EMPTY(file)) {
                            ss << file << "\t";
                        }
                        if (lineno >= 0) {
                            ss << lineno << "\t";
                        }
                        if (!IS_EMPTY(mesg)) {
                            ss << mesg;
                        }

                        r_mesg = string(ss.str());
                    }

                protected:
                    base_error(char const *file, const int line, const string prefix, string mesg) {
                        this->file = string(file);
                        this->lineno = line;
                        this->mesg = string(common_utils::format("%s %s", prefix.c_str(), mesg.c_str()));

                        set_r_mesg();
                    }

                public:
                    base_error(char const *file, const int line, string mesg) {
                        this->file = file;
                        this->lineno = line;
                        this->mesg = string(mesg);

                        set_r_mesg();
                    }

                    const string get_error() {
                        return mesg;
                    }

                    const char *what() const throw() override {
                        return r_mesg.c_str();
                    }

                };

                class not_found_error : public base_error {
                public:
                    not_found_error(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                                CONST_NOTF_ERROR_PREFIX,
                                                                                                mesg) {
                    }

                    not_found_error(char const *file, const int line, string mesg, string key) : base_error(file, line,
                                                                                                            CONST_NOTF_ERROR_PREFIX,
                                                                                                            common_utils::format(
                                                                                                                    "[key=%s][error=%s]",
                                                                                                                    key.c_str(),
                                                                                                                    mesg.c_str())) {
                    }
                };

                class alloc_error : public base_error {
                public:
                    alloc_error(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                            CONST_ALLOC_ERROR_PREFIX,
                                                                                            mesg) {

                    }
                };

                class cast_error : public base_error {
                public:
                    cast_error(char const *file, const int line, string source, string target) : base_error(file, line,
                                                                                                            CONST_CAST_ERROR_PREFIX,
                                                                                                            common_utils::format(
                                                                                                                    "Cast failed. [source type=%s][target type=%s]",
                                                                                                                    source.c_str(),
                                                                                                                    target.c_str())) {

                    }
                };
REACTFS_NS_COMMON_END

/// Convinience macros to create exception(s)
#define BASE_ERROR(fmt, ...) base_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define BASE_ERROR_PTR(fmt, ...) new base_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))

#define NOT_FOUND_ERROR(fmt, ...) not_found_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define NOT_FOUND_ERROR_PTR(fmt, ...) new not_found_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))

#define ALLOC_ERROR(fmt, ...) alloc_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define ALLOC_ERROR_PTR(fmt, ...) new alloc_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))

#define CAST_ERROR(s, t) cast_error(__FILE__, __LINE__, s, t)
#define CAST_ERROR_PTR(s, t) new cast_error(__FILE__, __LINE__, s, t)

#ifdef NDEBUG
#define    _assert(e)    ((void)0)
#else
#define _assert(e) do {\
    if (!(e)) { \
        LOG_ERROR("Assertion failed. [%s][%s]", #e, __PRETTY_FUNCTION__); \
        throw BASE_ERROR("Assertion failed. [%s][%s]", #e, __PRETTY_FUNCTION__); \
    } \
} while(0);
#endif

#define ASSERT _assert

/// Check to ensure that the specified pointer is not null. Will raise an exception if NULL.
#define CHECK_NOT_NULL(p) do { \
    if (IS_NULL(p)) { \
        throw BASE_ERROR("Specified pointer is null. [%s][%s]", #p, __PRETTY_FUNCTION__); \
    } \
} while(0);

#define CHECK_ALLOC(p, t) do {\
    if (IS_NULL(p)) { \
        throw ALLOC_ERROR("Specified pointer is null. [%s][%s][type=%s]", #p, __PRETTY_FUNCTION__, t); \
    } \
} while(0);

#define CHECK_CAST(p, s, t) do {\
    if (IS_NULL(p)) { \
        throw CAST_ERROR(s, t); \
    } \
} while(0);

#define CHECK_NOT_EMPTY(p) do { \
    if (IS_EMPTY(p)) { \
        throw BASE_ERROR("Specified instance is empty. [%s][%s]", #p, __PRETTY_FUNCTION__); \
    } \
} while(0);

#define CHECK_NOT_EMPTY_P(p) do { \
    if (IS_NULL(p) || IS_EMPTY_P(p)) { \
        throw BASE_ERROR("Specified instance is null or empty. [%s][%s]", #p, __PRETTY_FUNCTION__); \
    } \
} while(0);

#define PRECONDITION(p) do { \
    if (!(p)) { \
        throw BASE_ERROR("Pre-condition : Check condition failed. [%s][%s]", #p, __PRETTY_FUNCTION__); \
    } \
} while(0);

#define POSTCONDITION(p) do { \
    if (!(p)) { \
        throw BASE_ERROR("Post-condition : Check condition failed. [%s][%s]", #p, __PRETTY_FUNCTION__); \
    } \
} while(0);

#endif //WATERGATE_BASE_ERROR_H
