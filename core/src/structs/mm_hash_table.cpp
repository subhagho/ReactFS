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

//
// Created by Subhabrata Ghosh on 19/11/16.
//

#include "core/includes/structs/mm_hash_table.h"

const string com::wookler::reactfs::core::structs::mm_hash_constants::INFO_FILE = "mm_hash_table.info";
/// Small hash-table block - 64MB
const uint64_t com::wookler::reactfs::core::structs::mm_hash_constants::SMALL = 1024 * 1024 * 64;
/// Medium hash-table block - 128MB
const uint64_t com::wookler::reactfs::core::structs::mm_hash_constants::MEDIUM = 1024 * 1024 * 128;
/// Large hash-table block - 512MB
const uint64_t com::wookler::reactfs::core::structs::mm_hash_constants::LARGE = 1024 * 1024 * 512;
/// X-Large hash-table block - 1GB
const uint64_t com::wookler::reactfs::core::structs::mm_hash_constants::XLARGE = 1024 * 1024 * 1024;
