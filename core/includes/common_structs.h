//
// Created by Subhabrata Ghosh on 28/10/16.
//

#ifndef REACTFS_COMMON_STRUCTS_H
#define REACTFS_COMMON_STRUCTS_H

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                typedef enum __block_record_type__ {
                    RAW = 0, TYPED = 1
                } __block_record_type;

                typedef enum __block_type__ {
                    PRIMARY = 0, REPLICA = 1, DELETED = 2, CORRUPTED = 3
                } __block_type;

                typedef enum __compression_type__ {
                    NONE = -1, GZIP = 0, LZO, ZLIB, SNAPPY
                } __compression_type;

                typedef struct {
                    bool compressed = false;
                    uint64_t compressed_size = 0;
                    __compression_type type = __compression_type::NONE;
                } __compression;

                typedef enum __archival__ {
                    DISABLED = 0, DELETE = 1, COMPRESSED = 2, ARCHIVE = 3
                } __archival;

                typedef struct {
                    uint64_t block_id;
                    char block_uid[128];
                    __block_type block_type = __block_type::PRIMARY;
                    uint64_t create_time;
                    uint64_t update_time;
                    uint64_t block_size;
                    uint64_t used_bytes;
                    uint64_t write_offset;
                    bool writable = true;
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
