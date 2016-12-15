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

#include "types/includes/type_defs.h"
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
                        unordered_map<string, __reference_type *> *types;
                        /// Map of parsed indexes defined in the schema file.
                        unordered_map<string, __index_def *> *indexes;

                        __default *create_default_value(__declare *field, __type_def_enum type) {
                            string *value = field->default_value;
                            CHECK_NOT_EMPTY_P(value);
                            __default *d = __defaults_loader::create(value, type);
                            return d;
                        }

                        __constraint *create_constraint(__declare *field, __type_def_enum type) {
                            __constraint_def *def = field->constraint;
                            CHECK_NOT_NULL(def);
                            CHECK_NOT_EMPTY_P(def->constraint);
                            CHECK_NOT_EMPTY_P(def->values);

                            __constraint *c = __constraint_loader::create(def->constraint, def->values, type,
                                                                          def->is_not);
                            CHECK_NOT_NULL(c);

                            return c;
                        }

                        __complex_type *
                        create_type(__native_type *parent, string &name, __reference_type *type, uint8_t index) {
                            CHECK_NOT_NULL(parent);
                            CHECK_NOT_NULL(type);
                            CHECK_NOT_EMPTY(name);

                            __complex_type *ct = new __complex_type(parent, index, name);
                            CHECK_ALLOC(ct, TYPE_NAME(__complex_type));

                            __declare *ptr = type->members;
                            uint8_t t_index = 0;
                            while (NOT_NULL(ptr)) {
                                add_field(ct, ptr, t_index);
                                ptr = ptr->next;
                                t_index++;
                                if (t_index >= UCHAR_MAX) {
                                    throw TYPE_PARSER_ERROR(
                                            "Exceeds maximum number of fields allowed. [max=%d][type=%s]",
                                            UCHAR_MAX, ct->get_name().c_str());
                                }
                            }
                            return ct;
                        }

                        __native_type *create_inner_type(__native_type *parent, __declare *field, string &name) {
                            CHECK_NOT_NULL(parent);
                            CHECK_NOT_NULL(field);

                            if (!field->is_reference) {
                                __type_def_enum type = __type_enum_helper::parse_type(*(field->type));
                                POSTCONDITION(type != __type_def_enum::TYPE_UNKNOWN);
                                PRECONDITION(__type_enum_helper::is_inner_type_valid(type));
                                if (__type_enum_helper::is_native(type)) {
                                    __native_type *nt = new __native_type(parent, 0, name, type);
                                    CHECK_ALLOC(nt, TYPE_NAME(__native_type));
                                    return nt;
                                }
                            } else {
                                string *r_type = field->type;
                                unordered_map<string, __reference_type *>::iterator iter = types->find(*r_type);
                                if (iter == types->end()) {
                                    throw BASE_ERROR("Cannot find specified type definition. [type=%s]",
                                                     r_type->c_str());
                                }
                                __reference_type *rt = iter->second;
                                CHECK_NOT_NULL(rt);
                                return create_type(parent, name, rt, 0);
                            }
                            return nullptr;
                        }

                        void add_field(__complex_type *type, __declare *field, uint8_t index) {
                            CHECK_NOT_NULL(type);
                            CHECK_NOT_NULL(field);
                            CHECK_NOT_EMPTY_P(field->type);
                            CHECK_NOT_EMPTY_P(field->variable);

                            __native_type *nt = nullptr;
                            if (!field->is_reference) {
                                __type_def_enum ft = __type_enum_helper::parse_type(*(field->type));
                                POSTCONDITION(ft != __type_def_enum::TYPE_UNKNOWN);
                                if (__type_enum_helper::is_native(ft)) {
                                    nt = new __native_type(type, index, *(field->variable), ft);
                                    CHECK_ALLOC(nt, TYPE_NAME(__native_type));
                                } else if (ft == __type_def_enum::TYPE_ARRAY) {
                                    uint32_t size = field->size;
                                    POSTCONDITION(size > 0);


                                    __declare *inner_type = field->inner_types;
                                    CHECK_NOT_NULL(inner_type);
                                    if (!inner_type->is_reference) {
                                        __type_def_enum it = __type_enum_helper::parse_type(*(inner_type->type));
                                        POSTCONDITION(__type_enum_helper::is_inner_type_valid(it));

                                        __array_type *at = new __array_type(type, index, *(field->variable), it, size);
                                        CHECK_ALLOC(at, TYPE_NAME(__array_type));

                                        __native_type *inner = create_inner_type(at, inner_type, *(field->variable));
                                        CHECK_NOT_NULL(inner);
                                        at->set_inner_type(inner);
                                        nt = at;
                                    } else {
                                        __type_def_enum it = __type_def_enum::TYPE_STRUCT;

                                        __array_type *at = new __array_type(type, index, *(field->variable), it, size);
                                        CHECK_ALLOC(at, TYPE_NAME(__array_type));

                                        __native_type *inner = create_inner_type(at, inner_type, *(field->variable));
                                        CHECK_NOT_NULL(inner);
                                        at->set_inner_type(inner);
                                        nt = at;
                                    }
                                } else if (ft == __type_def_enum::TYPE_LIST) {

                                    __declare *inner_type = field->inner_types;
                                    CHECK_NOT_NULL(inner_type);
                                    if (!inner_type->is_reference) {
                                        __type_def_enum it = __type_enum_helper::parse_type(*(inner_type->type));
                                        POSTCONDITION(__type_enum_helper::is_inner_type_valid(it));

                                        __list_type *at = new __list_type(type, index, *(field->variable), it);
                                        CHECK_ALLOC(at, TYPE_NAME(__list_type));

                                        __native_type *inner = create_inner_type(at, inner_type, *(field->variable));
                                        CHECK_NOT_NULL(inner);
                                        at->set_inner_type(inner);

                                        nt = at;
                                    } else {
                                        __type_def_enum it = __type_def_enum::TYPE_STRUCT;

                                        __list_type *at = new __list_type(type, index, *(field->variable), it);
                                        CHECK_ALLOC(at, TYPE_NAME(__list_type));

                                        __native_type *inner = create_inner_type(at, inner_type, *(field->variable));
                                        CHECK_NOT_NULL(inner);
                                        at->set_inner_type(inner);

                                        nt = at;
                                    }
                                } else if (ft == __type_def_enum::TYPE_MAP) {
                                    __declare *key_type = field->inner_types;
                                    CHECK_NOT_NULL(key_type);
                                    __declare *value_type = field->inner_types->next;
                                    CHECK_NOT_NULL(value_type);

                                    __type_def_enum kt = __type_enum_helper::parse_type(*(key_type->type));
                                    POSTCONDITION(__type_enum_helper::is_native(kt));

                                    __type_def_enum vt = __type_def_enum::TYPE_UNKNOWN;

                                    if (value_type->is_reference) {
                                        vt = __type_def_enum::TYPE_STRUCT;
                                    } else {
                                        vt = __type_enum_helper::parse_type(*(value_type->type));
                                    }
                                    POSTCONDITION(__type_enum_helper::is_inner_type_valid(vt));

                                    __map_type *mt = new __map_type(type, index, *(field->variable), kt, vt);
                                    CHECK_ALLOC(mt, TYPE_NAME(__map_type));

                                    string key_name(MAP_TYPE_KEY_NAME);
                                    __native_type *key = create_inner_type(mt, key_type, key_name);
                                    CHECK_NOT_NULL(key);
                                    string value_name(MAP_TYPE_VALUE_NAME);
                                    __native_type *value = create_inner_type(mt, value_type, value_name);
                                    CHECK_NOT_NULL(value);

                                    mt->set_key_type(key);
                                    mt->set_value_type(value);

                                    nt = mt;
                                }
                            } else {
                                string *r_type = field->type;
                                unordered_map<string, __reference_type *>::iterator iter = types->find(*r_type);
                                if (iter == types->end()) {
                                    throw BASE_ERROR("Cannot find specified type definition. [type=%s]",
                                                     r_type->c_str());
                                }
                                __reference_type *rt = iter->second;
                                CHECK_NOT_NULL(rt);
                                nt = create_type(type, *(field->variable), rt, index);
                            }
                            CHECK_NOT_NULL(nt);
                            nt->set_nullable(field->is_nullable);
                            if (NOT_NULL(field->constraint)) {
                                __constraint *c = create_constraint(field, nt->get_datatype());
                                CHECK_NOT_NULL(c);
                                nt->set_constraint(c);
                            }
                            if (NOT_NULL(field->default_value)) {
                                __default *d = create_default_value(field, nt->get_datatype());
                                CHECK_NOT_NULL(d);
                                nt->set_default_value(d);
                            }
                            type->add_field(nt, index);
                        }

                    public:
                        translator(__schema_def *schema_def, unordered_map<string, __reference_type *> *types,
                                   unordered_map<string, __index_def *> *indexes) {
                            CHECK_NOT_NULL(schema_def);
                            this->schema_def = schema_def;
                            this->types = types;
                            this->indexes = indexes;
                        }

                        __complex_type *translate() {
                            __complex_type *type = new __complex_type(nullptr, *(schema_def->name));
                            CHECK_ALLOC(type, TYPE_NAME(__complex_type));
                            __declare *ptr = schema_def->members;
                            uint8_t index = 0;
                            while (NOT_NULL(ptr)) {
                                add_field(type, ptr, index);
                                ptr = ptr->next;
                                index++;
                                if (index >= UCHAR_MAX) {
                                    throw TYPE_PARSER_ERROR(
                                            "Exceeds maximum number of fields allowed. [max=%d][schema=%s]",
                                            UCHAR_MAX, type->get_name().c_str());
                                }
                            }
                            return type;
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
