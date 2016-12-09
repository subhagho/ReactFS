//
// Created by Subhabrata Ghosh on 07/12/16.
//
#include <cassert>

#include "common/includes/log_utils.h"
#include "schema_driver.h"

com::wookler::reactfs::core::parsers::schema_driver::~schema_driver() {
    CHECK_AND_FREE(scanner);
    CHECK_AND_FREE(parser);

    unordered_map<string, __reference_type *>::iterator iter;
    for (iter = types.begin(); iter != types.end(); iter++) {
        free_type(iter->second);
    }
    types.clear();

    free_schema();
}

void com::wookler::reactfs::core::parsers::schema_driver::create_schema(const string &name) {
    PRECONDITION(IS_NULL(type_stack));
    PRECONDITION(IS_NULL(schema_stack));

    schema_stack = new __schema_stack();
    CHECK_ALLOC(schema_stack, TYPE_NAME(__schema_stack));

    schema_stack->current_schema = (__schema_def *) malloc(sizeof(__schema_def));
    CHECK_ALLOC(schema_stack->current_schema, TYPE_NAME(__schema_def));
    memset(schema_stack->current_schema, 0, sizeof(__schema_def));

    schema_stack->current_schema->name = string(name);

    state = __schema_parse_state::SPS_IN_SCHEMA;

    LOG_DEBUG("Added schema definition. [name=%s]", schema_stack->current_schema->name.c_str());
}

void com::wookler::reactfs::core::parsers::schema_driver::add_type(const string &name) {
    LOG_DEBUG("Adding new datatype. [name=%s]", name.c_str());

    __reference_type *type = (__reference_type *) malloc(sizeof(__reference_type));
    CHECK_ALLOC(type, TYPE_NAME(__reference_type));
    memset(type, 0, sizeof(__reference_type));

    type->name = string(name);

    clear_type_stack();

    type_stack = new __type_stack();
    CHECK_ALLOC(type_stack, TYPE_NAME(__type_stack));
    type_stack->current_type = type;

    state = __schema_parse_state::SPS_IN_TYPE;
}

void com::wookler::reactfs::core::parsers::schema_driver::add_declaration(const string &varname, const string &type,
                                                                          bool is_ref) {
    CHECK_NOT_EMPTY(varname);
    CHECK_NOT_EMPTY(type);

    if (state == __schema_parse_state::SPS_IN_TYPE)
        LOG_DEBUG("[type=%s] Adding declaration. [type=%s][name=%s]", type_stack->current_type->name.c_str(),
                  type.c_str(),
                  varname.c_str());

    if (is_ref) {
        check_reference_type(type);
    }

    __declare *d = (__declare *) malloc(sizeof(__declare));
    CHECK_ALLOC(d, TYPE_NAME(__declare));
    memset(d, 0, sizeof(__declare));

    d->is_reference = is_ref;
    d->variable = new string(varname);
    d->type = new string(type);

    if (state == __schema_parse_state::SPS_IN_TYPE) {
        CHECK_NOT_NULL(type_stack);

        d->constraint = get_type_var_constraint(type);
        d->default_value = get_type_default_value();

        type_stack->declares.push_back(d);
        FREE_PTR(type_stack->declare.current_constraint);
        FREE_PTR(type_stack->declare.default_value);
    } else if (state == __schema_parse_state::SPS_IN_SCHEMA) {
        CHECK_NOT_NULL(schema_stack);

        d->constraint = get_schema_var_constraint(type);
        d->default_value = get_schema_default_value();

        schema_stack->declares.push_back(d);
        FREE_PTR(schema_stack->declare.current_constraint);
        FREE_PTR(schema_stack->declare.default_value);
    }
}

void com::wookler::reactfs::core::parsers::schema_driver::add_array_decl(const string &varname, uint16_t size,
                                                                         const string &type, bool is_ref) {
    CHECK_NOT_EMPTY(varname);
    CHECK_NOT_EMPTY(type);
    PRECONDITION(size > 0);

    if (is_ref) {
        check_reference_type(type);
    }

    __declare *d = (__declare *) malloc(sizeof(__declare));
    CHECK_ALLOC(d, TYPE_NAME(__declare));
    memset(d, 0, sizeof(__declare));

    d->is_reference = false;
    d->variable = new string(varname);
    d->type = new string(TYPE_NAME_ARRAY);
    d->size = size;

    d->inner_types = (__declare *) malloc(sizeof(__declare));
    CHECK_ALLOC(d->inner_types, TYPE_NAME(__declare));
    d->inner_types->type = new string(type);
    d->inner_types->is_reference = is_ref;

    if (state == __schema_parse_state::SPS_IN_TYPE) {
        CHECK_NOT_NULL(type_stack);

        d->constraint = get_type_var_constraint(type);
        d->default_value = get_type_default_value();

        type_stack->declares.push_back(d);
        FREE_PTR(type_stack->declare.current_constraint);
        FREE_PTR(type_stack->declare.default_value);
    } else if (state == __schema_parse_state::SPS_IN_SCHEMA) {
        CHECK_NOT_NULL(schema_stack);

        d->constraint = get_schema_var_constraint(type);
        d->default_value = get_schema_default_value();

        schema_stack->declares.push_back(d);
        FREE_PTR(schema_stack->declare.current_constraint);
        FREE_PTR(schema_stack->declare.default_value);
    }
}

