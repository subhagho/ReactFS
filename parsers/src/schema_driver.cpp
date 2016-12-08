//
// Created by Subhabrata Ghosh on 07/12/16.
//
#include <cassert>

#include "common/includes/log_utils.h"
#include "schema_driver.h"

com::wookler::reactfs::core::parsers::schema_driver::~schema_driver() {
    CHECK_AND_FREE(scanner);
    CHECK_AND_FREE(parser);
}

void com::wookler::reactfs::core::parsers::schema_driver::add_type(const string &name) {
    LOG_DEBUG("Adding new datatype. [name=%s]", name.c_str());

    __reference_type *type = (__reference_type *) malloc(sizeof(__reference_type));
    CHECK_ALLOC(type, TYPE_NAME(__reference_type));
    memset(type, 0, sizeof(__reference_type));

    type->name = string(name);

    state = __schema_parse_state::SPS_IN_TYPE;
    current_type = type;
}

void com::wookler::reactfs::core::parsers::schema_driver::add_declaration(const string &varname, const string &type,
                                                                          bool is_ref) {
    CHECK_NOT_EMPTY(varname);
    CHECK_NOT_EMPTY(type);

    if (state == __schema_parse_state::SPS_IN_TYPE)
        LOG_DEBUG("[type=%s] Adding declaration. [type=%s][name=%s]", current_type->name.c_str(), type.c_str(),
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

    if (NOT_NULL(current_constraint)) {
        current_constraint->type = new string(type);
        d->constraint = current_constraint;
        current_constraint = nullptr;
    }
    declares.push_back(d);
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

    if (NOT_NULL(current_constraint)) {
        current_constraint->type = new string(type);
        d->constraint = current_constraint;
        current_constraint = nullptr;
    }
    declares.push_back(d);
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

    if (NOT_NULL(current_constraint)) {
        current_constraint->type = new string(type);
        d->constraint = current_constraint;
        current_constraint = nullptr;
    }
    declares.push_back(d);
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

    if (NOT_NULL(current_constraint)) {
        current_constraint->type = new string(vtype);
        d->constraint = current_constraint;
        current_constraint = nullptr;
    }
    declares.push_back(d);
}

void com::wookler::reactfs::core::parsers::schema_driver::set_constraint(bool is_not, const string &constraint,
                                                                         const string &values) {
    CHECK_NOT_EMPTY(constraint);
    CHECK_NOT_EMPTY(values);

    LOG_DEBUG("Constraint called. [type=%s][values=%s]", constraint.c_str(), values.c_str());
    current_constraint = (__constraint_def *) malloc(sizeof(__constraint_def));
    CHECK_ALLOC(current_constraint, TYPE_NAME(__constraint_def));
    memset(current_constraint, 0, sizeof(__constraint_def));

    current_constraint->is_not = is_not;
    current_constraint->type = new string(constraint);
    current_constraint->values = new string(values);
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
    CHECK_NOT_NULL(current_type);
    CHECK_NOT_EMPTY(declares);
    vector<__declare *>::iterator iter;
    __declare *d = nullptr;
    for (iter = declares.begin(); iter != declares.end(); iter++) {
        if (IS_NULL(d)) {
            d = *iter;
            current_type->members = d;
        } else {
            d->next = *iter;
            d = d->next;
        }
    }
    declares.clear();

    check_new_type(current_type->name);
    types.insert({current_type->name, current_type});
    LOG_DEBUG("[type=%s] Finished type declaration...", current_type->name.c_str());

    current_type = nullptr;
}

void com::wookler::reactfs::core::parsers::schema_driver::finish_schema() {

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