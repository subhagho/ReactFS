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

using namespace REACTFS_NS_COMMON_PREFIX;
typedef struct __constraint_def__ {
    bool is_not = false;
    string *constraint = nullptr;
    string *type = nullptr;
    string *values = nullptr;
} __constraint_def;

typedef struct __declare__ {
    bool is_reference = false;
    string *type;
    string *variable;
    __declare__ *next = nullptr;
    uint32_t size = 0;
    __declare__ *inner_types = nullptr;
    __constraint_def *constraint = nullptr;
    string *default_value = nullptr;
} __declare;

typedef struct __reference_type__ {
    string name;
    int declare_count = 0;
    __declare *members = nullptr;
} __reference_type;

typedef struct __key_column__ {
    string name;
    int index;
    bool sort_asc = true;
    __key_column__ *next = nullptr;
} __key_column;

typedef struct __index_def__ {
    string name;
    int column_count = 0;
    __key_column *columns;
    __index_def__ *next = nullptr;
} __index_def;

typedef struct __schema_def__ {
    string name;
    int column_count = 0;
    __declare *members = nullptr;
    __key_column *pk_columns = nullptr;
    __index_def *indexes = nullptr;
} __schema_def;
#endif //REACTFS_SCHEMA_H
