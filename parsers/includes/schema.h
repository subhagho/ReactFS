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
} __declare;

typedef struct __reference_type__ {
    string name;
    __declare *members = nullptr;
} __reference_type;

#endif //REACTFS_SCHEMA_H
