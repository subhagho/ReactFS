//
// Created by Subhabrata Ghosh on 06/12/16.
//

#ifndef REACTFS_SCHEMA_H
#define REACTFS_SCHEMA_H

#include <unordered_map>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/time_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"
#include "types/includes/type_core.h"

using namespace REACTFS_NS_COMMON_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX::types;

REACTFS_NS_CORE
                namespace parsers {
                    typedef struct __constraint_def__ {
                        bool is_not = false;
                        string *constraint = nullptr;
                        string *type = nullptr;
                        string *values = nullptr;
                    } __constraint_def;

                    typedef struct __declare__ {
                        bool is_reference = false;
                        bool is_nullable = false;
                        string *type;
                        string *variable;
                        __declare__ *next = nullptr;
                        uint32_t size = 0;
                        __declare__ *inner_types = nullptr;
                        __constraint_def *constraint = nullptr;
                        string *default_value = nullptr;
                    } __declare;

                    typedef struct __reference_type__ {
                        string *name;
                        int field_count = 0;
                        __declare *members = nullptr;
                    } __reference_type;

                    typedef struct __key_column__ {
                        string *name;
                        int index;
                        bool sort_asc = true;
                        __key_column__ *next = nullptr;
                    } __key_column;

                    typedef struct __index_def__ {
                        string *name;
                        string *schema_name;
                        __index_type_enum type = __index_type_enum::TREE_INDEX;
                        int field_count = 0;
                        __key_column *columns = nullptr;
                    } __index_def;

                    typedef struct __schema_def__ {
                        string *name;
                        int field_count = 0;
                        __declare *members = nullptr;
                        __key_column *pk_columns = nullptr;
                    } __schema_def;

                    class index_type_utils {
                    public:
                        static string get_index_name(__index_type_enum te) {
                            switch (te) {
                                case __index_type_enum::FULLTEXT_INDEX:
                                    return "FULLTEXT";
                                case __index_type_enum::HASH_INDEX:
                                    return "HASH";
                                case __index_type_enum::TREE_INDEX:
                                    return "TREE";
                            }
                            return common_consts::EMPTY_STRING;
                        }

                        static __index_type_enum get_index_type(const string &type) {
                            CHECK_NOT_EMPTY(type);
                            if (type == get_index_name(__index_type_enum::TREE_INDEX)) {
                                return __index_type_enum::TREE_INDEX;
                            } else if (type == get_index_name(__index_type_enum::HASH_INDEX)) {
                                return __index_type_enum::HASH_INDEX;
                            } else if (type == get_index_name(__index_type_enum::FULLTEXT_INDEX)) {
                                return __index_type_enum::FULLTEXT_INDEX;
                            }
                            throw TYPE_PARSER_ERROR("Error parsing index type. [type=%s]", type.c_str());
                        }
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_SCHEMA_H
