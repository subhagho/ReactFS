//
// Created by Subhabrata Ghosh on 21/12/16.
//


/*
 * Copyright [yyyy] [name of copyright owner]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef REACTFS_TYPED_HASH_INDEX_H
#define REACTFS_TYPED_HASH_INDEX_H

#include "common/includes/common.h"
#include "common/includes/__state__.h"
#include "common/includes/log_utils.h"
#include "common/includes/common_utils.h"
#include "common/includes/file_utils.h"
#include "common/includes/timer.h"
#include "common/includes/__alarm.h"
#include "common/includes/read_write_lock.h"
#include "common/includes/mapped_data.h"
#include "common/includes/buffer_utils.h"
#include "common/includes/ext/murmur3.h"

#include "core.h"
#include "common_structs.h"
#include "fs_error_base.h"
#include "typed_index_base.h"

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;

#define DEFAULT_BLOAT_FACTOR (15 / 10)
#define HASH_INDEX_VERSION_MAJOR ((uint16_t) 0)
#define HASH_INDEX_VERSION_MINOR ((uint16_t) 1)

REACTFS_NS_CORE
                class typed_hash_index : public typed_index_base {

                };
REACTFS_NS_CORE_END

#endif //REACTFS_TYPED_HASH_INDEX_H
