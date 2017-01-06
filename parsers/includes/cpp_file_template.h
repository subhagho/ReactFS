//
// Created by Subhabrata Ghosh on 25/12/16.
//

#ifndef REACTFS_CPP_FILE_TEMPLATE_H
#define REACTFS_CPP_FILE_TEMPLATE_H

#include "core/includes/core.h"
#include "code_generator.h"
#include "cpp_template_header.h"
#include "cpp_template_defs.h"

#define CPPT_CLASS_PARENT_TYPE " : public com::wookler::reactfs::core::types::__base_type"
#define CPPT_CLASS_PARENT_TYPE_M " : public com::wookler::reactfs::core::types::__mutable_base_type"
#define CPPT_MUTABLE_CLASS_PREFIX "mutable_"

#define READ_PARSED_ROWS(var, templ, token) \
do { \
    stringstream stream; \
    vector<string> *ft = templ.find_token(token); \
    CHECK_NOT_NULL(ft); \
    for (string str : *ft) { \
        stream << str << "\n"; \
    } \
    var = string(stream.str()); \
} while(0);

#define REPLACE_TOKEN(var, t, v) var = string_utils::set_token(t, v, var)

#define READ_FROM_VECT(var, v) do {\
    stringstream stream; \
    for(string str : v) { \
        stream << str << "\n"; \
    } \
    var = string(stream.str()); \
} while(0);

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace parsers {
                    typedef struct __cpp_class_template__ {
                        string classname;
                        string class_str;
                        stringstream declare;
                        stringstream pr_methods;
                        stringstream pu_methods;
                        vector<string> free_calls;
                    } __cpp_class_template;

                    class cpp_file_template {
                    private:
                        cpp_template_header template_header;
                        __cpp_class_template ro_class;
                        __cpp_class_template m_class;

                        string name_space;
                        string file_str;
                        unordered_map<string, string> headers;
                        vector<string> m_call_serde;
                        vector<string> variables;
                        vector<string> copy_constr_calls;
                        vector<string> copy_constr_calls_ptr;

                        string get_file_header(const string &classname, const string &schema_name,
                                               __version_header &version) {
                            CHECK_NOT_EMPTY(classname);

                            string ss;
                            READ_PARSED_ROWS(ss, template_header, CPPT_TOKEN_FILE_COMMENT);

                            string tk_schema = CPPT_TOKEN_DEF_SCHEMA_NAME;
                            string tk_version = CPPT_TOKEN_DEF_VERSION;
                            string tk_username = CPPT_TOKEN_DEF_USERNAME;
                            string tk_date = CPPT_TOKEN_DEF_DATE;

                            uint64_t date = time_utils::now();
                            string date_s = time_utils::get_time_string(date);
                            string uname = common_utils::get_username();
                            CHECK_NOT_EMPTY(uname);
                            string version_s = version_utils::get_version_string(version);
                            CHECK_NOT_EMPTY(version_s)

                            REPLACE_TOKEN(ss, tk_schema, schema_name);
                            REPLACE_TOKEN(ss, tk_version, version_s);
                            REPLACE_TOKEN(ss, tk_username, uname);
                            REPLACE_TOKEN(ss, tk_date, date_s);

                            return ss;
                        }

                        void
                        init_file_template(const string &classname, const string &schema_name,
                                           __version_header &version) {
                            CHECK_NOT_EMPTY(classname);

                            string vs;
                            READ_PARSED_ROWS(vs, template_header, CPPT_TOKEN_FILE_DEF);

                            string header = get_file_header(classname, schema_name, version);
                            CHECK_NOT_EMPTY(header);

                            string guard_name = string_utils::toupper(classname);
                            guard_name = common_utils::format("%s_H", guard_name.c_str());

                            string tk_guard = CPPT_TOKEN_DEF_GUARD_NAME;
                            CHECK_NOT_EMPTY(tk_guard);
                            string tk_header = CPPT_TOKEN_DEF_HEADER;
                            CHECK_NOT_EMPTY(tk_header);

                            REPLACE_TOKEN(vs, tk_guard, guard_name);
                            REPLACE_TOKEN(vs, tk_header, header);

                            this->file_str = string(vs);
                            this->ro_class.classname = string(classname);
                            this->m_class.classname = common_utils::format("%s%s", CPPT_MUTABLE_CLASS_PREFIX,
                                                                           classname.c_str());
                        }

                        void init_ro_class_template() {
                            string vs;
                            READ_PARSED_ROWS(vs, template_header, CPPT_TOKEN_CLASS_DEF);

                            string tk_class = CPPT_TOKEN_DEF_NAME;
                            string tk_parent = CPPT_TOKEN_DEF_PARENT;

                            REPLACE_TOKEN(vs, tk_class, this->ro_class.classname);
                            REPLACE_TOKEN(vs, tk_parent, CPPT_CLASS_PARENT_TYPE);

                            this->ro_class.class_str = string(vs);
                        }

                        void init_m_class_template() {
                            string vs;
                            READ_PARSED_ROWS(vs, template_header, CPPT_TOKEN_CLASS_DEF);

                            string tk_class = CPPT_TOKEN_DEF_NAME;
                            string tk_parent = CPPT_TOKEN_DEF_PARENT;

                            REPLACE_TOKEN(vs, tk_class, this->m_class.classname);
                            REPLACE_TOKEN(vs, tk_parent, CPPT_CLASS_PARENT_TYPE_M);

                            this->m_class.class_str = string(vs);
                        }

                        string get_namespace(string &ns) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, CPPT_TOKEN_NAMESPACE);

                            string tk = CPPT_TOKEN_DEF_NAME;
                            REPLACE_TOKEN(str, tk, ns);

                            return str;
                        }

                        string get_namespace() {
                            CHECK_NOT_EMPTY(this->name_space);
                            vector<string> parts;
                            string_utils::split(this->name_space, '.', &parts);
                            CHECK_NOT_EMPTY(parts);

                            string prev;
                            string tk = CPPT_TOKEN_DEF_NESTED;
                            for (uint8_t ii = parts.size() - 1; ii >= 0 && ii < parts.size(); ii--) {
                                string ns = parts[ii];
                                CHECK_NOT_EMPTY(ns);
                                string n_str = get_namespace(ns);
                                CHECK_NOT_EMPTY(n_str);
                                if (!IS_EMPTY(prev)) {
                                    REPLACE_TOKEN(n_str, tk, prev);
                                    prev = n_str;
                                } else {
                                    prev = n_str;
                                }
                            }
                            return prev;
                        }

                        void generate_deserializer() {
                            string fstr;
                            READ_PARSED_ROWS(fstr, template_header, CPPT_TOKEN_FUNC_DESERIALIZE);

                            string callstr;
                            READ_PARSED_ROWS(callstr, template_header, CPPT_TOKEN_CALL_DESERIALIZE_VALUE);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            stringstream body;
                            for (string ss : m_call_serde) {
                                string c = string(callstr);
                                REPLACE_TOKEN(c, tk_name, ss);
                                body << c << "\n";
                            }
                            string bstr(body.str());

                            string tk_read = CPPT_TOKEN_DEF_READ_MAP_CALLS;
                            REPLACE_TOKEN(fstr, tk_read, bstr);

                            TRACE("DESERIALIZER [%s]", fstr.c_str());

                            add_public_method(ro_class, fstr);
                        }

                        void generate_serializer() {
                            string fstr;
                            READ_PARSED_ROWS(fstr, template_header, CPPT_TOKEN_FUNC_SERIALIZE);

                            string callstr;
                            READ_PARSED_ROWS(callstr, template_header, CPPT_TOKEN_CALL_SETTER_TO_RECORD);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            stringstream body;
                            for (string ss : m_call_serde) {
                                string c = string(callstr);
                                REPLACE_TOKEN(c, tk_name, ss);
                                body << c << "\n";
                            }
                            string bstr(body.str());

                            string tk_read = CPPT_TOKEN_DEF_SET_MAP_CALLS;
                            REPLACE_TOKEN(fstr, tk_read, bstr);

                            TRACE("SERIALIZER [%s]", fstr.c_str());

                            add_public_method(m_class, fstr);
                        }

                        void generate_empty_constr() {
                            stringstream istream;
                            for (string var : variables) {
                                string ss = common_utils::format("this->%s = nullptr;", var.c_str());
                                istream << ss << "\n";
                            }
                            string inits = string(istream.str());

                            string fstr;
                            READ_PARSED_ROWS(fstr, template_header, CPPT_TOKEN_FUNC_CONSTRUCTOR_WRITABLE);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_inits = CPPT_TOKEN_DEF_VARIABLE_INITS;

                            REPLACE_TOKEN(fstr, tk_name, m_class.classname);
                            REPLACE_TOKEN(fstr, tk_inits, inits);

                            add_public_method(m_class, fstr);
                        }

                        void generate_serde_constr() {
                            stringstream istream;
                            for (string var : variables) {
                                string ss = common_utils::format("this->%s = nullptr;", var.c_str());
                                istream << ss << "\n";
                            }
                            string inits = string(istream.str());

                            string fstr;
                            READ_PARSED_ROWS(fstr, template_header, CPPT_TOKEN_FUNC_CONSTRUCTOR_READABLE);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_inits = CPPT_TOKEN_DEF_VARIABLE_INITS;

                            REPLACE_TOKEN(fstr, tk_name, ro_class.classname);
                            REPLACE_TOKEN(fstr, tk_inits, inits);

                            add_public_method(ro_class, fstr);
                        }

                        void generate_copy_constr() {
                            stringstream istream;
                            for (string var : variables) {
                                string ss = common_utils::format("this->%s = nullptr;", var.c_str());
                                istream << ss << "\n";
                            }
                            string inits = string(istream.str());

                            string copies;
                            READ_FROM_VECT(copies, copy_constr_calls);

                            string fstr;
                            READ_PARSED_ROWS(fstr, template_header, CPPT_TOKEN_FUNC_CONSTRUCTOR_COPY);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_inits = CPPT_TOKEN_DEF_VARIABLE_INITS;
                            string tk_copies = CPPT_TOKEN_DEF_VARIABLE_COPY;
                            string tk_s_name = CPPT_TOKEN_DEF_NAME_READ;

                            REPLACE_TOKEN(fstr, tk_name, m_class.classname);
                            REPLACE_TOKEN(fstr, tk_s_name, ro_class.classname);
                            REPLACE_TOKEN(fstr, tk_inits, inits);
                            REPLACE_TOKEN(fstr, tk_copies, copies);

                            add_public_method(m_class, fstr);
                        }

                        void generate_copy_constr_ptr() {
                            stringstream istream;
                            for (string var : variables) {
                                string ss = common_utils::format("this->%s = nullptr;", var.c_str());
                                istream << ss << "\n";
                            }
                            string inits = string(istream.str());

                            string copies;
                            READ_FROM_VECT(copies, copy_constr_calls_ptr);

                            string fstr;
                            READ_PARSED_ROWS(fstr, template_header, CPPT_TOKEN_FUNC_CONSTRUCTOR_COPY_PTR);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_inits = CPPT_TOKEN_DEF_VARIABLE_INITS;
                            string tk_copies = CPPT_TOKEN_DEF_VARIABLE_COPY;
                            string tk_s_name = CPPT_TOKEN_DEF_NAME_READ;

                            REPLACE_TOKEN(fstr, tk_name, m_class.classname);
                            REPLACE_TOKEN(fstr, tk_s_name, ro_class.classname);
                            REPLACE_TOKEN(fstr, tk_inits, inits);
                            REPLACE_TOKEN(fstr, tk_copies, copies);

                            add_public_method(m_class, fstr);
                        }

                        void generate_destr(__cpp_class_template &templ) {
                            string inits;
                            READ_FROM_VECT(inits, templ.free_calls);

                            string fstr;
                            READ_PARSED_ROWS(fstr, template_header, CPPT_TOKEN_FUNC_DESTRUCTOR);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_inits = CPPT_TOKEN_DEF_VARIABLE_FREES;

                            REPLACE_TOKEN(fstr, tk_name, templ.classname);
                            REPLACE_TOKEN(fstr, tk_inits, inits);

                            add_public_method(templ, fstr);
                        }

                        string get_ro_class_def() {
                            generate_serde_constr();
                            generate_destr(ro_class);
                            generate_deserializer();

                            string declares = ro_class.declare.str();
                            string pr_methods = ro_class.pr_methods.str();
                            string pu_methods = ro_class.pu_methods.str();

                            if (!IS_EMPTY(declares)) {
                                string tk_declare = CPPT_TOKEN_DEF_DECLARATIONS;
                                CHECK_NOT_EMPTY(tk_declare);

                                ro_class.class_str = string_utils::set_token(tk_declare, declares, ro_class.class_str);
                            }

                            string tk_prm = CPPT_TOKEN_DEF_PRIVATE_FUNCTIONS;
                            CHECK_NOT_EMPTY(tk_prm);

                            if (!IS_EMPTY(pr_methods)) {
                                ro_class.class_str = string_utils::set_token(tk_prm, pr_methods, ro_class.class_str);
                            } else {
                                ro_class.class_str = string_utils::set_token(tk_prm, "", ro_class.class_str);
                            }

                            if (!IS_EMPTY(pu_methods)) {
                                string tk_pum = CPPT_TOKEN_DEF_PUBLIC_FUNCTIONS;
                                CHECK_NOT_EMPTY(tk_pum);

                                ro_class.class_str = string_utils::set_token(tk_pum, pu_methods, ro_class.class_str);
                            }
                            return ro_class.class_str;
                        }

                        string get_m_class_def() {
                            generate_empty_constr();
                            generate_copy_constr();
                            generate_copy_constr_ptr();
                            generate_destr(m_class);
                            generate_serializer();

                            string declares = m_class.declare.str();
                            string pr_methods = m_class.pr_methods.str();
                            string pu_methods = m_class.pu_methods.str();

                            if (!IS_EMPTY(declares)) {
                                string tk_declare = CPPT_TOKEN_DEF_DECLARATIONS;
                                CHECK_NOT_EMPTY(tk_declare);

                                m_class.class_str = string_utils::set_token(tk_declare, declares, m_class.class_str);
                            }

                            string tk_prm = CPPT_TOKEN_DEF_PRIVATE_FUNCTIONS;
                            CHECK_NOT_EMPTY(tk_prm);

                            if (!IS_EMPTY(pr_methods)) {
                                m_class.class_str = string_utils::set_token(tk_prm, pr_methods, m_class.class_str);
                            } else {
                                m_class.class_str = string_utils::set_token(tk_prm, "", m_class.class_str);
                            }

                            if (!IS_EMPTY(pu_methods)) {
                                string tk_pum = CPPT_TOKEN_DEF_PUBLIC_FUNCTIONS;
                                CHECK_NOT_EMPTY(tk_pum);

                                m_class.class_str = string_utils::set_token(tk_pum, pu_methods, m_class.class_str);
                            }
                            return m_class.class_str;
                        }

                        string get_class_def() {
                            string ro_str = get_ro_class_def();
                            string m_str = get_m_class_def();

                            string class_def = common_utils::format("%s\n\n%s", ro_str.c_str(), m_str.c_str());
                            if (!IS_EMPTY(this->name_space)) {
                                string nstr = get_namespace();
                                CHECK_NOT_EMPTY(nstr);

                                string tk = CPPT_TOKEN_DEF_NESTED;
                                CHECK_NOT_EMPTY(tk);
                                nstr = string_utils::set_token(tk, class_def, nstr);
                                return nstr;
                            }
                            return class_def;
                        }

                    public:
                        cpp_file_template(const string &classname, const string &schema_name, const string &name_space,
                                          __version_header &version) {
                            this->init_file_template(classname, schema_name, version);
                            this->init_ro_class_template();
                            this->init_m_class_template();
                            this->name_space = string(name_space);
                        }

                        string get_class_name() {
                            return this->ro_class.classname;
                        }

                        void add_declare(const string &declare, __cpp_class_template &templ) {
                            CHECK_NOT_EMPTY(declare);
                            templ.declare << declare << "\n";
                        }

                        void add_private_method(__cpp_class_template &templ, const string &method) {
                            CHECK_NOT_EMPTY(method);
                            templ.pr_methods << method << "\n";
                        }

                        void add_public_method(__cpp_class_template &templ, const string &method) {
                            CHECK_NOT_EMPTY(method);
                            templ.pu_methods << method << "\n";
                        }

                        void add_header(const string &header) {
                            this->headers.insert({header, header});
                        }

                        void generate_list_serde(__list_type *type, string token, bool mutate) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, token);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;
                            string prefix = (mutate ? CPPT_MUTABLE_CLASS_PREFIX : common_consts::EMPTY_STRING);

                            string tn = type->get_type_name(prefix);
                            tn = common_utils::get_normalized_name(tn);
                            string dt = type->get_inner_type()->get_type_name(prefix);
                            string dtp = type->get_inner_type()->get_type_ptr(prefix);

                            REPLACE_TOKEN(str, tk_name, tn);
                            REPLACE_TOKEN(str, tk_type, dt);
                            REPLACE_TOKEN(str, tk_type_ptr, dtp);

                            TRACE("LIST SERIALIZER [%s]", str.c_str());

                            if (mutate)
                                add_private_method(m_class, str);
                            else
                                add_private_method(ro_class, str);
                        }

                        void generate_list_add(const __native_type *type, string token) {
                            const __list_type *lt = dynamic_cast<const __list_type *>(type);
                            CHECK_CAST(lt, TYPE_NAME(__native_type), TYPE_NAME(__list_type));

                            string str;
                            READ_PARSED_ROWS(str, template_header, token);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;

                            string tn = lt->get_name();
                            CHECK_NOT_EMPTY(tn);
                            string dt = lt->get_inner_type()->get_type_name(CPPT_MUTABLE_CLASS_PREFIX);
                            CHECK_NOT_EMPTY(dt);
                            string dtp = lt->get_inner_type()->get_type_ptr(CPPT_MUTABLE_CLASS_PREFIX);
                            CHECK_NOT_EMPTY(dtp);

                            REPLACE_TOKEN(str, tk_name, tn);
                            REPLACE_TOKEN(str, tk_type, dt);
                            REPLACE_TOKEN(str, tk_type_ptr, dtp);

                            TRACE("SETTER [%s]", str.c_str());

                            add_public_method(m_class, str);
                        }

                        void generate_map_serde(__map_type *type, string token, bool mutate) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, token);
                            string prefix = (mutate ? CPPT_MUTABLE_CLASS_PREFIX : common_consts::EMPTY_STRING);

                            string k_type = type->get_key_type()->get_type_name(prefix);
                            if (type->get_key_type()->get_datatype() == __type_def_enum::TYPE_STRING) {
                                k_type = "std::string";
                            }
                            string v_type = type->get_value_type()->get_type_name(prefix);
                            string v_type_ptr = type->get_value_type()->get_type_ptr(prefix);
                            string tn = type->get_type_name(prefix);
                            tn = common_utils::get_normalized_name(tn);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_key = CPPT_TOKEN_DEF_KEY_TYPE;
                            string tk_value = CPPT_TOKEN_DEF_VALUE_TYPE;
                            string tk_value_ptr = CPPT_TOKEN_DEF_VALUE_TYPE_PTR;

                            REPLACE_TOKEN(str, tk_name, tn);
                            REPLACE_TOKEN(str, tk_key, k_type);
                            REPLACE_TOKEN(str, tk_value, v_type);
                            REPLACE_TOKEN(str, tk_value_ptr, v_type_ptr);

                            TRACE("SETTER [%s]", str.c_str());

                            if (mutate)
                                add_private_method(m_class, str);
                            else
                                add_private_method(ro_class, str);
                        }

                        void generate_map_add(const __native_type *type, string token) {
                            const __map_type *mt = dynamic_cast<const __map_type *>(type);
                            CHECK_CAST(mt, TYPE_NAME(__native_type), TYPE_NAME(__map_type));

                            string str;
                            READ_PARSED_ROWS(str, template_header, token);

                            string k_type = mt->get_key_type()->get_type_name(CPPT_MUTABLE_CLASS_PREFIX);
                            if (mt->get_key_type()->get_datatype() == __type_def_enum::TYPE_STRING) {
                                k_type = "std::string";
                            }
                            string v_type = mt->get_value_type()->get_type_name(CPPT_MUTABLE_CLASS_PREFIX);
                            string v_type_ptr = mt->get_value_type()->get_type_ptr(CPPT_MUTABLE_CLASS_PREFIX);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_key = CPPT_TOKEN_DEF_KEY_TYPE;
                            string tk_value = CPPT_TOKEN_DEF_VALUE_TYPE;
                            string tk_value_ptr = CPPT_TOKEN_DEF_VALUE_TYPE_PTR;

                            REPLACE_TOKEN(str, tk_name, mt->get_name());
                            REPLACE_TOKEN(str, tk_key, k_type);
                            REPLACE_TOKEN(str, tk_value, v_type);
                            REPLACE_TOKEN(str, tk_value_ptr, v_type_ptr);

                            TRACE("SETTER [%s]", str.c_str());

                            add_public_method(m_class, str);
                        }

                        void generate_setter(const __native_type *type, string token) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, token);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;

                            string dt = type->get_type_ptr(CPPT_MUTABLE_CLASS_PREFIX);
                            CHECK_NOT_EMPTY(dt);
                            string tn = type->get_name();
                            CHECK_NOT_EMPTY(tn);

                            REPLACE_TOKEN(str, tk_name, tn);
                            REPLACE_TOKEN(str, tk_type, dt);

                            TRACE("SETTER [%s]", str.c_str());

                            add_public_method(m_class, str);
                        }

                        void generate_native_setter(const __native_type *type) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, CPPT_TOKEN_FUNC_NATIVE_SETTER_DEF);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;

                            string dt = type->get_type_name(CPPT_MUTABLE_CLASS_PREFIX);
                            CHECK_NOT_EMPTY(dt);
                            string tn = type->get_name();
                            CHECK_NOT_EMPTY(tn);
                            string dtp = type->get_type_ptr(CPPT_MUTABLE_CLASS_PREFIX);

                            REPLACE_TOKEN(str, tk_type, dt);
                            REPLACE_TOKEN(str, tk_name, tn);
                            REPLACE_TOKEN(str, tk_type_ptr, dtp);

                            TRACE("SETTER [%s]", str.c_str());

                            add_public_method(m_class, str);
                        }

                        void generate_getter(const __native_type *type, string token) {

                            string tk_type = CPPT_TOKEN_DEF_RETURN;
                            string tk_name = CPPT_TOKEN_DEF_NAME;

                            {
                                string str;
                                READ_PARSED_ROWS(str, template_header, token);

                                string dt = type->get_type_ptr(CPPT_MUTABLE_CLASS_PREFIX);
                                CHECK_NOT_EMPTY(dt);
                                string tn = type->get_name();
                                CHECK_NOT_EMPTY(tn);

                                REPLACE_TOKEN(str, tk_name, tn);
                                REPLACE_TOKEN(str, tk_type, dt);

                                TRACE("GETTER [%s]", str.c_str());
                                add_public_method(m_class, str);
                            }
                            {
                                string str;
                                READ_PARSED_ROWS(str, template_header, token);

                                string dt = type->get_type_ptr(common_consts::EMPTY_STRING);
                                CHECK_NOT_EMPTY(dt);
                                string tn = type->get_name();
                                CHECK_NOT_EMPTY(tn);

                                REPLACE_TOKEN(str, tk_name, tn);
                                REPLACE_TOKEN(str, tk_type, dt);

                                TRACE("GETTER [%s]", str.c_str());
                                add_public_method(ro_class, str);
                            }
                        }

                        void add_native_copy(const __native_type *type, string token) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, token);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            REPLACE_TOKEN(str, tk_name, type->get_name());

                            copy_constr_calls.push_back(str);
                        }

                        void add_type_copy(const __native_type *type) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, CPPT_TOKEN_COPY_CALL_TYPE);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;

                            REPLACE_TOKEN(str, tk_name, type->get_name());
                            REPLACE_TOKEN(str, tk_type, type->get_type_name(CPPT_MUTABLE_CLASS_PREFIX));

                            copy_constr_calls.push_back(str);
                        }

                        void add_list_copy(__list_type *type, string token) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, token);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;
                            string tk_s_type_ptr = CPPT_TOKEN_DEF_SRC_TYPE_PTR;

                            const __native_type *it = type->get_inner_type();

                            string dt = it->get_type_name(CPPT_MUTABLE_CLASS_PREFIX);
                            string dtp = it->get_type_ptr(CPPT_MUTABLE_CLASS_PREFIX);
                            string sdtp = it->get_type_ptr(common_consts::EMPTY_STRING);

                            REPLACE_TOKEN(str, tk_name, type->get_name());
                            REPLACE_TOKEN(str, tk_type, dt);
                            REPLACE_TOKEN(str, tk_type_ptr, dtp);
                            REPLACE_TOKEN(str, tk_s_type_ptr, sdtp);

                            copy_constr_calls.push_back(str);
                        }

                        void add_map_copy(__map_type *type, string token) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, token);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_k_type = CPPT_TOKEN_DEF_KEY_TYPE;
                            string tk_v_type_ptr = CPPT_TOKEN_DEF_VALUE_TYPE_PTR;
                            string tk_v_type = CPPT_TOKEN_DEF_VALUE_TYPE;
                            string tk_sv_type_ptr = CPPT_TOKEN_DEF_SRC_VALUE_TYPE_PTR;

                            const __native_type *kt = type->get_key_type();
                            const __native_type *vt = type->get_value_type();

                            string kdt = kt->get_type_name(CPPT_MUTABLE_CLASS_PREFIX);
                            if (type->get_key_type()->get_datatype() == __type_def_enum::TYPE_STRING) {
                                kdt = "std::string";
                            }

                            REPLACE_TOKEN(str, tk_name, type->get_name());
                            REPLACE_TOKEN(str, tk_k_type, kdt);
                            REPLACE_TOKEN(str, tk_v_type, vt->get_type_name(CPPT_MUTABLE_CLASS_PREFIX));
                            REPLACE_TOKEN(str, tk_v_type_ptr, vt->get_type_ptr(CPPT_MUTABLE_CLASS_PREFIX));
                            REPLACE_TOKEN(str, tk_sv_type_ptr, vt->get_type_ptr(common_consts::EMPTY_STRING));

                            copy_constr_calls.push_back(str);
                        }

                        void add_native_copy_ptr(const __native_type *type, string token) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, token);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            REPLACE_TOKEN(str, tk_name, type->get_name());

                            copy_constr_calls_ptr.push_back(str);
                        }

                        void add_type_copy_ptr(const __native_type *type) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, CPPT_TOKEN_COPY_CALL_TYPE_PTR);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;

                            REPLACE_TOKEN(str, tk_name, type->get_name());
                            REPLACE_TOKEN(str, tk_type, type->get_type_name(CPPT_MUTABLE_CLASS_PREFIX));

                            copy_constr_calls_ptr.push_back(str);
                        }

                        void add_list_copy_ptr(__list_type *type, string token) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, token);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;
                            string tk_s_type_ptr = CPPT_TOKEN_DEF_SRC_TYPE_PTR;

                            const __native_type *it = type->get_inner_type();

                            string dt = it->get_type_name(CPPT_MUTABLE_CLASS_PREFIX);
                            string dtp = it->get_type_ptr(CPPT_MUTABLE_CLASS_PREFIX);
                            string sdtp = it->get_type_ptr(common_consts::EMPTY_STRING);

                            REPLACE_TOKEN(str, tk_name, type->get_name());
                            REPLACE_TOKEN(str, tk_type, dt);
                            REPLACE_TOKEN(str, tk_type_ptr, dtp);
                            REPLACE_TOKEN(str, tk_s_type_ptr, sdtp);

                            copy_constr_calls_ptr.push_back(str);
                        }

                        void add_map_copy_ptr(__map_type *type, string token) {
                            string str;
                            READ_PARSED_ROWS(str, template_header, token);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_k_type = CPPT_TOKEN_DEF_KEY_TYPE;
                            string tk_v_type_ptr = CPPT_TOKEN_DEF_VALUE_TYPE_PTR;
                            string tk_v_type = CPPT_TOKEN_DEF_VALUE_TYPE;
                            string tk_sv_type_ptr = CPPT_TOKEN_DEF_SRC_VALUE_TYPE_PTR;

                            const __native_type *kt = type->get_key_type();
                            const __native_type *vt = type->get_value_type();

                            string kdt = kt->get_type_name(CPPT_MUTABLE_CLASS_PREFIX);
                            if (type->get_key_type()->get_datatype() == __type_def_enum::TYPE_STRING) {
                                kdt = "std::string";
                            }

                            REPLACE_TOKEN(str, tk_name, type->get_name());
                            REPLACE_TOKEN(str, tk_k_type, kdt);
                            REPLACE_TOKEN(str, tk_v_type, vt->get_type_name(CPPT_MUTABLE_CLASS_PREFIX));
                            REPLACE_TOKEN(str, tk_v_type_ptr, vt->get_type_ptr(CPPT_MUTABLE_CLASS_PREFIX));
                            REPLACE_TOKEN(str, tk_sv_type_ptr, vt->get_type_ptr(common_consts::EMPTY_STRING));

                            copy_constr_calls_ptr.push_back(str);
                        }

                        void get_declare(const __native_type *type) {

                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tn = type->get_name();
                            {
                                string str;
                                READ_PARSED_ROWS(str, template_header, CPPT_TOKEN_VARIABLE_NATIVE_DEF);
                                string dt = type->get_type_ptr(common_consts::EMPTY_STRING);

                                REPLACE_TOKEN(str, tk_name, tn);
                                REPLACE_TOKEN(str, tk_type, dt);

                                TRACE("DECLARE [%s]", str.c_str());
                                string c_str(str);
                                if (!type->has_complex_type()) {
                                    c_str = common_utils::format("const %s", str.c_str());
                                }
                                add_declare(c_str, ro_class);
                            }
                            {
                                string str;
                                READ_PARSED_ROWS(str, template_header, CPPT_TOKEN_VARIABLE_NATIVE_DEF);
                                string dt = type->get_type_ptr(CPPT_MUTABLE_CLASS_PREFIX);

                                REPLACE_TOKEN(str, tk_name, tn);
                                REPLACE_TOKEN(str, tk_type, dt);

                                TRACE("DECLARE [%s]", str.c_str());
                                add_declare(str, m_class);
                            }

                            variables.push_back(tn);
                        }


                        void generate_type_serializer(const __native_type *type) {
                            const __complex_type *ct = dynamic_cast<const __complex_type *>(type);
                            CHECK_CAST(ct, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));

                            string str;
                            READ_PARSED_ROWS(str, template_header, CPPT_TOKEN_FUNC_TYPE_SERIALIZER);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;
                            string tk_m_name = CPPT_TOKEN_DEF_M_NAME;

                            string dt = (type->get_type_name(CPPT_MUTABLE_CLASS_PREFIX));
                            string dtp = (type->get_type_ptr(CPPT_MUTABLE_CLASS_PREFIX));

                            CHECK_NOT_EMPTY(dt);
                            string tn = common_utils::get_normalized_name(dt);

                            REPLACE_TOKEN(str, tk_name, tn);
                            REPLACE_TOKEN(str, tk_m_name, dt);
                            REPLACE_TOKEN(str, tk_type_ptr, dtp);

                            TRACE("SERIALIZER [%s]", str.c_str());
                            add_private_method(m_class, str);
                        }

                        void generate_type_deserializer(const __native_type *type) {
                            const __complex_type *ct = dynamic_cast<const __complex_type *>(type);
                            CHECK_CAST(ct, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));

                            string str;
                            READ_PARSED_ROWS(str, template_header, CPPT_TOKEN_FUNC_TYPE_DESERIALIZER);

                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;

                            string dt = type->get_type_name(common_consts::EMPTY_STRING);
                            string dtp = type->get_type_ptr(common_consts::EMPTY_STRING);
                            CHECK_NOT_EMPTY(dt);
                            string tn = common_utils::get_normalized_name(dt);
                            CHECK_NOT_EMPTY(tn);

                            REPLACE_TOKEN(str, tk_name, tn);
                            REPLACE_TOKEN(str, tk_type, dt);
                            REPLACE_TOKEN(str, tk_type_ptr, dtp);

                            TRACE("DESERIALIZER [%s]", str.c_str());
                            add_private_method(ro_class, str);
                        }

                        void generate_native_setter_to_map(const __native_type *type) {
                            string f_str;
                            READ_PARSED_ROWS(f_str, template_header, CPPT_TOKEN_FUNC_SETTER_TO_RECORD);

                            string c_str;
                            READ_PARSED_ROWS(c_str, template_header, CPPT_TOKEN_CALL_NATIVE_SETTER_TO_RECORD);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tn = type->get_name();

                            REPLACE_TOKEN(c_str, tk_name, tn);

                            string tk_read = CPPT_TOKEN_DEF_SET_MAP_CALLS;

                            REPLACE_TOKEN(f_str, tk_name, tn);
                            REPLACE_TOKEN(f_str, tk_read, c_str);

                            TRACE("WRITE FROM MAP [%s]", f_str.c_str());
                            add_private_method(m_class, f_str);
                        }

                        void generate_native_setter_from_map(const __native_type *type) {
                            string f_str;
                            READ_PARSED_ROWS(f_str, template_header, CPPT_TOKEN_FUNC_SETTER_FROM_RECORD);

                            string c_str;
                            READ_PARSED_ROWS(c_str, template_header, CPPT_TOKEN_CALL_NATIVE_READ_FROM_RECORD);

                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_index = CPPT_TOKEN_DEF_FIELD_INDEX;

                            string tn = type->get_name();
                            string dt = __type_enum_helper::get_datatype(type->get_datatype());
                            string tp = type->get_type_ptr(common_consts::EMPTY_STRING);
                            string fi = to_string(type->get_index());

                            REPLACE_TOKEN(c_str, tk_name, tn);
                            REPLACE_TOKEN(c_str, tk_type, dt);
                            REPLACE_TOKEN(c_str, tk_type_ptr, tp);

                            string tk_read = CPPT_TOKEN_DEF_READ_SERDE_CALLS;
                            REPLACE_TOKEN(f_str, tk_name, tn);
                            REPLACE_TOKEN(f_str, tk_read, c_str);
                            REPLACE_TOKEN(f_str, tk_index, fi);

                            TRACE("READ FROM MAP [%s]", f_str.c_str());
                            add_private_method(ro_class, f_str);

                            m_call_serde.push_back(tn);
                        }

                        void generate_type_setter_to_map(__complex_type *type) {
                            string f_str;
                            READ_PARSED_ROWS(f_str, template_header, CPPT_TOKEN_FUNC_SETTER_TO_RECORD);

                            string c_str;
                            READ_PARSED_ROWS(c_str, template_header, CPPT_TOKEN_CALL_TYPE_SETTER_TO_RECORD);

                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;

                            string tn = type->get_name();
                            string dt = (type->get_type_name(CPPT_MUTABLE_CLASS_PREFIX));

                            REPLACE_TOKEN(c_str, tk_name, tn);
                            REPLACE_TOKEN(c_str, tk_type, dt);

                            string tk_read = CPPT_TOKEN_DEF_SET_MAP_CALLS;
                            REPLACE_TOKEN(f_str, tk_name, tn);
                            REPLACE_TOKEN(f_str, tk_read, c_str);

                            TRACE("WRITE FROM MAP [%s]", f_str.c_str());
                            add_private_method(m_class, f_str);

                        }

                        void generate_type_setter_from_map(__complex_type *type) {
                            string f_str;
                            READ_PARSED_ROWS(f_str, template_header, CPPT_TOKEN_FUNC_SETTER_FROM_RECORD);

                            string c_str;
                            READ_PARSED_ROWS(c_str, template_header, CPPT_TOKEN_CALL_TYPE_READ_FROM_RECORD);

                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_index = CPPT_TOKEN_DEF_FIELD_INDEX;

                            string tn = type->get_name();
                            string dt = type->get_type_name(common_consts::EMPTY_STRING);
                            string tp = type->get_type_ptr(common_consts::EMPTY_STRING);
                            string fi = to_string(type->get_index());

                            REPLACE_TOKEN(c_str, tk_name, tn);
                            REPLACE_TOKEN(c_str, tk_type, dt);
                            REPLACE_TOKEN(c_str, tk_type_ptr, tp);

                            string tk_read = CPPT_TOKEN_DEF_READ_SERDE_CALLS;
                            REPLACE_TOKEN(f_str, tk_name, tn);
                            REPLACE_TOKEN(f_str, tk_read, c_str);
                            REPLACE_TOKEN(f_str, tk_index, fi);

                            TRACE("READ FROM MAP [%s]", f_str.c_str());
                            add_private_method(ro_class, f_str);

                            m_call_serde.push_back(tn);
                        }

                        void generate_type_list_setter_to_map(__list_type *type) {
                            string f_str;
                            READ_PARSED_ROWS(f_str, template_header, CPPT_TOKEN_FUNC_SETTER_TO_RECORD);

                            string c_str;
                            READ_PARSED_ROWS(c_str, template_header, CPPT_TOKEN_CALL_TYPE_LIST_SETTER_TO_RECORD);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_m_name = CPPT_TOKEN_DEF_M_NAME;

                            string tn = type->get_name();

                            string mn = (type->get_type_name(CPPT_MUTABLE_CLASS_PREFIX));
                            mn = common_utils::get_normalized_name(mn);

                            REPLACE_TOKEN(c_str, tk_name, tn);
                            REPLACE_TOKEN(c_str, tk_m_name, mn);

                            string tk_read = CPPT_TOKEN_DEF_SET_MAP_CALLS;
                            REPLACE_TOKEN(f_str, tk_name, tn);
                            REPLACE_TOKEN(f_str, tk_read, c_str);

                            TRACE("WRITE FROM MAP [%s]", f_str.c_str());
                            add_private_method(m_class, f_str);

                        }

                        void generate_type_list_setter_from_map(__list_type *type) {
                            string f_str;
                            READ_PARSED_ROWS(f_str, template_header, CPPT_TOKEN_FUNC_SETTER_FROM_RECORD);

                            string c_str;
                            READ_PARSED_ROWS(c_str, template_header, CPPT_TOKEN_CALL_TYPE_LIST_READ_FROM_RECORD);

                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_m_name = CPPT_TOKEN_DEF_M_NAME;
                            string tk_index = CPPT_TOKEN_DEF_FIELD_INDEX;

                            const __native_type *it = type->get_inner_type();
                            string tn = type->get_name();

                            string dt = it->get_type_name(common_consts::EMPTY_STRING);
                            string tp = it->get_type_ptr(common_consts::EMPTY_STRING);
                            string mn = type->get_type_name(common_consts::EMPTY_STRING);
                            mn = common_utils::get_normalized_name(mn);
                            string fi = to_string(type->get_index());

                            REPLACE_TOKEN(c_str, tk_name, tn);
                            REPLACE_TOKEN(c_str, tk_type, dt);
                            REPLACE_TOKEN(c_str, tk_type_ptr, tp);
                            REPLACE_TOKEN(c_str, tk_m_name, mn);

                            string tk_read = CPPT_TOKEN_DEF_READ_SERDE_CALLS;
                            REPLACE_TOKEN(f_str, tk_name, tn);
                            REPLACE_TOKEN(f_str, tk_read, c_str);
                            REPLACE_TOKEN(f_str, tk_index, fi);

                            TRACE("READ FROM MAP [%s]", f_str.c_str());
                            add_private_method(ro_class, f_str);

                            m_call_serde.push_back(tn);
                        }

                        void generate_type_map_setter_to_map(__map_type *type) {
                            string f_str;
                            READ_PARSED_ROWS(f_str, template_header, CPPT_TOKEN_FUNC_SETTER_TO_RECORD);

                            string c_str;
                            READ_PARSED_ROWS(c_str, template_header, CPPT_TOKEN_CALL_TYPE_MAP_SETTER_TO_RECORD);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_m_name = CPPT_TOKEN_DEF_M_NAME;

                            string mn = (type->get_type_name(CPPT_MUTABLE_CLASS_PREFIX));
                            mn = common_utils::get_normalized_name(mn);

                            string tn = type->get_name();

                            REPLACE_TOKEN(c_str, tk_name, tn);
                            REPLACE_TOKEN(c_str, tk_m_name, mn);

                            string tk_read = CPPT_TOKEN_DEF_SET_MAP_CALLS;
                            REPLACE_TOKEN(f_str, tk_name, tn);
                            REPLACE_TOKEN(f_str, tk_read, c_str);

                            TRACE("READ FROM MAP [%s]", f_str.c_str());
                            add_private_method(m_class, f_str);

                        }

                        void generate_type_map_setter_from_map(__map_type *type) {
                            string f_str;
                            READ_PARSED_ROWS(f_str, template_header, CPPT_TOKEN_FUNC_SETTER_FROM_RECORD);

                            string c_str;
                            READ_PARSED_ROWS(c_str, template_header, CPPT_TOKEN_CALL_TYPE_MAP_READ_FROM_RECORD);

                            string tk_v_type_ptr = CPPT_TOKEN_DEF_VALUE_TYPE_PTR;
                            string tk_k_type = CPPT_TOKEN_DEF_KEY_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_m_name = CPPT_TOKEN_DEF_M_NAME;
                            string tk_index = CPPT_TOKEN_DEF_FIELD_INDEX;

                            const __native_type *kt = type->get_key_type();
                            const __native_type *vt = type->get_value_type();

                            string mn = type->get_type_name(common_consts::EMPTY_STRING);
                            mn = common_utils::get_normalized_name(mn);
                            string fi = to_string(type->get_index());

                            string tn = type->get_name();
                            string kdt = kt->get_type_name(common_consts::EMPTY_STRING);
                            if (type->get_key_type()->get_datatype() == __type_def_enum::TYPE_STRING) {
                                kdt = "std::string";
                            }
                            string vdtp = vt->get_type_ptr(common_consts::EMPTY_STRING);

                            REPLACE_TOKEN(c_str, tk_name, tn);
                            REPLACE_TOKEN(c_str, tk_k_type, kdt);
                            REPLACE_TOKEN(c_str, tk_v_type_ptr, vdtp);
                            REPLACE_TOKEN(c_str, tk_m_name, mn);

                            string tk_read = CPPT_TOKEN_DEF_READ_SERDE_CALLS;
                            REPLACE_TOKEN(f_str, tk_name, tn);
                            REPLACE_TOKEN(f_str, tk_read, c_str);
                            REPLACE_TOKEN(f_str, tk_index, fi);

                            TRACE("READ FROM MAP [%s]", f_str.c_str());
                            add_private_method(ro_class, f_str);

                            m_call_serde.push_back(tn);
                        }

                        void generate_free_call(const __native_type *type, string token, bool add_both) {
                            string f_str;
                            READ_PARSED_ROWS(f_str, template_header, token);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tn = type->get_name();
                            REPLACE_TOKEN(f_str, tk_name, tn);

                            m_class.free_calls.push_back(f_str);
                            if (add_both) {
                                ro_class.free_calls.push_back(f_str);
                            }
                        }

                        void generate_free_call_local(const __native_type *type, string token, bool add_both) {
                            string f_str;
                            READ_PARSED_ROWS(f_str, template_header, token);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tn = type->get_name();
                            REPLACE_TOKEN(f_str, tk_name, tn);

                            m_class.free_calls.push_back(f_str);
                            if (add_both) {
                                ro_class.free_calls.push_back(f_str);
                            }
                        }

                        string finish() {
                            string includes;
                            if (!IS_EMPTY(headers)) {
                                stringstream buff;
                                unordered_map<string, string>::iterator iter;
                                for (iter = headers.begin(); iter != headers.end(); iter++) {
                                    buff << "#include \"" << iter->second << "\"\n";
                                }

                                includes = string(buff.str());
                            }
                            string header;
                            READ_PARSED_ROWS(header, template_header, CPPT_TOKEN_INCLUDE_BASE);

                            string tk_includes = CPPT_TOKEN_DEF_INCLUDES;
                            CHECK_NOT_EMPTY(tk_includes);

                            REPLACE_TOKEN(header, tk_includes, includes);

                            this->file_str = string_utils::set_token(tk_includes, header, this->file_str);

                            string body = get_class_def();
                            CHECK_NOT_EMPTY(body);

                            string tk_body = CPPT_TOKEN_DEF_BODY;
                            CHECK_NOT_EMPTY(tk_body);
                            this->file_str = string_utils::set_token(tk_body, body, this->file_str);

                            return this->file_str;
                        }
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_CPP_FILE_TEMPLATE_H
