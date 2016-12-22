//
// Created by Subhabrata Ghosh on 07/12/16.
//
#include <cassert>

#include "common/includes/log_utils.h"
#include "schema_driver.h"

com::wookler::reactfs::core::parsers::schema_driver::~schema_driver() {
    CHECK_AND_FREE(scanner);
    CHECK_AND_FREE(parser);

    free_schema();

    for (auto iter = types.begin(); iter != types.end(); iter++) {
        free_type(iter->second);
    }
    types.clear();

    for (auto iter = indexes.begin(); iter != indexes.end(); iter++) {
        free_index_def(iter->second);
    }
    indexes.clear();
    CHECK_AND_FREE(name_space);
}

void com::wookler::reactfs::core::parsers::schema_driver::create_schema(const string &name) {
    if (NOT_NULL(type_stack) && NOT_NULL(type_stack->current_type)) {
        throw TYPE_PARSER_ERROR("Previous type definition not terminated correctly.");
    }
    if (NOT_NULL(type_stack)) {
        CHECK_AND_FREE(type_stack);
    }
    if (NOT_NULL(schema_stack)) {
        throw TYPE_PARSER_ERROR("Expected schema stack to be null. Multiple schema definitions not allowed.");
    }

    schema_stack = new __schema_stack();
    CHECK_ALLOC(schema_stack, TYPE_NAME(__schema_stack));

    schema_stack->current_schema = (__schema_def *) malloc(sizeof(__schema_def));
    CHECK_ALLOC(schema_stack->current_schema, TYPE_NAME(__schema_def));
    memset(schema_stack->current_schema, 0, sizeof(__schema_def));

    schema_stack->current_schema->name = new string(name);
    CHECK_ALLOC(schema_stack->current_schema->name, TYPE_NAME(string));

    state = __schema_parse_state::SPS_IN_SCHEMA;

    LOG_DEBUG("Added schema definition. [name=%s]", schema_stack->current_schema->name->c_str());
}

void com::wookler::reactfs::core::parsers::schema_driver::add_type(const string &name) {
    LOG_DEBUG("Adding new datatype. [name=%s]", name.c_str());

    if (NOT_NULL(type_stack)) {
        PRECONDITION(IS_NULL(type_stack->current_type));
        PRECONDITION(IS_NULL(type_stack->declare.current_constraint));
        PRECONDITION(IS_NULL(type_stack->declare.default_value));
    }

    __reference_type *type = (__reference_type *) malloc(sizeof(__reference_type));
    CHECK_ALLOC(type, TYPE_NAME(__reference_type));
    memset(type, 0, sizeof(__reference_type));

    type->name = new string(name);
    CHECK_ALLOC(type->name, TYPE_NAME(string));

    clear_type_stack();

    if (IS_NULL(type_stack)) {
        type_stack = new __type_stack();
        CHECK_ALLOC(type_stack, TYPE_NAME(__type_stack));
    }
    type_stack->current_type = type;

    state = __schema_parse_state::SPS_IN_TYPE;
}

