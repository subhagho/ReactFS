//
// Created by Subhabrata Ghosh on 18/11/16.
//

#ifndef REACTFS_RESOURCE_CREATOR_H
#define REACTFS_RESOURCE_CREATOR_H

#include "common/includes/common.h"
#include "common/includes/config.h"
#include "resource_def.h"


namespace com {
    namespace wookler {
        namespace watergate {
            namespace core {
                class resource_creator {
                public:
                    virtual ~resource_creator() {

                    }

                    virtual resource_def *create(string name, const ConfigValue *node) = 0;
                };
            }
        }
    }
}
#endif //REACTFS_RESOURCE_CREATOR_H
