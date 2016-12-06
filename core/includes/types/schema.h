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

#include "core/includes/types/type_defs.h"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
    namespace types {
        class __schema {
        private:
            string name;
            __complex_type *types;
        };
    }
REACTFS_NS_CORE_END
#endif //REACTFS_SCHEMA_H
