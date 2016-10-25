//
// Created by Subhabrata Ghosh on 25/10/16.
//

#ifndef REACTFS_FS_BLOCK_H
#define REACTFS_FS_BLOCK_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                class fs_block {
                private:
                    uint64_t block_id;
                    string block_uid;

                };
            }
        }
    }
}


#endif //REACTFS_FS_BLOCK_H
