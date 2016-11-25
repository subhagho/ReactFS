//
// Created by Subhabrata Ghosh on 25/11/16.
//

#ifndef REACTFS_MOUNT_STRUCTS_V0_H
#define REACTFS_MOUNT_STRUCTS_V0_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/exclusive_lock.h"

#define MAX_MOUNT_POINTS 32

using namespace com::wookler::reactfs::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                typedef enum __mount_state__ {
                    MP_READ_WRITE = 0, MP_READ_ONLY, MP_UNAVAILABLE, MP_CORRUPTED
                } __mount_state;

                typedef struct __mount_point__v0__ {
                    char path[SIZE_MAX_PATH];
                    char lock_name[SIZE_LOCK_NAME];
                    __mount_state state = __mount_state::MP_UNAVAILABLE;
                    int64_t usage_limit = 0;
                    uint32_t total_blocks = 0;
                    uint64_t total_bytes_written = 0;
                    uint64_t total_bytes_read = 0;
                    uint64_t bytes_written = 0;
                    uint64_t bytes_read = 0;
                    uint64_t time_write = 0;
                    uint64_t time_read = 0;
                } __mount_point_v0;

                typedef struct __mount_data_v0__ {
                    __version_header version;
                    uint16_t mount_count = 0;
                    uint16_t available_count = 0;
                    __mount_point_v0 mounts[MAX_MOUNT_POINTS];
                } __mount_data_v0;

            }
        }
    }
}
#endif //REACTFS_MOUNT_STRUCTS_V0_H
