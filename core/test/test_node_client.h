//
// Created by Subhabrata Ghosh on 09/11/16.
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

#ifndef REACTFS_TEST_NODE_CLIENT_H
#define REACTFS_TEST_NODE_CLIENT_H
#define CATCH_CONFIG_MAIN

#include <iostream>
#include "test_common.h"
#include "common/includes/init_utils.h"
#include "common/includes/__threads.h"
#include "core/includes/block_utils.h"
#include "core/includes/server/node_init_manager.h"
#include "core/includes/clients/node_client_env.h"

using namespace com::wookler::reactfs::core;
using namespace com::wookler::reactfs::core::server;
using namespace com::wookler::reactfs::core::client;

#define CONFIG_FILE getenv("CONFIG_FILE_PATH")


#endif //REACTFS_TEST_NODE_CLIENT_H
