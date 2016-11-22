//
// Created by Subhabrata Ghosh on 22/11/16.
//

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

#ifndef REACTFS_NODE_ENV_H
#define REACTFS_NODE_ENV_H

#include <unordered_map>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/config.h"
#include "common/includes/__env.h"
#include "common/includes/mapped_data.h"
#include "common/includes/init_utils.h"
#include "common/includes/exclusive_lock.h"

#include "common_structs.h"

#define NODE_ENV_BASE_DIR "__node"
#define NODE_ENV_FILE_NAME "__NODE_ENV.DATA"
#define NODE_ENV_CONFIG_NODE "/configuration/node-env"
#define NODE_ENV_CONFIG_SHM_SIZE "data-size"

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

                class __node_env {
                protected:
                    __state__ state;

                    exclusive_lock *lock = nullptr;
                    const __env *env = nullptr;

                    file_mapped_data *data_ptr = nullptr;
                    void *base_ptr = nullptr;
                    __env_header *header = nullptr;
                    unordered_map<string, shared_mapped_ptr> data_index;

                    void *get_write_pointer() {
                        PRECONDITION(header->write_offset < header->data_size);
                        void *ptr = common_utils::increment_data_ptr(base_ptr, header->write_offset);
                        CHECK_NOT_NULL(ptr);

                        return ptr;
                    }

                    void *write(string *key, void *data, uint32_t size) {
                        CHECK_STATE_AVAILABLE(state);
                        CHECK_NOT_NULL(data);
                        PRECONDITION((header->write_offset + size + sizeof(uint32_t)) <= header->data_size);

                        void *ptr = get_write_pointer();

                        memset(ptr, 0, SIZE_KEY_MAX);
                        memcpy(ptr, key->c_str(), key->length());

                        ptr = common_utils::increment_data_ptr(ptr, SIZE_KEY_MAX);
                        memset(ptr, 0, (size + sizeof(uint32_t)));
                        memcpy(ptr, &size, sizeof(uint32_t));

                        ptr = common_utils::increment_data_ptr(ptr, sizeof(uint32_t));
                        CHECK_NOT_NULL(ptr);
                        memcpy(ptr, data, size);

                        header->write_offset += (size + SIZE_KEY_MAX + sizeof(uint32_t));

                        return ptr;
                    }

                    void create_new_file(Path *p, uint64_t size) {
                        data_ptr = new file_mapped_data(p->get_path(), size, false);
                        CHECK_NOT_NULL(data_ptr);
                        base_ptr = data_ptr->get_base_ptr();

                        header = static_cast<__env_header *>(base_ptr);
                        header->create_date = time_utils::now();
                        header->records = 0;
                        header->data_size = size;
                        header->write_offset = sizeof(__env_header);

                        data_ptr->flush();
                    }

                    void *get_read_ptr(uint32_t offset) {
                        void *ptr = base_ptr;
                        offset += sizeof(__env_header);

                        return common_utils::increment_data_ptr(ptr, offset);
                    }

                    bool is_valid_offset(uint32_t offset) {
                        if ((sizeof(__env_header) + offset) < header->data_size) {
                            return true;
                        }
                        return false;
                    }

                    void load_env_data(Path *p) {
                        data_ptr = new file_mapped_data(p->get_path());
                        CHECK_NOT_NULL(data_ptr);
                        base_ptr = data_ptr->get_base_ptr();

                        header = static_cast<__env_header *>(base_ptr);

                        uint32_t count = 0;
                        uint32_t offset = 0;
                        __env_record *record = (__env_record *) malloc(sizeof(__env_record));
                        CHECK_NOT_NULL(record);
                        while (count < header->records && is_valid_offset(offset)) {
                            void *ptr = get_read_ptr(offset);
                            CHECK_NOT_NULL(ptr);
                            memset(record, 0, sizeof(__env_record));

                            memcpy(record->key, ptr, SIZE_KEY_MAX);
                            ptr = common_utils::increment_data_ptr(ptr, SIZE_KEY_MAX);

                            memcpy(&record->size, ptr, sizeof(uint32_t));
                            ptr = common_utils::increment_data_ptr(ptr, sizeof(uint32_t));

                            record->data = ptr;

                            shared_mapped_ptr s_ptr = make_shared<__mapped_ptr>();
                            (*s_ptr).set_data_ptr(ptr, record->size);

                            data_index.insert({string(record->key), s_ptr});

                            offset += (record->size + sizeof(uint32_t) + SIZE_KEY_MAX);

                            count++;
                        }
                        POSTCONDITION(count == header->records);
                    }

                public:
                    __node_env() {
                        this->env = env_utils::get_env();
                        CHECK_NOT_NULL(this->env);
                        CHECK_STATE_AVAILABLE(this->env->get_state());

                        const Config *config = this->env->get_config();
                        CHECK_NOT_NULL(config);

                        const ConfigValue *node = config->find(NODE_ENV_CONFIG_NODE);
                        CHECK_NOT_NULL(node);

                        const BasicConfigValue *s_node = Config::get_value(NODE_ENV_CONFIG_SHM_SIZE, node);
                        CHECK_NOT_NULL(s_node);
                        string s_size = s_node->get_value();
                        POSTCONDITION(!IS_EMPTY(s_size));

                        uint64_t size = (uint64_t) common_utils::parse_size(s_size);
                        POSTCONDITION(size > 0);

                        string name_l = string(NODE_ENV_FILE_NAME);
                        lock = new exclusive_lock(&name_l, DEFAULT_RESOURCE_MODE);
                        CHECK_NOT_NULL(lock);
                        WAIT_LOCK_P(lock);
                        try {
                            const Path *w_dir = env->get_work_dir();
                            CHECK_NOT_NULL(w_dir);
                            Path p(w_dir->get_path());
                            p.append(NODE_ENV_BASE_DIR);
                            if (!p.exists()) {
                                p.create(DEFAULT_RESOURCE_MODE);
                            }
                            p.append(NODE_ENV_FILE_NAME);
                            if (!p.exists()) {
                                create_new_file(&p, size);
                            } else {
                                load_env_data(&p);
                            }

                            state.set_state(__state_enum::Available);
                            RELEASE_LOCK_P(lock);
                        } catch (const exception &e) {
                            base_error err = BASE_ERROR("Error occurred while creating node envirnoment. [error=%s]",
                                                        e.what());
                            state.set_error(&err);
                            throw err;
                        } catch (...) {
                            base_error err = BASE_ERROR("Error occurred while creating node envirnoment. [error=%s]",
                                                        "UNKNOWN ERROR");
                            state.set_error(&err);
                            throw err;
                        }
                    }

                    virtual ~__node_env() {
                        this->env = nullptr;
                        data_index.clear();
                        CHECK_AND_FREE(data_ptr);
                    }

                    __state__ get_state() {
                        return this->state;
                    }

                    bool add_env_data(__env_loader *loader) {
                        CHECK_STATE_AVAILABLE(state);
                        CHECK_NOT_NULL(loader);
                        PRECONDITION(!IS_EMPTY_P(loader->get_key()));
                        PRECONDITION(!IS_EMPTY_P(loader->get_config_path()));

                        const Config *config = env->get_config();
                        const ConfigValue *node = config->find(*loader->get_config_path());

                        __env_record *record = loader->load(node);
                        if (NOT_NULL(record)) {
                            void *ptr = write(loader->get_key(), record->data, record->size);
                            CHECK_NOT_NULL(ptr);

                            shared_mapped_ptr s_ptr = make_shared<__mapped_ptr>();
                            (*s_ptr).set_data_ptr(ptr, record->size);

                            data_index.insert({*(loader->get_key()), s_ptr});

                            loader->load_finished(s_ptr);

                            return true;
                        }
                        return false;
                    }

                    shared_mapped_ptr get_env_data(string key) {
                        unordered_map<string, shared_mapped_ptr>::const_iterator iter = data_index.find(key);
                        if (iter != data_index.end()) {
                            return iter->second;
                        }
                        return nullptr;
                    }
                };

                class node_utils {
                private:
                    static __node_env *node_env;

                public:
                    static void create_node_env() {
                        node_env = new __node_env();
                    }

                    static __node_env *get_node_env() {
                        CHECK_NOT_NULL(node_env);
                        CHECK_STATE_AVAILABLE(node_env->get_state());

                        return node_env;
                    }
                };
            }
        }
    }
}
#endif //REACTFS_NODE_ENV_H