void com::wookler::reactfs::core::parsers::schema_driver::add_declaration(const string &varname, const string &type,
                                                                          bool is_ref, int nullable) {
    CHECK_NOT_EMPTY(varname);
    CHECK_NOT_EMPTY(type);

    if (state == __schema_parse_state::SPS_IN_TYPE)
        LOG_DEBUG("[type=%s] Adding declaration. [type=%s][name=%s]", type_stack->current_type->name->c_str(),
                  type.c_str(),
                  varname.c_str());

    if (is_ref) {
        check_reference_type(type);
    }

    __declare *d = (__declare *) malloc(sizeof(__declare));
    CHECK_ALLOC(d, TYPE_NAME(__declare));
    memset(d, 0, sizeof(__declare));

    d->is_reference = is_ref;
    d->is_nullable = (nullable > 0 ? true : false);
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

void com::wookler::reactfs::core::parsers::schema_driver::add_list_decl(const string &varname, const string &type,
                                                                        bool is_ref, int nullable) {
    CHECK_NOT_EMPTY(varname);
    CHECK_NOT_EMPTY(type);

    if (is_ref) {
        check_reference_type(type);
    }

    __declare *d = (__declare *) malloc(sizeof(__declare));
    CHECK_ALLOC(d, TYPE_NAME(__declare));
    memset(d, 0, sizeof(__declare));

    d->is_reference = false;
    d->is_nullable = (nullable > 0 ? true : false);
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
                                                                       const string &vtype, bool is_ref,
                                                                       int nullable) {
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
    d->is_nullable = (nullable > 0 ? true : false);
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

void com::wookler::reactfs::core::parsers::schema_driver::set_nullable() {

    LOG_DEBUG("Setting nullable. ");

    if (state == __schema_parse_state::SPS_IN_TYPE) {
        type_stack->declare.nullable = true;
    } else if (state == __schema_parse_state::SPS_IN_SCHEMA) {
        schema_stack->declare.nullable = true;
    }
}

void com::wookler::reactfs::core::parsers::schema_driver::set_namespace(const string &names) {
    CHECK_NOT_EMPTY(names);
    this->name_space = new string(names);
}

void com::wookler::reactfs::core::parsers::schema_driver::set_primary_key(const string &keys) {
    CHECK_NOT_EMPTY(keys);
    CHECK_NOT_NULL(schema_stack);
    PRECONDITION(state == __schema_parse_state::SPS_IN_SCHEMA);

    schema_stack->pk_columns = new string(keys);
}

void com::wookler::reactfs::core::parsers::schema_driver::create_index(const string &name, const string &schema) {
    CHECK_NOT_EMPTY(name);
    CHECK_NOT_EMPTY(schema);

    if (NOT_NULL(index_stack)) {
        PRECONDITION(IS_NULL(index_stack->current_index));
        PRECONDITION(IS_NULL(index_stack->fields));
        PRECONDITION(IS_NULL(index_stack->index_type));
    }

    PRECONDITION(NOT_NULL(schema_stack));
    PRECONDITION(*(schema_stack->current_schema->name) == schema);

    __index_def *ci = (__index_def *) malloc(sizeof(__index_def));
    CHECK_ALLOC(ci, TYPE_NAME(__index_def));
    ci->name = new string(name);
    CHECK_ALLOC(ci->name, TYPE_NAME(string));
    ci->schema_name = new string(schema);
    CHECK_ALLOC(ci->schema_name, TYPE_NAME(string));

    if (IS_NULL(index_stack)) {
        index_stack = new __index_stack();
        CHECK_ALLOC(index_stack, TYPE_NAME(__index_stack));
    }
    index_stack->current_index = ci;

    state = __schema_parse_state::SPS_IN_INDEX;

    LOG_DEBUG("Added schema definition. [name=%s][schema=%s]", index_stack->current_index->name->c_str(),
              index_stack->current_index->schema_name->c_str());
}

void com::wookler::reactfs::core::parsers::schema_driver::set_index_fields(const string &fields) {
    CHECK_NOT_EMPTY(fields);
    CHECK_NOT_NULL(index_stack);
    CHECK_NOT_NULL(index_stack->current_index);
    index_stack->fields = new string(fields);
}

void com::wookler::reactfs::core::parsers::schema_driver::set_index_type(const string &type) {
    CHECK_NOT_EMPTY(type);
    CHECK_NOT_NULL(index_stack);
    CHECK_NOT_NULL(index_stack->current_index);
    index_stack->index_type = new string(type);
}

void com::wookler::reactfs::core::parsers::schema_driver::finish_index() {
    CHECK_NOT_NULL(index_stack);
    CHECK_NOT_NULL(index_stack->current_index);
    CHECK_NOT_EMPTY_P(index_stack->fields);

    check_new_index(*(index_stack->current_index->name));

    vector<string> columns;
    string_utils::split(*(index_stack->fields), ',', &columns);
    POSTCONDITION(!IS_EMPTY(columns));

    __key_column *kc = nullptr;
    uint16_t index = 0;
    for (string c : columns) {
        __key_column *nkc = get_key_column(c);
        nkc->index = index++;
        CHECK_NOT_NULL(nkc);
        if (IS_NULL(kc)) {
            kc = nkc;
            index_stack->current_index->columns = kc;
        } else {
            kc->next = nkc;
            kc = nkc;
        }
        index_stack->current_index->field_count++;
    }
    if (!IS_EMPTY_P(index_stack->index_type)) {
        index_stack->current_index->type = index_type_utils::get_index_type(*(index_stack->index_type));
    }
    __index_def *def = index_stack->current_index;
    clear_index_stack();

    indexes.insert({*(def->name), def});
}

void com::wookler::reactfs::core::parsers::schema_driver::finish_def() {
    if (state == __schema_parse_state::SPS_IN_TYPE) {
        this->finish_type();
    } else if (state == __schema_parse_state::SPS_IN_SCHEMA) {
        this->finish_schema();
    } else if (state == __schema_parse_state::SPS_IN_INDEX) {
        this->finish_index();
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
        type_stack->current_type->field_count++;
    }
    __reference_type *type = type_stack->current_type;

    check_new_type(*(type->name));
    types.insert({*(type->name), type});
    LOG_DEBUG("[type=%s] Finished type declaration...", type->name->c_str());

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
        schema_stack->current_schema->field_count++;
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
    LOG_DEBUG("Finished schema declaration. [name=%s]", schema_stack->current_schema->name->c_str());
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

__complex_type *com::wookler::reactfs::core::parsers::schema_driver::translate() {
    CHECK_NOT_NULL(schema_stack);
    CHECK_NOT_NULL(schema_stack->current_schema);

    __schema_def *schema_def = schema_stack->current_schema;
    translator tr(schema_def, &types, &indexes);
    return tr.translate();
}
