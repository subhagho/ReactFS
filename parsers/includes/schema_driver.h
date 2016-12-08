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

#include <iostream>
#include <vector>
#include <unordered_map>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/time_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"

#include "core/includes/types/type_defs.h"
#include "schema.h"
#include "schema_scanner.h"

#define TYPE_NAME_ARRAY "ARRAY"
#define TYPE_NAME_LIST "LIST"
#define TYPE_NAME_MAP "MAP"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace parsers {
                    typedef enum __schema_parse_state__ {
                        SPS_NONE = 0, SPS_IN_TYPE, SPS_IN_SCHEMA, SPS_IN_DECLARE
                    } __schema_parse_state;

                    class schema_driver {
                    private:
                        schema_parser *parser = nullptr;
                        schema_scanner *scanner = nullptr;

                        vector<__declare *> declares;
                        unordered_map<string, __reference_type *> types;
                        __reference_type *current_type = nullptr;
                        __constraint_def *current_constraint = nullptr;

                        __schema_parse_state state = __schema_parse_state::SPS_NONE;

                        void parse_helper(std::istream &stream);

                        void finish_type();

                        void finish_schema();

                        void check_new_type(const string &name) {
                            unordered_map<string, __reference_type *>::iterator iter = types.find(name);
                            if (iter != types.end()) {
                                throw TYPE_PARSER_ERROR("Duplicate type defined. [name=%s]", name.c_str());
                            }
                        }

                        void check_reference_type(const string &name) {
                            unordered_map<string, __reference_type *>::iterator iter = types.find(name);
                            if (iter == types.end()) {
                                throw TYPE_PARSER_ERROR("Reference type not defined. [name=%s]", name.c_str());
                            }
                        }

                    public:
                        schema_driver() = default;

                        ~schema_driver();

                        void add_type(const string &name);

                        void add_declaration(const string &varname, const string &type, bool is_ref = false);

                        void
                        add_array_decl(const string &varname, uint16_t size, const string &type, bool is_ref = false);

                        void
                        add_list_decl(const string &varname, const string &type, bool is_ref = false);

                        void
                        add_map_decl(const string &varname, const string &ktype, const string &vtype,
                                     bool is_ref = false);

                        void set_constraint(bool is_not, const string &constraint, const string &values);

                        void finish_def();

                        /**
                        * parse - parse from a file
                        * @param filename - valid string with input file
                        */
                        void parse(const char *const filename);

                        void parse(std::istream &iss);
                    };
                }
REACTFS_NS_CORE_END

#endif //REACTFS_SCHEMA_PARSER_H
