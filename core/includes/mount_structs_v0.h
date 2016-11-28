//
// Created by Subhabrata Ghosh on 25/11/16.
//

#ifndef REACTFS_MOUNT_STRUCTS_V0_H
#define REACTFS_MOUNT_STRUCTS_V0_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/exclusive_lock.h"
#include "common/includes/metrics.h"

#define MAX_MOUNT_POINTS 32

using namespace com::wookler::reactfs::common;

#define __MP_READ_WRITE "READ_WRITE"
#define __MP_READ_ONLY "READ_ONLY"
#define __MP_UNAVAILABLE "UNAVAILABLE"
#define __MP_CORRUPTED "CORRUPTED"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                /*!
                 * State of the specified mount point.
                 */
                typedef enum __mount_state__ {
                    /// Mount-Point is available to read/write
                            MP_READ_WRITE = 0,
                    /// Mount-Point is only available to read
                            MP_READ_ONLY,
                    /// Mount-Point is not available.
                            MP_UNAVAILABLE,
                    /// Mount-point is corrupted
                            MP_CORRUPTED
                } __mount_state;


                /*!
                 * Structure for defining a Mount Point and recording usage statistics.
                 */
                typedef struct __mount_point__v0__ {
                    /// File-system path of this mount point.
                    char path[SIZE_MAX_PATH];
                    /// Name of the shared lock used to updating mount data
                    char lock_name[SIZE_LOCK_NAME];
                    /// State of this mount point
                    __mount_state state = __mount_state::MP_UNAVAILABLE;
                    /// Usage limit, by default the available disk space.
                    int64_t usage_limit = 0;
                    /// Total number of blocks created in this mount point.
                    uint32_t total_blocks = 0;
                    /// Total bytes used on this mount point by the blocks.
                    uint64_t total_bytes_used = 0;
                    /// Timestamp when the last recent data was recycled.
                    uint64_t last_recycled_time = 0;
                    /// Total number of bytes written to this mount point.
                    uint64_t total_bytes_written = 0;
                    /// Total number of bytes read from this mount point.
                    uint64_t total_bytes_read = 0;
                    /// Bytes written in the last 24Hrs
                    __hourly_usage_metric bytes_written;
                    /// Bytes read in the last 24Hrs
                    __hourly_usage_metric bytes_read;
                } __mount_point_v0;

                /*!
                 * Envelope structure to define the mount points.
                 */
                typedef struct __mount_data_v0__ {
                    /// Structure version of this record.
                    __version_header version;
                    /// Number of mount points defined.
                    uint16_t mount_count = 0;
                    /// Number of mount points available.
                    uint16_t available_count = 0;
                    /// Array of mounts points.
                    __mount_point_v0 mounts[MAX_MOUNT_POINTS];
                } __mount_data_v0;

            }
        }
    }
}
#endif //REACTFS_MOUNT_STRUCTS_V0_H
