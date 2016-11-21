//
// Created by Subhabrata Ghosh on 20/11/16.
//

#include "core/includes/mm_block_meta.h"


const string com::wookler::reactfs::core::structs::mm_hash_constants::INFO_FILE = "mm_hash_table.info";
/// Small hash-table block - 64MB
const uint64_t com::wookler::reactfs::core::structs::mm_hash_constants::SMALL = 1024 * 1024 * 64;
/// Medium hash-table block - 128MB
const uint64_t com::wookler::reactfs::core::structs::mm_hash_constants::MEDIUM = 1024 * 1024 * 128;
/// Large hash-table block - 512MB
const uint64_t com::wookler::reactfs::core::structs::mm_hash_constants::LARGE = 1024 * 1024 * 512;
/// X-Large hash-table block - 1GB
const uint64_t com::wookler::reactfs::core::structs::mm_hash_constants::XLARGE = 1024 * 1024 * 1024;