//
// Created by Subhabrata Ghosh on 23/12/16.
//

#ifndef REACTFS_BASE_TYPE_H
#define REACTFS_BASE_TYPE_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace types {
                    class __base_type {
                    protected:
                        bool __is_allocated = false;
                    public:
                        virtual ~__base_type() {

                        }
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_BASE_TYPE_H
