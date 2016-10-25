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
