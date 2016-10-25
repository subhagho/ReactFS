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
// Created by Subhabrata Ghosh on 14/10/16.
//

#include "watergate/includes/fs_writer.h"

#define CHECK_WRITTEN(n, m) do {\
    if (n < m) { \
        throw BASE_ERROR("Error writing data to file. [file=%s][error=%s]", path->get_path().c_str(), strerror(errno)); \
    } \
} while(0)

#define _LOCK_(n, p, s, t, e, r) do {\
    if (t > 0) { \
        r = client->lock(n, p, s, t, &e); \
    } else { \
        r = client->lock(n, p, e, &e); \
    } \
} while(0);

size_t com::watergate::core::io::fs_writer::write(const void *ptr, size_t size, size_t nitems, uint64_t timeout) {
    CHECK_STATE_AVAILABLE(state);
    CHECK_NOT_NULL(ptr);

    const control_client *client = init_utils::get_client();
    size_t written = 0;
    if (size * nitems < quota_max) {
        int err = 0;
        _lock_state r = None;
        _LOCK_(lock_name, priority, (size * nitems), timeout, err, r);
        if (r == Locked && err == 0) {
            try {
                size_t w = fwrite(ptr, size, nitems, f_ptr);
                client->release(lock_name, priority);

                CHECK_WRITTEN(w, nitems);
                written = size * nitems;
            } catch (const exception &e) {
                client->release(lock_name, priority);
                throw CONTROL_ERROR("Error while writing to file. [file=%s]\n\t[error=%s]",
                                    path->get_path().c_str(),
                                    e.what());
            }
        } else if (err != 0) {
            throw CONTROL_ERROR("Error getting write lock. [name=%s][priority=%d][error code=%d]", lock_name.c_str(),
                                priority, err);
        } else {
            written = 0;
        }
    } else {
        int err = 0;
        const char *c_ptr = static_cast<const char *>(ptr);
        size_t c_size = size * nitems;
        written = 0;
        while (written < c_size) {
            size_t rem = quota_max;
            if (rem > (c_size - written)) {
                rem = (c_size - written);
            }
            _lock_state r = None;
            _LOCK_(lock_name, priority, rem, timeout, err, r);
            if (r == Locked && err == 0) {
                try {
                    char buff[rem + 1];
                    memset(buff, 0, rem + 1);
                    memcpy(buff, c_ptr + (written * sizeof(char)), rem);
                    size_t w = fwrite(buff, rem, 1, f_ptr);
                    CHECK_WRITTEN(w, 1);
                    written += rem;
                    client->release(lock_name, priority);
                } catch (const exception &e) {
                    client->release(lock_name, priority);
                    throw CONTROL_ERROR("Error while writing to file. [file=%s]\n\t[error=%s]",
                                        path->get_path().c_str(),
                                        e.what());
                }
            } else if (err != 0) {
                throw CONTROL_ERROR("Error getting write lock. [name=%s][priority=%d][error code=%d]",
                                    lock_name.c_str(),
                                    priority, err);
            } else {
                written = 0;
            }
        }
    }
    return written;
}