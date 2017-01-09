//
// Created by Subhabrata Ghosh on 25/11/16.
//

#ifndef REACTFS_RW_LOCK_STRUCTS_V0_H
#define REACTFS_RW_LOCK_STRUCTS_V0_H

#include "common/includes/common.h"

#define DEFAULT_RW_LOCK_RETRY 10
#define SIZE_LOCK_NAME 32
#define MAX_READER_LOCKS 256
#define MAX_RW_SHARED_LOCKS 4096
#define MAX_W_SHARED_LOCKS 4096 * 10

REACTFS_NS_COMMON
                typedef struct __owner_v0__ {
                    char owner[SIZE_USER_NAME + 1];
                    char txn_id[SIZE_UUID + 1];
                    pid_t process_id;
                    char thread_id[SIZE_THREAD_ID + 1];
                    uint64_t lock_timestamp = 0;
                    uint64_t last_updated = 0;
                } __owner_v0;

                typedef struct __lock_readers_v0__ {
                    bool used = false;
                    pid_t process_id;
                    char thread_id[SIZE_THREAD_ID + 1];
                    uint64_t lock_timestamp = 0;
                } __lock_readers_v0;


                /*!
                 * Enum defines ths current state of this instance of the lock.
                 */
                typedef enum __lock_state_enum__ {
                    /// Lock state is unknown
                            LOCK_UNKNOWN = 0,
                    /// Lock is free and can be acquired.
                            LOCK_FREE = 1,
                    /// Lock has already been aqcuired.
                            LOCK_LOCKED = 2,
                    /// Lock has expired or lease has run out.
                            LOCK_EXPIRED = 3,
                    /// Lock has been reset due to inactivity.
                            LOCK_RESET = 4,
                    /// Lock is in an error state.
                            LOCK_ERROR = 5
                } __lock_state_enum;

                typedef struct __w_lock_struct_v0__ {
                    bool used = false;
                    char name[SIZE_LOCK_NAME + 1];
                    uint64_t last_used = 0;
                    bool write_locked = false;
                    __lock_state_enum state = __lock_state_enum::LOCK_UNKNOWN;
                    uint32_t ref_count = 0;
                    __owner_v0 owner;
                } __w_lock_struct_v0;

                typedef __w_lock_struct_v0 __w_lock_struct;

                typedef struct __rw_lock_struct_v0__ {
                    __w_lock_struct w_lock_struct;
                    uint64_t reader_count;
                    __lock_readers_v0 readers[MAX_READER_LOCKS];
                } __rw_lock_struct_v0;

                typedef struct __shared_lock_data_v0__ {
                    __version_header version;
                    uint32_t max_count = 0;
                    uint32_t used_count = 0;
                } __shared_lock_data_v0;
REACTFS_NS_COMMON_END
#endif //REACTFS_RW_LOCK_STRUCTS_V0_H
