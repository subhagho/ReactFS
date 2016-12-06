//
// Created by Subhabrata Ghosh on 06/12/16.
//

#ifndef REACTFS_TYPE_ERRORS_H
#define REACTFS_TYPE_ERRORS_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/time_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"

using namespace REACTFS_NS_COMMON_PREFIX;

#define CONST_TYPE_VALID_ERROR_PREFIX "Data validation error : "
#define CONST_TYPE_PARSER_ERROR_PREFIX "Type Parser error : "
REACTFS_NS_CORE
                namespace types {
                    class type_validation_error : public base_error {
                    public:
                        type_validation_error(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                                          CONST_TYPE_VALID_ERROR_PREFIX,
                                                                                                          mesg) {
                        }

                        type_validation_error(char const *file, const int line, const exception &e) : base_error(file,
                                                                                                                 line,
                                                                                                                 CONST_TYPE_VALID_ERROR_PREFIX,
                                                                                                                 e.what()) {
                        }
                    };

                    class type_parser_error : public base_error {
                    public:
                        type_parser_error(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                                      CONST_TYPE_PARSER_ERROR_PREFIX,
                                                                                                      mesg) {
                        }

                        type_parser_error(char const *file, const int line, const exception &e) : base_error(file,
                                                                                                             line,
                                                                                                             CONST_TYPE_PARSER_ERROR_PREFIX,
                                                                                                             e.what()) {
                        }
                    };
                }
REACTFS_NS_CORE_END

#define TYPE_VALID_ERROR(fmt, ...) type_validation_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define TYPE_VALID_ERROR_PTR(fmt, ...) new type_validation_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))

#define TYPE_PARSER_ERROR(fmt, ...) type_parser_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define TYPE_PARSER_ERROR_PTR(fmt, ...) new type_parser_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))

#endif //REACTFS_TYPE_ERRORS_H
