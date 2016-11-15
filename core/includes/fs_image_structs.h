//
// Created by Subhabrata Ghosh on 04/11/16.
//

#ifndef REACTFS_FS_IMAGE_STRUCTS_H
#define REACTFS_FS_IMAGE_STRUCTS_H

#include "common/includes/common.h"

#define MAX_USERNAME_SIZE 64

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                typedef struct _varchar_ {
                    uint32_t size;
                    uint8_t *data;
                } _varchar;

                typedef enum _fs_file_node_type_ {
                    FILE = 0, DIR = 1
                } _fs_file_node_type;

                typedef struct _fs_file_node_ {
                    char file_uuid[SIZE_UUID];
                    char parent_uuid[SIZE_UUID];
                    _varchar name;
                    _varchar path;
                    _fs_file_node_type type;
                    uint64_t size_local = 0;
                    uint64_t create_time = 0;
                    uint64_t update_time = 0;
                    char owner[MAX_USERNAME_SIZE];
                    char group[MAX_USERNAME_SIZE];
                    mode_t mode;
                } _fs_file_node;
            }
        }
    }
}
#endif //REACTFS_FS_IMAGE_STRUCTS_H
