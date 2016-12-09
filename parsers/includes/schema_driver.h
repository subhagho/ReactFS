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

#define TYPE_NAME_ARRAY "array"
#define TYPE_NAME_LIST "list"
#define TYPE_NAME_MAP "map"
#define COLUMN_SORT_DIR_ASC "ASC"
#define COLUMN_SORT_DIR_DESC "DESC"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace parsers {
                    typedef enum __schema_parse_state__ {
                        SPS_NONE = 0, SPS_IN_TYPE, SPS_IN_SCHEMA, SPS_IN_DECLARE
                    } __schema_parse_state;

                    typedef struct __declare_stack__ {
                        __constraint_def *current_constraint = nullptr;
                        string *default_value = nullptr;
                    } __declare_stack;

                    class __type_stack {
                    public:
                        __reference_type *current_type = nullptr;
                        vector<__declare *> declares;
                        __declare_stack declare;

                        __type_stack() {
                            this->current_type = nullptr;
                            this->declare.current_constraint = nullptr;
                            this->declare.default_value = nullptr;
                        }

                        virtual ~__type_stack() {
                            this->declares.clear();
                            FREE_PTR(this->declare.default_value);
                            FREE_PTR(this->declare.current_constraint);
                            this->current_type = nullptr;
                        }
                    };

                    class __schema_stack {
                    public:
                        __schema_def *current_schema = nullptr;
                        vector<__declare *> declares;
                        __declare_stack declare;
                        vector<__index_def *> indexes;
                        string *pk_columns = nullptr;
                        __index_def *current_index = nullptr;

                        __schema_stack() {
                            this->current_schema = nullptr;
                            this->declare.current_constraint = nullptr;
                            this->declare.default_value = nullptr;
                            this->current_index = nullptr;
                            CHECK_AND_FREE(pk_columns);
                        }

                        ~__schema_stack() {
                            this->declares.clear();
                            FREE_PTR(this->declare.default_value);
                            FREE_PTR(this->declare.current_constraint);
                            this->current_schema = nullptr;
                            this->pk_columns = nullptr;
                            this->current_index = nullptr;
                        }
                    };

                    class schema_driver {
                    private:
                        schema_parser *parser = nullptr;
                        schema_scanner *scanner = nullptr;
                        __type_stack *type_stack = nullptr;
                        __schema_stack *schema_stack = nullptr;

                        unordered_map<string, __reference_type *> types;

                        __schema_parse_state state = __schema_parse_state::SPS_NONE;

                        void parse_helper(std::istream &stream);

                        void finish_type();

                        void finish_schema();

                        void clear_type_stack() {
                            if (NOT_NULL(type_stack)) {
                                CHECK_AND_FREE(type_stack);
                            }
                        }

                        void free_schema() {
                            if (NOT_NULL(schema_stack)) {
                                free_schema_def(schema_stack->current_schema);
                                CHECK_AND_FREE(schema_stack->pk_columns);
                                CHECK_AND_FREE(schema_stack);
                            }
                        }

                        void free_schema_def(__schema_def *schema) {
                            if (NOT_NULL(schema)) {
                                if (NOT_NULL(schema->members)) {
                                    __declare *ptr = schema->members;
                                    while (NOT_NULL(ptr)) {
                                        __declare *next = ptr->next;
                                        free_declare(ptr);
                                        ptr = next;
                                    }
                                }
                                schema->members = nullptr;
                                free_key_column(schema->pk_columns);
                                free_index_defs(schema->indexes);
                            }
                        }

                        void free_index_defs(__index_def *index) {
                            if (NOT_NULL(index)) {
                                __index_def *ptr = index;
                                while (NOT_NULL(ptr)) {
                                    __index_def *next = ptr->next;
                                    free_key_column(ptr->columns);
                                    FREE_PTR(ptr);
                                    ptr = next;
                                }
                            }
                        }

                        void free_key_column(__key_column *c) {
                            if (NOT_NULL(c)) {
                                while (NOT_NULL(c)) {
                                    __key_column *next = c->next;
                                    FREE_PTR(c);
                                    c = next;
                                }
                            }
                        }

                        void free_type(__reference_type *type) {
                            if (NOT_NULL(type)) {
                                if (NOT_NULL(type->members)) {
                                    __declare *ptr = type->members;
                                    while (NOT_NULL(ptr)) {
                                        __declare *next = ptr->next;
                                        free_declare(ptr);
                                        ptr = next;
                                    }
                                    type->members = nullptr;
                                }
                                FREE_PTR(type);
                            }
                            type = nullptr;
                        }

                        void free_declare(__declare *ptr) {
                            if (NOT_NULL(ptr)) {
                                CHECK_AND_FREE(ptr->default_value);
                                free_constraint_def(ptr->constraint);
                                if (NOT_NULL(ptr->inner_types)) {
                                    __declare *iptr = ptr->inner_types;
                                    while (NOT_NULL(iptr)) {
                                        __declare *next = iptr->next;
                                        free_declare(iptr);
                                        iptr = next;
                                    }
                                }
                                CHECK_AND_FREE(ptr->type);
                                CHECK_AND_FREE(ptr->variable);
                                FREE_PTR(ptr);
                            }
                            ptr = nullptr;
                        }

                        void free_constraint_def(__constraint_def *ptr) {
                            if (NOT_NULL(ptr)) {
                                CHECK_AND_FREE(ptr->constraint);
                                CHECK_AND_FREE(ptr->type);
                                CHECK_AND_FREE(ptr->values);
                                FREE_PTR(ptr);
                            }
                            ptr = nullptr;
                        }

                        __constraint_def *get_type_var_constraint(const string &type) {
                            CHECK_NOT_NULL(type_stack);
                            if (NOT_NULL(type_stack->declare.current_constraint)) {
                                __constraint_def *def = type_stack->declare.current_constraint;
                                def->type = new string(type);
                                type_stack->declare.current_constraint = nullptr;
                                return def;
                            }
                            return nullptr;
                        }

                        string *get_type_default_value() {
                            CHECK_NOT_NULL(type_stack);
                            if (NOT_NULL(type_stack->declare.default_value)) {
                                string *def_value = type_stack->declare.default_value;
                                type_stack->declare.default_value = nullptr;
                                return def_value;
                            }
                            return nullptr;
                        }

                        __constraint_def *get_schema_var_constraint(const string &type) {
                            CHECK_NOT_NULL(schema_stack);
                            if (NOT_NULL(schema_stack->declare.current_constraint)) {
                                __constraint_def *def = schema_stack->declare.current_constraint;
                                def->type = new string(type);
                                schema_stack->declare.current_constraint = nullptr;
                                return def;
                            }
                            return nullptr;
                        }

                        string *get_schema_default_value() {
                            CHECK_NOT_NULL(schema_stack);
                            if (NOT_NULL(schema_stack->declare.default_value)) {
                                string *def_value = schema_stack->declare.default_value;
                                schema_stack->declare.default_value = nullptr;
                                return def_value;
                            }
                            return nullptr;
                        }

                        __key_column *get_key_column(const string &s) {
                            CHECK_NOT_EMPTY(s);
                            string array[2];
                            array[0] = common_consts::EMPTY_STRING;
                            array[1] = common_consts::EMPTY_STRING;

                            uint32_t c = string_utils::split(s, ' ', array, 2);
                            POSTCONDITION(c > 0);
                            CHECK_NOT_EMPTY(array[0]);

                            __key_column *kc = (__key_column *) malloc(sizeof(__key_column));
                            CHECK_ALLOC(kc, TYPE_NAME(__key_column));
                            memset(kc, 0, sizeof(__key_column));

                            kc->name = string(array[0]);
                            if (c > 1) {
                                if (array[1] == COLUMN_SORT_DIR_ASC) {
                                    kc->sort_asc = true;
                                } else if (array[1] == COLUMN_SORT_DIR_DESC) {
                                    kc->sort_asc = false;
                                }
                            }
                            kc->next = nullptr;
                            return kc;
                        }

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

                        void create_schema(const string &name);

                        void add_declaration(const string &varname, const string &type, bool is_ref = false);

                        void
                        add_array_decl(const string &varname, uint16_t size, const string &type, bool is_ref = false);

                        void
                        add_list_decl(const string &varname, const string &type, bool is_ref = false);

                        void
                        add_map_decl(const string &varname, const string &ktype, const string &vtype,
                                     bool is_ref = false);

                        void set_constraint(bool is_not, const string &constraint, const string &values);

                        void set_default_value(const string &value);

                        void set_primary_key(const string &keys);

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
