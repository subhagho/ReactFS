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
using namespace REACTFS_NS_CORE_PREFIX::types;

REACTFS_NS_CORE
                namespace parsers {
                    /*!
                     * Enum definitions for the parser state model.
                     */
                    typedef enum __schema_parse_state__ {
                        /// State is undefined.
                                SPS_NONE = 0,
                        /// Parser is in the processes of parsing a type declaration.
                                SPS_IN_TYPE,
                        /// Parser is in the processes of parsing a schema declaration.
                                SPS_IN_SCHEMA,
                        /// Parser is in the processes of parsing an index declaration.
                                SPS_IN_INDEX
                    } __schema_parse_state;

                    /*!
                     * Struct for defining the stack for capturing field declarations.
                     */
                    typedef struct __declare_stack__ {
                        /// Constraint definition for the current field.
                        __constraint_def *current_constraint = nullptr;
                        /// Default value definition for the current field.
                        string *default_value = nullptr;
                        /// Is the current field nullable.
                        bool nullable = false;
                    } __declare_stack;

                    /*!
                    * Class for defining the stack for capturing type declarations.
                    *
                    * Utility class, hence members are public.
                    */
                    class __type_stack {
                    public:
                        /// Instance of the current type being defined.
                        __reference_type *current_type = nullptr;
                        /// List of field declarations for the current type.
                        vector<__declare *> declares;
                        /// Last variable declaration stack.
                        __declare_stack declare;

                        /*!
                         * Default empty constructor.
                         */
                        __type_stack() {
                            this->current_type = nullptr;
                            this->declare.current_constraint = nullptr;
                            this->declare.default_value = nullptr;
                        }

                        /*!
                         * Default destructor.
                         */
                        virtual ~__type_stack() {
                            this->declares.clear();
                            FREE_PTR(this->declare.default_value);
                            FREE_PTR(this->declare.current_constraint);
                            this->current_type = nullptr;
                        }
                    };

                    /*!
                    * Class for defining the stack for capturing schema declarations.
                    *
                    * Utility class, hence members are public.
                    */
                    class __schema_stack {
                    public:
                        /// Schema definition instance.
                        __schema_def *current_schema = nullptr;
                        /// Fields declared for this schema definition.
                        vector<__declare *> declares;
                        /// Last variable declaration stack.
                        __declare_stack declare;
                        /// Primary Key columns (if any)
                        string *pk_columns = nullptr;

                        /*!
                         * Default empty constructor.
                         */
                        __schema_stack() {
                            this->current_schema = nullptr;
                            this->declare.current_constraint = nullptr;
                            this->declare.default_value = nullptr;
                            CHECK_AND_FREE(pk_columns);
                        }

                        /*!
                         * Default destructor.
                         */
                        ~__schema_stack() {
                            this->declares.clear();
                            FREE_PTR(this->declare.default_value);
                            FREE_PTR(this->declare.current_constraint);
                            this->current_schema = nullptr;
                            this->pk_columns = nullptr;
                        }
                    };

                    /*!
                    * Class for defining the stack for capturing index declarations.
                    *
                    * Utility class, hence members are public.
                    */
                    class __index_stack {
                    public:
                        /// Current index being defined.
                        __index_def *current_index = nullptr;
                        /// Comma seperated list of index fields.
                        string *fields = nullptr;
                        /// Type of the index
                        string *index_type = nullptr;

                        /*!
                         * Default empty constructor.
                         */
                        __index_stack() {
                            this->current_index = nullptr;
                            this->fields = nullptr;
                            this->index_type = nullptr;
                        }

                        /*!
                         * Default destructor.
                         */
                        ~__index_stack() {
                            CHECK_AND_FREE(fields);
                            CHECK_AND_FREE(index_type);
                            current_index = nullptr;
                        }
                    };

                    class translator {
                    private:
                        /// Schema definition.
                        __schema_def *schema_def = nullptr;
                        /// Map of parsed types defined in the schema file.
                        unordered_map<string, __reference_type *> types;
                        /// Map of parsed indexes defined in the schema file.
                        unordered_map<string, __index_def *> indexes;

                    public:
                        translator(__schema_def *schema_def, const unordered_map<string, __reference_type *> &types,
                                   const unordered_map<string, __index_def *> &indexes) {
                            CHECK_NOT_NULL(schema_def);
                            this->schema_def = schema_def;
                            this->types = types;
                            this->indexes = indexes;
                        }

                        __complex_type *translate() {
                            return nullptr;
                        }
                    };

                    /*!
                     * The schema driver class is responsible for parsing the schema/type/index
                     * definitions. This class creates temporary definitions that will be translated
                     * to the final schema (__complex_type) definition.
                     *
                     * This class's methods are used by the bison parser to parse the schema/type/index.
                     */
                    class schema_driver {
                    private:
                        /// Bison generated Schema Parser
                        schema_parser *parser = nullptr;
                        /// Flex generate Schema Scanner (Lexer)
                        schema_scanner *scanner = nullptr;
                        /// Type stack instance to capture types being parsed.
                        __type_stack *type_stack = nullptr;
                        /// Schema stack instance to capture the schema being parsed.
                        __schema_stack *schema_stack = nullptr;
                        /// Index stack instance to capture indexes being parsed.
                        __index_stack *index_stack = nullptr;

                        /// Map of parsed types defined in the schema file.
                        unordered_map<string, __reference_type *> types;
                        /// Map of parsed indexes defined in the schema file.
                        unordered_map<string, __index_def *> indexes;

                        /// Current parse state of the schema parser.
                        __schema_parse_state state = __schema_parse_state::SPS_NONE;

                        void parse_helper(std::istream &stream);

                        void finish_type();

                        void finish_schema();

                        void finish_index();

                        void clear_type_stack() {
                            if (NOT_NULL(type_stack)) {
                                type_stack->current_type = nullptr;
                                type_stack->declare.current_constraint = nullptr;
                                type_stack->declare.default_value = nullptr;
                                type_stack->declares.clear();
                            }
                        }

                        void clear_index_stack() {
                            if (NOT_NULL(index_stack)) {
                                index_stack->current_index = nullptr;
                                CHECK_AND_FREE(index_stack->index_type);
                                CHECK_AND_FREE(index_stack->fields);
                            }
                        }

                        void free_schema() {
                            if (NOT_NULL(schema_stack)) {
                                free_schema_def(schema_stack->current_schema);
                                CHECK_AND_FREE(schema_stack->pk_columns);
                                CHECK_AND_FREE(schema_stack);
                            }
                        }

                        void free_index_def(__index_def *index) {
                            if (NOT_NULL(index)) {
                                free_key_column(index->columns);
                                CHECK_AND_FREE(index->name);
                                CHECK_AND_FREE(index->schema_name);
                                FREE_PTR(index);
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
                                CHECK_AND_FREE(schema->name);
                                schema->members = nullptr;
                                free_key_column(schema->pk_columns);
                            }
                        }

                        void free_key_column(__key_column *c) {
                            while (NOT_NULL(c)) {
                                __key_column *next = c->next;
                                CHECK_AND_FREE(c->name);
                                FREE_PTR(c);
                                c = next;
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
                                CHECK_AND_FREE(type->name);
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

                            kc->name = new string(array[0]);
                            if (c > 1) {
                                if (array[1] == COLUMN_SORT_DIR_ASC) {
                                    kc->sort_asc = true;
                                } else if (array[1] == COLUMN_SORT_DIR_DESC) {
                                    kc->sort_asc = false;
                                }
                            } else {
                                kc->sort_asc = true;
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

                        void check_new_index(const string &name) {
                            unordered_map<string, __index_def *>::iterator iter = indexes.find(name);
                            if (iter != indexes.end()) {
                                throw TYPE_PARSER_ERROR("Duplicate index defined. [name=%s]", name.c_str());
                            }
                        }

                    public:
                        schema_driver() = default;

                        ~schema_driver();

                        void add_type(const string &name);

                        void create_schema(const string &name);

                        void create_index(const string &name, const string &schema);

                        void set_index_fields(const string &fields);

                        void set_index_type(const string &type);

                        void add_declaration(const string &varname, const string &type, bool is_ref,
                                             int nullable);

                        void
                        add_array_decl(const string &varname, uint16_t size, const string &type, bool is_ref,
                                       int nullable);

                        void
                        add_list_decl(const string &varname, const string &type, bool is_ref,
                                      int nullable);

                        void
                        add_map_decl(const string &varname, const string &ktype, const string &vtype,
                                     bool is_ref, int nullable);

                        void set_constraint(bool is_not, const string &constraint, const string &values);

                        void set_default_value(const string &value);

                        void set_nullable();

                        void set_primary_key(const string &keys);

                        void finish_def();

                        /**
                        * parse - parse from a file
                        * @param filename - valid string with input file
                        */
                        void parse(const char *const filename);

                        void parse(std::istream &iss);

                        __complex_type *translate();
                    };
                }
REACTFS_NS_CORE_END

#endif //REACTFS_SCHEMA_PARSER_H
