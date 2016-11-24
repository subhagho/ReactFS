
/*
 * Copyright [2016] [Subhabrata Ghosh]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//
// Created by Subhabrata Ghosh on 24/11/16.
//

#ifndef REACTFS_NODE_ENV_STRUCTS_H
#define REACTFS_NODE_ENV_STRUCTS_H

#include "common_structs.h"

#define NODE_ENV_BASE_DIR "__node"
#define NODE_ENV_FILE_NAME "__NODE_ENV.DATA"
#define NODE_ENV_CONFIG_NODE "/configuration/node/node-env"
#define NODE_ENV_CONFIG_THREAD_POOL "/configuration/node/thread-pool"
#define NODE_ENV_CONFIG_SHM_SIZE "data-size"
#define NODE_ENV_CONFIG_POOL_SIZE "pool-size"

#define SIZE_KEY_MAX 64

using namespace com::wookler::reactfs::common;
using namespace com::wookler::reactfs::core;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {

                typedef struct __env_record__ {
                    char key[SIZE_KEY_MAX];
                    uint32_t size = 0;
                    void *data = nullptr;
                } __env_record;

                typedef struct __env_header__ {
                    __version_header version;
                    uint64_t create_date = 0;
                    uint64_t records = 0;
                    uint64_t write_offset = 0;
                    uint64_t data_size = 0;
                } __env_header;

                class __env_loader {
                protected:
                    string *config_path;
                    string *key;
                public:
                    __env_loader(string *key) {
                        PRECONDITION(!IS_EMPTY_P(key));
                        this->key = key;
                    }

                    virtual ~__env_loader() {
                        CHECK_AND_FREE(key);
                        CHECK_AND_FREE(config_path);
                    }

                    __env_loader *set_config_path(string *config_path) {
                        PRECONDITION(!IS_EMPTY_P(config_path));
                        this->config_path = config_path;

                        return this;
                    }

                    string *get_config_path() {
                        return this->config_path;
                    }

                    string *get_key() {
                        return this->key;
                    }

                    virtual __env_record *load(const ConfigValue *config) = 0;

                    virtual void load_finished(shared_mapped_ptr record) = 0;
                };
            }
        }
    }
}
#endif //REACTFS_NODE_ENV_STRUCTS_H
