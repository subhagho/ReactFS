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
// Created by Subhabrata Ghosh on 25/10/16.
//

#ifndef REACTFS_FS_BLOCK_H
#define REACTFS_FS_BLOCK_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/exclusive_lock.h"

#include "fs_error_base.h"
#include "fmstream.h"


namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                typedef struct __block_record_type__ {
                    static const uint16_t RAW = 0, TYPED = 1;
                } __block_record_type;

                typedef struct __block_type__ {
                    static const uint16_t
                            PRIMARY = 0, REPLICA = 1, DELETED = 2, CORRUPTED = 3;
                } __block_type;

                typedef struct {
                    uint64_t block_id;
                    char block_uid[128];
                    uint16_t block_type = __block_type::PRIMARY;
                    uint64_t create_time;
                    uint64_t update_time;
                    uint64_t block_size;
                    uint64_t used_bytes;
                    uint64_t write_offet;
                    uint16_t record_type = __block_record_type::RAW;
                } __block_header;

                class fs_block {
                private:
                    string filename;

                protected:
                    __block_header *header = nullptr;
                    void *data_ptr = nullptr;
                    exclusive_lock *block_lock = nullptr;
                    void *write_ptr = nullptr;
                    fmstream *stream = nullptr;

                    void *write_r(const void *source, uint32_t len);

                    string _create(uint64_t block_id, string filename, uint16_t block_type,
                                   uint16_t record_type, uint64_t block_size, bool overwrite);

                    void *_open(uint64_t block_id, string filename);

                public:
                    virtual ~fs_block() {
                        CHECK_AND_FREE(block_lock);
                        close();
                    }

                    virtual string open(uint64_t block_id, string filename);

                    virtual string
                    create(uint64_t block_id, string filename, uint16_t block_type, uint64_t block_size,
                           bool overwrite);

                    virtual const void *read(uint64_t size, uint64_t offset = 0);

                    virtual const void *write(const void *source, uint32_t len);

                    virtual void close() {
                        if (NOT_NULL(stream)) {
                            if (stream->is_open())
                                stream->close();
                            delete (stream);
                            stream = nullptr;
                        }
                        header = nullptr;
                        data_ptr = nullptr;
                    }

                    uint64_t get_free_space() {
                        CHECK_NOT_NULL(header);

                        return (header->block_size - header->used_bytes);
                    }

                    uint64_t get_used_space() {
                        CHECK_NOT_NULL(header);

                        return header->used_bytes;
                    }

                    virtual void remove();

                    static string get_lock_name(uint64_t block_id) {
                        return common_utils::format("block_%lu_lock", block_id);
                    }
                };
            }
        }
    }
}


#endif //REACTFS_FS_BLOCK_H
