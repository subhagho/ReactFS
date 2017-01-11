//
// Created by Subhabrata Ghosh on 11/01/17.
//

#ifndef REACTFS_TYPE_CORE_H
#define REACTFS_TYPE_CORE_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/time_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
    namespace types {

        typedef enum __index_type_enum__ {
            HASH_INDEX, TREE_INDEX, FULLTEXT_INDEX
        } __index_type_enum;

    }
REACTFS_NS_CORE_END
#endif //REACTFS_TYPE_CORE_H