void com::wookler::reactfs::core::parsers::schema_driver::add_list_decl(const string &varname, const string &type,
                                                                        bool is_ref) {
    CHECK_NOT_EMPTY(varname);
    CHECK_NOT_EMPTY(type);

    if (is_ref) {
        check_reference_type(type);
    }

    __declare *d = (__declare *) malloc(sizeof(__declare));
    CHECK_ALLOC(d, TYPE_NAME(__declare));
    memset(d, 0, sizeof(__declare));

    d->is_reference = false;
    d->variable = new string(varname);
    d->type = new string(TYPE_NAME_LIST);

    d->inner_types = (__declare *) malloc(sizeof(__declare));
    CHECK_ALLOC(d->inner_types, TYPE_NAME(__declare));
    d->inner_types->type = new string(type);
    d->inner_types->is_reference = is_ref;

    if (state == __schema_parse_state::SPS_IN_TYPE) {
        CHECK_NOT_NULL(type_stack);

        d->constraint = get_type_var_constraint(type);
        d->default_value = get_type_default_value();

        type_stack->declares.push_back(d);
        FREE_PTR(type_stack->declare.current_constraint);
        FREE_PTR(type_stack->declare.default_value);
    } else if (state == __schema_parse_state::SPS_IN_SCHEMA) {
        CHECK_NOT_NULL(schema_stack);

        d->constraint = get_schema_var_constraint(type);
        d->default_value = get_schema_default_value();

        schema_stack->declares.push_back(d);
        FREE_PTR(schema_stack->declare.current_constraint);
        FREE_PTR(schema_stack->declare.default_value);
    }
}

void com::wookler::reactfs::core::parsers::schema_driver::add_map_decl(const string &varname, const string &ktype,
                                                                       const string &vtype, bool is_ref) {
    CHECK_NOT_EMPTY(varname);
    CHECK_NOT_EMPTY(ktype);
    CHECK_NOT_EMPTY(vtype);

    if (is_ref) {
        check_reference_type(vtype);
    }

    __declare *d = (__declare *) malloc(sizeof(__declare));
    CHECK_ALLOC(d, TYPE_NAME(__declare));
    memset(d, 0, sizeof(__declare));

    d->is_reference = false;
    d->variable = new string(varname);
    d->type = new string(TYPE_NAME_MAP);

    d->inner_types = (__declare *) malloc(sizeof(__declare));
    CHECK_ALLOC(d->inner_types, TYPE_NAME(__declare));
    memset(d->inner_types, 0, sizeof(__declare));

    d->inner_types->type = new string(ktype);
    d->inner_types->is_reference = false;

    __declare *dv = (__declare *) malloc(sizeof(__declare));
    CHECK_ALLOC(dv, TYPE_NAME(__declare));
    memset(dv, 0, sizeof(__declare));

    dv->is_reference = is_ref;
    dv->type = new string(vtype);

    d->inner_types->next = dv;

    if (state == __schema_parse_state::SPS_IN_TYPE) {
        CHECK_NOT_NULL(type_stack);

        d->constraint = get_type_var_constraint(vtype);
        d->default_value = get_type_default_value();

        type_stack->declares.push_back(d);
        FREE_PTR(type_stack->declare.current_constraint);
        FREE_PTR(type_stack->declare.default_value);
    } else if (state == __schema_parse_state::SPS_IN_SCHEMA) {
        CHECK_NOT_NULL(schema_stack);

        d->constraint = get_schema_var_constraint(vtype);
        d->default_value = get_schema_default_value();

        schema_stack->declares.push_back(d);
        FREE_PTR(schema_stack->declare.current_constraint);
        FREE_PTR(schema_stack->declare.default_value);
    }
}

void com::wookler::reactfs::core::parsers::schema_driver::set_constraint(bool is_not, const string &constraint,
                                                                         const string &values) {
    CHECK_NOT_EMPTY(constraint);
    CHECK_NOT_EMPTY(values);

    LOG_DEBUG("Constraint called. [type=%s][values=%s]", constraint.c_str(), values.c_str());
    __constraint_def *current_constraint = (__constraint_def *) malloc(sizeof(__constraint_def));
    CHECK_ALLOC(current_constraint, TYPE_NAME(__constraint_def));
    memset(current_constraint, 0, sizeof(__constraint_def));

    current_constraint->is_not = is_not;
    current_constraint->constraint = new string(constraint);
    current_constraint->values = new string(values);

    if (state == __schema_parse_state::SPS_IN_TYPE) {
        type_stack->declare.current_constraint = current_constraint;
    } else if (state == __schema_parse_state::SPS_IN_SCHEMA) {
        schema_stack->declare.current_constraint = current_constraint;
    }
}

