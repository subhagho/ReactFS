//
// Created by Subhabrata Ghosh on 07/12/16.
//
#include <cassert>

#include "schema_driver.h"

com::wookler::reactfs::core::parsers::schema_driver::~schema_driver() {
    CHECK_AND_FREE(scanner);
    CHECK_AND_FREE(parser);
}

void com::wookler::reactfs::core::parsers::schema_driver::add_type(const string &name) {
    cout << "TYPE : " << name << " type count=" << declares.size() << "\n";
}

void com::wookler::reactfs::core::parsers::schema_driver::add_declaration(const string &varname, const string &type,
                                                                          bool is_ref) {
    cout << "DECLARATION : " << varname << " [" << type << "]\n";
    __declare *d = (__declare *) malloc(sizeof(__declare));
    CHECK_ALLOC(d, TYPE_NAME(__declare));
    d->is_reference = is_ref;
    d->varname = string(varname);
    d->type = string(type);

    declares.push_back(d);
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
    try {
        scanner = new schema_scanner(&stream);
    }
    catch (std::bad_alloc &ba) {
        std::cerr << "Failed to allocate scanner: (" <<
                  ba.what() << "), exiting!!\n";
        exit(EXIT_FAILURE);
    }

    CHECK_AND_FREE (parser);
    try {
        parser = new schema_parser((*scanner) /* scanner */,
                                   (*this) /* driver */ );
    }
    catch (std::bad_alloc &ba) {
        std::cerr << "Failed to allocate parser: (" <<
                  ba.what() << "), exiting!!\n";
        exit(EXIT_FAILURE);
    }
    const int accept(0);
    if (parser->parse() != accept) {
        std::cerr << "Parse failed!!\n";
    }
    return;
}