//
// Created by Subhabrata Ghosh on 28/10/16.
//

#ifndef REACTFS_COMMON_STRUCTS_H
#define REACTFS_COMMON_STRUCTS_H

#include "common/includes/common.h"

typedef uint8_t BYTE;
typedef uint8_t *BYTE_PTR;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                typedef enum __block_record_type__ {
                    RAW = 0, // Raw data block.
                    TYPED = 1 // Blocked contains typed records.
                } __block_record_type;

                typedef enum __block_type__ {
                    PRIMARY = 0, // Block is currently assigned to be the primary block.
                    REPLICA = 1, // Block is currently assigned as a replica block.
                    DELETED = 2 // Block has been marked as deleted.
                } __block_type;

                typedef enum __compression_type__ {
                    NONE = -1, // Data is not compressed.
                    GZIP = 0, // Data compressed using GZIP
                    LZO, // Data compressed using LZO
                    ZLIB, // Data compressed using GZIP
                    SNAPPY // Data compressed using Snappy
                } __compression_type;

                typedef struct {
                    bool compressed = false;
                    uint64_t uncompressed_size = 0; // Exploded size of this data block.
                    __compression_type type = __compression_type::NONE;
                } __compression;

                typedef enum __archival__ {
                    DISABLED = 0, // Data block is not archived.
                    DELETE = 1, // Delete the data block instead of archiving
                    COMPRESSED = 2, // Data block compressed.
                    ARCHIVE = 3 // Data block is archived to an archive folder.
                } __archival;

                typedef enum __block_state__ {
                    AVAILABLE = 0, // Data block is available for read/write
                    LOCKED = 1, // Data block is locked for writes
                    OUT_OF_SYNC = 2, // Data block is out-of-sync, needs to be synced
                    COMMIT_PENDING = 3, // Data block has commit pending.
                    CORRUPTED = 4 // Block is corrupted and needs to be re-synced.
                } __block_state;

                typedef enum __write_state__ {
                    WRITABLE = 0, CLOSED = 1
                } __write_state;

                typedef struct __rollback_info__ {
                    uint64_t write_offset = 0; // Write-offset of the last committed write.
                    uint64_t block_size = 0; // Block data size at the time of the last commit.
                } __rollback_info;

                typedef struct {
                    uint64_t block_id;
                    char block_uid[SIZE_UUID];
                    __block_type block_type = __block_type::PRIMARY;
                    __block_state state = __block_state::AVAILABLE;
                    uint64_t create_time;
                    uint64_t update_time;
                    uint64_t block_size;
                    uint64_t used_bytes;
                    uint64_t write_offset;
                    __rollback_info rollback_info;
                    __write_state write_state = __write_state::WRITABLE;
                    __block_record_type record_type = __block_record_type::RAW;
                    __compression compression;
                    uint64_t block_ttl = 0;
                    __archival archival = __archival::DISABLED;
                } __block_header;
            }
        }
    }
}
#endif //REACTFS_COMMON_STRUCTS_H
