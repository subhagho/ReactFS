//
// Created by Subhabrata Ghosh on 06/12/16.
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

#ifndef REACTFS_SCHEMA_PARSER_H
#define REACTFS_SCHEMA_PARSER_H

#include <unordered_map>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/time_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"

#include "core/includes/types/type_defs.h"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace types {
                    class __schema {
                    private:
                        __version_header version;
                        string schema_name;
                        __dt_struct *schema_def = nullptr;

                    public:
                        __schema(const string &schema_name, const __version_header &version) {
                            CHECK_NOT_EMPTY(schema_name);
                            this->schema_name = string(schema_name);
                            this->version.major = version.major;
                            this->version.minor = version.minor;
                        }

                        ~__schema() {
                            CHECK_AND_FREE(schema_def);
                        }

                        bool is_compatiable(const __version_header &version) {
                            return version_utils::compatible(this->version, version);
                        }

                        int compare(const __version_header &version) {
                            return version_utils::compare(this->version, version);
                        }

                        void set_schema_def(__dt_struct *schema_def) {
                            CHECK_NOT_NULL(schema_def);
                            this->schema_def = schema_def;
                        }

                        __dt_struct *get_schema_def() {
                            return this->schema_def;
                        }

                        string get_schema_name() {
                            return this->schema_name;
                        }

                        __version_header get_schema_version() {
                            return this->version;
                        }
                    };

                    class schema_parser {
                    public:
                        static __schema *parse(string schema, __version_header version) {
                            string schema_str = string_utils::toupper(schema);
                            CHECK_NOT_EMPTY(schema_str);

                            return nullptr;
                        }
                    };
                }
REACTFS_NS_CORE_END

#endif //REACTFS_SCHEMA_PARSER_H