void com::wookler::reactfs::core::parsers::schema_driver::set_default_value(const string &value) {
    CHECK_NOT_EMPTY(value);

    LOG_DEBUG("Setting default value. [value=%s]", value.c_str());

    string *def_value = new string(value);
    CHECK_ALLOC(def_value, TYPE_NAME(string));

    if (state == __schema_parse_state::SPS_IN_TYPE) {
        type_stack->declare.default_value = def_value;
    } else if (state == __schema_parse_state::SPS_IN_SCHEMA) {
        schema_stack->declare.default_value = def_value;
    }
}

void com::wookler::reactfs::core::parsers::schema_driver::set_primary_key(const string &keys) {
    CHECK_NOT_EMPTY(keys);
    CHECK_NOT_NULL(schema_stack);
    PRECONDITION(state == __schema_parse_state::SPS_IN_SCHEMA);

    schema_stack->pk_columns = new string(keys);
}

void com::wookler::reactfs::core::parsers::schema_driver::finish_def() {
    if (state == __schema_parse_state::SPS_IN_TYPE) {
        this->finish_type();
    } else if (state == __schema_parse_state::SPS_IN_SCHEMA) {
        this->finish_schema();
    } else {
        throw TYPE_PARSER_ERROR("Invalid state at finish. [state=%d]", state);
    }
}

void com::wookler::reactfs::core::parsers::schema_driver::finish_type() {
    CHECK_NOT_NULL(type_stack);
    CHECK_NOT_NULL(type_stack->current_type);
    CHECK_NOT_EMPTY(type_stack->declares);

    vector<__declare *>::iterator iter;
    __declare *d = nullptr;
    for (iter = type_stack->declares.begin(); iter != type_stack->declares.end(); iter++) {
        if (IS_NULL(d)) {
            d = *iter;
            type_stack->current_type->members = d;
        } else {
            d->next = *iter;
            d = d->next;
        }
    }
    __reference_type *type = type_stack->current_type;

    check_new_type(type->name);
    types.insert({type->name, type});
    LOG_DEBUG("[type=%s] Finished type declaration...", type->name.c_str());

    clear_type_stack();
}

void com::wookler::reactfs::core::parsers::schema_driver::finish_schema() {
    CHECK_NOT_NULL(schema_stack);
    CHECK_NOT_NULL(schema_stack->current_schema);
    CHECK_NOT_EMPTY(schema_stack->declares);

    vector<__declare *>::iterator iter;
    __declare *d = nullptr;
    for (iter = schema_stack->declares.begin(); iter != schema_stack->declares.end(); iter++) {
        if (IS_NULL(d)) {
            d = *iter;
            schema_stack->current_schema->members = d;
        } else {
            d->next = *iter;
            d = d->next;
        }
    }

    if (NOT_EMPTY_P(schema_stack->pk_columns)) {
        vector<string> columns;
        string_utils::split(*(schema_stack->pk_columns), ',', &columns);
        POSTCONDITION(!IS_EMPTY(columns));

        __key_column *kc = nullptr;
        uint16_t index = 0;
        for (string c : columns) {
            __key_column *nkc = get_key_column(c);
            nkc->index = index++;
            CHECK_NOT_NULL(nkc);
            if (IS_NULL(kc)) {
                kc = nkc;
                schema_stack->current_schema->pk_columns = kc;
            } else {
                kc->next = nkc;
                kc = nkc;
            }
        }
    }
    LOG_DEBUG("Finished schema declaration. [name=%s]", schema_stack->current_schema->name.c_str());
}

void com::wookler::reactfs::core::parsers::schema_driver::parse(const char *const filename) {
    assert(filename != nullptr);
    std::ifstream in_file(filename);
    if (!in_file.good()) {
        exit(EXIT_FAILURE);
    }
    parse_helper(in_file);
    return;
}

void com::wookler::reactfs::core::parsers::schema_driver::parse(std::istream &stream) {
    if (!stream.good() && stream.eof()) {
        return;
    }
    //else
    parse_helper(stream);
    return;
}

void com::wookler::reactfs::core::parsers::schema_driver::parse_helper(std::istream &stream) {
    CHECK_AND_FREE(scanner);
    scanner = new schema_scanner(&stream);
    CHECK_ALLOC(scanner, TYPE_NAME(schema_scanner));

    CHECK_AND_FREE (parser);
    parser = new schema_parser((*scanner) /* scanner */,
                               (*this) /* driver */ );
    CHECK_ALLOC(parser, TYPE_NAME(schema_parser));

    const int accept(0);
    if (parser->parse() != accept) {
        std::cerr << "Parse failed!!\n";
    }
    return;
}