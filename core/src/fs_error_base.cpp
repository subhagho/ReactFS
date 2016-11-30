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
// Created by Subhabrata Ghosh on 28/11/16.
//

#include "core/includes/fs_error_base.h"

const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_INDEX_COPRRUPTED = 0;
const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_INDEX_NOT_FOUND = 1;
const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_INDEX_FILE_NOUT_FOUND = 2;
const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_BLOCK_COPRRUPTED = 3;
const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_BLOCK_FILE_NOT_FOUND = 4;
const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_RECORD_NOT_FOUND = 5;
const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_INDEX_DATA_VERSION = 6;
const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_BLOCK_DATA_VERSION = 7;
const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_BLOCK_COMPRESSION = 8;
const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_BLOCK_OUT_OF_SPACE = 9;
const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_ALLOCATING_BLOCK_SPACE = 10;
const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_MM_MAX_BLOCKS_USED = 11;
const uint16_t com::wookler::reactfs::core::fs_block_error::ERRCODE_BLOCK_SANITY_FAILED = 12;

const char *com::wookler::reactfs::core::fs_block_error::error_strings[] = {
        "Block index corrupted.",
        "Specified index not found",
        "Index file not found",
        "Data block corrupted",
        "Data block file not found",
        "Specified data block not found",
        "Index structures versions don't match.",
        "Data block structures versions don't match",
        "Data block compression handler not found.",
        "Not enough space left on local filesystem",
        "Error trying to allocate block space",
        "Maximum number of blocks per file structure exhausted.",
        "Sanity check for block failed."

};