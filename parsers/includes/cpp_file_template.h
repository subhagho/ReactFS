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

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace parsers {
                    class cpp_file_template {
                    private:
                        cpp_template_header template_header;

                        string classname;
                        string name_space;
                        string file_str;
                        string class_str;
                        stringstream declare_s;
                        stringstream pr_methods_s;
                        stringstream pu_methods_s;
                        unordered_map<string, string> headers;
                        vector<string> m_call_serde;
                        vector<string> variables;
                        vector<string> free_calls;
                        vector<string> struct_free_calls;

                        string get_file_header(const string &classname, const string &schema_name,
                                               __version_header &version) {
                            CHECK_NOT_EMPTY(classname);

                            stringstream stream;
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FILE_COMMENT);
                            CHECK_NOT_EMPTY_P(ft);

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

                            CHECK_NOT_EMPTY_P(ft);
                            for (string str : *ft) {
                                string ss = string(str);
                                stream << ss << "\n";
                            }

                            string ss = string(stream.str());
                            ss = string_utils::set_token(tk_schema, schema_name, ss);
                            ss = string_utils::set_token(tk_version, version_s, ss);
                            ss = string_utils::set_token(tk_username, uname, ss);
                            ss = string_utils::set_token(tk_date, date_s, ss);

                            return ss;
                        }

                        void
                        init_file_template(const string &classname, const string &schema_name,
                                           __version_header &version) {
                            CHECK_NOT_EMPTY(classname);

                            string header = get_file_header(classname, schema_name, version);
                            CHECK_NOT_EMPTY(header);

                            stringstream stream;

                            string guard_name = string_utils::toupper(classname);
                            guard_name = common_utils::format("%s_H", guard_name.c_str());

                            string tk_guard = CPPT_TOKEN_DEF_GUARD_NAME;
                            CHECK_NOT_EMPTY(tk_guard);
                            string tk_header = CPPT_TOKEN_DEF_HEADER;
                            CHECK_NOT_EMPTY(tk_header);

                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FILE_DEF);
                            for (string str : *ft) {
                                string vs = string(str);
                                stream << vs << "\n";
                            }

                            string vs = string(stream.str());
                            vs = string_utils::set_token(tk_guard, guard_name, vs);
                            vs = string_utils::set_token(tk_header, header, vs);

                            this->file_str = string(vs);
                            this->classname = string(classname);
                        }

                        void init_class_template() {
                            stringstream stream;
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_CLASS_DEF);
                            CHECK_NOT_EMPTY_P(ft);

                            string tk_class = CPPT_TOKEN_DEF_NAME;
                            string tk_parent = CPPT_TOKEN_DEF_PARENT;

                            for (string str : *ft) {
                                string vs = string(str);
                                stream << vs << "\n";
                            }
                            string vs = string(stream.str());
                            vs = string_utils::set_token(tk_class, this->classname, vs);
                            vs = string_utils::set_token(tk_parent, CPPT_CLASS_PARENT_TYPE, vs);
                            this->class_str = string(vs);
                        }

                        string get_namespace(string &ns) {
                            stringstream stream;
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_NAMESPACE);
                            CHECK_NOT_EMPTY_P(ft);

                            for (string ss : *ft) {
                                stream << ss << "\n";
                            }
                            string tk = CPPT_TOKEN_DEF_NAME;
                            string str = string(stream.str());
                            str = string_utils::set_token(tk, ns, str);

                            return str;
                        }

                        string get_namespace() {
                            CHECK_NOT_EMPTY(this->name_space);
                            vector<string> parts;
                            string_utils::split(this->name_space, '.', &parts);
                            CHECK_NOT_EMPTY(parts);

                            string prev;
                            for (uint8_t ii = parts.size() - 1; ii >= 0 && ii < parts.size(); ii--) {
                                string ns = parts[ii];
                                CHECK_NOT_EMPTY(ns);
                                string n_str = get_namespace(ns);
                                CHECK_NOT_EMPTY(n_str);
                                if (!IS_EMPTY(prev)) {
                                    string tk = CPPT_TOKEN_DEF_NESTED;
                                    n_str = string_utils::set_token(tk, prev, n_str);
                                    prev = n_str;
                                } else {
                                    prev = n_str;
                                }
                            }
                            return prev;
                        }

                        void generate_deserializer() {
                            stringstream stream;
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_DESERIALIZE);
                            CHECK_NOT_EMPTY_P(ft);

                            for (string ss : *ft) {
                                stream << ss << "\n";
                            }
                            string fstr = string(stream.str());

                            stringstream dstream;
                            ft = template_header.find_token(CPPT_TOKEN_CALL_DESERIALIZE_VALUE);
                            CHECK_NOT_EMPTY_P(ft);
                            for (string ss : *ft) {
                                dstream << ss << "\n";
                            }
                            string callstr(dstream.str());
                            string tk_name = CPPT_TOKEN_DEF_NAME;

                            stringstream body;
                            for (string ss : m_call_serde) {
                                string c = string(callstr);
                                c = string_utils::set_token(tk_name, ss, c);
                                body << c << "\n";
                            }
                            string bstr(body.str());

                            string tk_read = CPPT_TOKEN_DEF_READ_MAP_CALLS;
                            fstr = string_utils::set_token(tk_read, bstr, fstr);

                            TRACE("DESERIALIZER [%s]", fstr.c_str());

                            add_public_method(fstr);
                        }

                        void generate_serializer() {
                            stringstream stream;
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_SERIALIZE);
                            CHECK_NOT_EMPTY_P(ft);

                            for (string ss : *ft) {
                                stream << ss << "\n";
                            }
                            string fstr = string(stream.str());

                            stringstream dstream;
                            ft = template_header.find_token(CPPT_TOKEN_CALL_SETTER_TO_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            for (string ss : *ft) {
                                dstream << ss << "\n";
                            }
                            string callstr(dstream.str());
                            string tk_name = CPPT_TOKEN_DEF_NAME;

                            stringstream body;
                            for (string ss : m_call_serde) {
                                string c = string(callstr);
                                c = string_utils::set_token(tk_name, ss, c);
                                body << c << "\n";
                            }
                            string bstr(body.str());

                            string tk_read = CPPT_TOKEN_DEF_SET_MAP_CALLS;
                            fstr = string_utils::set_token(tk_read, bstr, fstr);

                            TRACE("DESERIALIZER [%s]", fstr.c_str());

                            add_public_method(fstr);
                        }

                        void generate_empty_constr() {
                            stringstream istream;
                            for (string var : variables) {
                                string ss = common_utils::format("this->%s = nullptr;", var.c_str());
                                istream << ss << "\n";
                            }
                            string inits = string(istream.str());

                            stringstream stream;
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_CONSTRUCTOR_WRITABLE);
                            CHECK_NOT_EMPTY_P(ft);

                            for (string ss : *ft) {
                                stream << ss << "\n";
                            }
                            string fstr = string(stream.str());

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_inits = CPPT_TOKEN_DEF_VARIABLE_INITS;

                            fstr = string_utils::set_token(tk_name, classname, fstr);
                            fstr = string_utils::set_token(tk_inits, inits, fstr);

                            add_public_method(fstr);
                        }

                        void generate_serde_constr() {
                            stringstream istream;
                            for (string var : variables) {
                                string ss = common_utils::format("this->%s = nullptr;", var.c_str());
                                istream << ss << "\n";
                            }
                            string inits = string(istream.str());

                            stringstream stream;
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_CONSTRUCTOR_READABLE);
                            CHECK_NOT_EMPTY_P(ft);

                            for (string ss : *ft) {
                                stream << ss << "\n";
                            }
                            string fstr = string(stream.str());

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_inits = CPPT_TOKEN_DEF_VARIABLE_INITS;

                            fstr = string_utils::set_token(tk_name, classname, fstr);
                            fstr = string_utils::set_token(tk_inits, inits, fstr);

                            add_public_method(fstr);
                        }

                        void generate_struct_free() {
                            stringstream istream;
                            for (string ss : struct_free_calls) {
                                istream << ss << "\n";
                            }
                            string inits = string(istream.str());

                            stringstream stream;
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_FREE_DATA_PTR);
                            CHECK_NOT_EMPTY_P(ft);

                            for (string ss : *ft) {
                                stream << ss << "\n";
                            }
                            string fstr = string(stream.str());
                            string tk_free = CPPT_TOKEN_DEF_FREE_DATA_CALLS;

                            fstr = string_utils::set_token(tk_free, inits, fstr);

                            add_public_method(fstr);
                        }

                        void generate_destr() {
                            stringstream istream;
                            for (string ss : free_calls) {
                                istream << ss << "\n";
                            }
                            string inits = string(istream.str());

                            stringstream stream;
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_DESTRUCTOR);
                            CHECK_NOT_EMPTY_P(ft);

                            for (string ss : *ft) {
                                stream << ss << "\n";
                            }
                            string fstr = string(stream.str());

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_inits = CPPT_TOKEN_DEF_VARIABLE_FREES;

                            fstr = string_utils::set_token(tk_name, classname, fstr);
                            fstr = string_utils::set_token(tk_inits, inits, fstr);

                            add_public_method(fstr);
                        }

                        string get_class_def() {
                            generate_empty_constr();
                            generate_serde_constr();
                            generate_destr();
                            generate_struct_free();
                            generate_deserializer();
                            generate_serializer();

                            string declares = declare_s.str();
                            string pr_methods = pr_methods_s.str();
                            string pu_methods = pu_methods_s.str();

                            if (!IS_EMPTY(declares)) {
                                string tk_declare = CPPT_TOKEN_DEF_DECLARATIONS;
                                CHECK_NOT_EMPTY(tk_declare);

                                this->class_str = string_utils::set_token(tk_declare, declares, this->class_str);
                            }

                            string tk_prm = CPPT_TOKEN_DEF_PRIVATE_FUNCTIONS;
                            CHECK_NOT_EMPTY(tk_prm);

                            if (!IS_EMPTY(pr_methods)) {
                                this->class_str = string_utils::set_token(tk_prm, pr_methods, this->class_str);
                            } else {
                                this->class_str = string_utils::set_token(tk_prm, "", this->class_str);
                            }

                            if (!IS_EMPTY(pu_methods)) {
                                string tk_pum = CPPT_TOKEN_DEF_PUBLIC_FUNCTIONS;
                                CHECK_NOT_EMPTY(tk_pum);

                                this->class_str = string_utils::set_token(tk_pum, pu_methods, this->class_str);
                            }

                            if (!IS_EMPTY(this->name_space)) {
                                string nstr = get_namespace();
                                CHECK_NOT_EMPTY(nstr);

                                string tk = CPPT_TOKEN_DEF_NESTED;
                                CHECK_NOT_EMPTY(tk);
                                nstr = string_utils::set_token(tk, this->class_str, nstr);
                                return nstr;
                            }
                            return this->class_str;
                        }

                    public:
                        cpp_file_template(const string &classname, const string &schema_name, const string &name_space,
                                          __version_header &version) {
                            this->init_file_template(classname, schema_name, version);
                            this->init_class_template();
                            this->name_space = string(name_space);
                        }

                        string get_class_name() {
                            return this->classname;
                        }

                        void add_declare(const string &declare) {
                            CHECK_NOT_EMPTY(declare);
                            declare_s << declare << "\n";
                        }

                        void add_private_method(const string &method) {
                            CHECK_NOT_EMPTY(method);
                            pr_methods_s << method << "\n";
                        }

                        void add_public_method(const string &method) {
                            CHECK_NOT_EMPTY(method);
                            pu_methods_s << method << "\n";
                        }

                        void add_header(const string &header) {
                            this->headers.insert({header, header});
                        }

                        void generate_list_serde(__list_type *type, string token) {
                            vector<string> *ft = template_header.find_token(token);
                            CHECK_NOT_EMPTY_P(ft);

                            stringstream buff;
                            for (string ss : *ft) {
                                string str = string(ss);
                                buff << str << "\n";
                            }
                            string str = string(buff.str());

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;

                            string tn = type->get_type_name();
                            tn = common_utils::get_normalized_name(tn);
                            CHECK_NOT_EMPTY(tn);
                            string dt = type->get_inner_type()->get_type_name();
                            CHECK_NOT_EMPTY(dt);
                            string dtp = type->get_inner_type()->get_type_ptr();
                            CHECK_NOT_EMPTY(dtp);

                            str = string_utils::set_token(tk_name, tn, str);
                            str = string_utils::set_token(tk_type, dt, str);
                            str = string_utils::set_token(tk_type_ptr, dtp, str);

                            TRACE("LIST SERIALIZER [%s]", str.c_str());

                            add_private_method(str);
                        }

                        void generate_list_add(__native_type *type, string token) {
                            __list_type *lt = dynamic_cast<__list_type *>(type);
                            CHECK_CAST(lt, TYPE_NAME(__native_type), TYPE_NAME(__list_type));

                            vector<string> *ft = template_header.find_token(token);
                            CHECK_NOT_EMPTY_P(ft);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;

                            stringstream buff;
                            for (string ss : *ft) {
                                string str = string(ss);
                                buff << str << "\n";
                            }
                            string str = string(buff.str());

                            string tn = lt->get_name();
                            CHECK_NOT_EMPTY(tn);
                            string dt = lt->get_inner_type()->get_type_name();
                            CHECK_NOT_EMPTY(dt);
                            string dtp = lt->get_inner_type()->get_type_ptr();
                            CHECK_NOT_EMPTY(dtp);

                            str = string_utils::set_token(tk_name, tn, str);
                            str = string_utils::set_token(tk_type, dt, str);
                            str = string_utils::set_token(tk_type_ptr, dtp, str);

                            TRACE("SETTER [%s]", str.c_str());

                            add_public_method(str);
                        }

                        void generate_map_serde(__map_type *type, string token) {

                            vector<string> *ft = template_header.find_token(token);
                            CHECK_NOT_EMPTY_P(ft);

                            string k_type = type->get_key_type()->get_type_name();
                            if (type->get_key_type()->get_datatype() == __type_def_enum::TYPE_STRING) {
                                k_type = "std::string";
                            }
                            string v_type = type->get_value_type()->get_type_name();
                            string v_type_ptr = type->get_value_type()->get_type_ptr();
                            string tn = type->get_type_name();
                            tn = common_utils::get_normalized_name(tn);

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_key = CPPT_TOKEN_DEF_KEY_TYPE;
                            string tk_value = CPPT_TOKEN_DEF_VALUE_TYPE;
                            string tk_value_ptr = CPPT_TOKEN_DEF_VALUE_TYPE_PTR;

                            stringstream buff;
                            for (string ss : *ft) {
                                string str = string(ss);
                                buff << str << "\n";
                            }
                            string str = string(buff.str());

                            str = string_utils::set_token(tk_name, tn, str);
                            str = string_utils::set_token(tk_key, k_type, str);
                            str = string_utils::set_token(tk_value, v_type, str);
                            str = string_utils::set_token(tk_value_ptr, v_type_ptr, str);

                            TRACE("SETTER [%s]", str.c_str());

                            add_private_method(str);
                        }

                        void generate_map_add(__native_type *type, string token) {
                            __map_type *mt = dynamic_cast<__map_type *>(type);
                            CHECK_CAST(mt, TYPE_NAME(__native_type), TYPE_NAME(__map_type));

                            vector<string> *ft = template_header.find_token(token);
                            CHECK_NOT_EMPTY_P(ft);

                            string k_type = mt->get_key_type()->get_type_name();
                            if (mt->get_key_type()->get_datatype() == __type_def_enum::TYPE_STRING) {
                                k_type = "std::string";
                            }
                            string v_type = mt->get_value_type()->get_type_name();
                            string v_type_ptr = mt->get_value_type()->get_type_ptr();

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_key = CPPT_TOKEN_DEF_KEY_TYPE;
                            string tk_value = CPPT_TOKEN_DEF_VALUE_TYPE;
                            string tk_value_ptr = CPPT_TOKEN_DEF_VALUE_TYPE_PTR;

                            stringstream buff;
                            for (string ss : *ft) {
                                string str = string(ss);
                                buff << str << "\n";
                            }
                            string str = string(buff.str());

                            str = string_utils::set_token(tk_name, mt->get_name(), str);
                            str = string_utils::set_token(tk_key, k_type, str);
                            str = string_utils::set_token(tk_value, v_type, str);
                            str = string_utils::set_token(tk_value_ptr, v_type_ptr, str);

                            TRACE("SETTER [%s]", str.c_str());

                            add_public_method(str);
                        }

                        void generate_setter(__native_type *type, string token) {
                            vector<string> *ft = template_header.find_token(token);
                            CHECK_NOT_EMPTY_P(ft);
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;

                            stringstream buff;
                            for (string ss : *ft) {
                                string str = string(ss);
                                buff << str << "\n";
                            }
                            string str = string(buff.str());
                            string dt = type->get_type_ptr();
                            CHECK_NOT_EMPTY(dt);
                            string tn = type->get_name();
                            CHECK_NOT_EMPTY(tn);

                            str = string_utils::set_token(tk_type, dt, str);
                            str = string_utils::set_token(tk_name, tn, str);

                            TRACE("SETTER [%s]", str.c_str());

                            add_public_method(str);
                        }

                        void generate_native_setter(__native_type *type) {
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_NATIVE_SETTER_DEF);
                            CHECK_NOT_EMPTY_P(ft);
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;

                            stringstream buff;
                            for (string ss : *ft) {
                                string str = string(ss);
                                buff << str << "\n";
                            }
                            string str = string(buff.str());
                            string dt = type->get_type_name();
                            CHECK_NOT_EMPTY(dt);
                            string tn = type->get_name();
                            CHECK_NOT_EMPTY(tn);
                            string dtp = type->get_type_ptr();

                            str = string_utils::set_token(tk_type, dt, str);
                            str = string_utils::set_token(tk_name, tn, str);
                            str = string_utils::set_token(tk_type_ptr, dtp, str);

                            TRACE("SETTER [%s]", str.c_str());

                            add_public_method(str);
                        }

                        void generate_getter(__native_type *type, string token) {
                            vector<string> *ft = template_header.find_token(token);
                            CHECK_NOT_EMPTY_P(ft);
                            string tk_type = CPPT_TOKEN_DEF_RETURN;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            stringstream buff;
                            for (string ss : *ft) {
                                string str = string(ss);
                                buff << str << "\n";
                            }
                            string str = string(buff.str());
                            string dt = type->get_type_ptr();
                            CHECK_NOT_EMPTY(dt);

                            string tn = type->get_name();
                            CHECK_NOT_EMPTY(tn);

                            str = string_utils::set_token(tk_type, dt, str);
                            str = string_utils::set_token(tk_name, tn, str);
                            TRACE("GETTER [%s]", str.c_str());
                            add_public_method(str);
                        }


                        string get_declare(__native_type *type) {
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_VARIABLE_NATIVE_DEF);
                            CHECK_NOT_EMPTY_P(ft);
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            stringstream buff;
                            for (string ss : *ft) {
                                string str = string(ss);
                                buff << str << "\n";
                            }
                            string str = string(buff.str());
                            string dt = type->get_type_ptr();
                            CHECK_NOT_EMPTY(dt);
                            string tn = type->get_name();
                            CHECK_NOT_EMPTY(tn);

                            str = string_utils::set_token(tk_type, dt, str);
                            str = string_utils::set_token(tk_name, tn, str);

                            TRACE("DECLARE [%s]", str.c_str());
                            add_declare(str);

                            variables.push_back(tn);

                            return str;
                        }


                        void generate_type_serializer(__native_type *type) {
                            __complex_type *ct = dynamic_cast<__complex_type *>(type);
                            CHECK_CAST(ct, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));

                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_TYPE_SERIALIZER);
                            CHECK_NOT_EMPTY_P(ft);
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;

                            stringstream buff;
                            for (string ss : *ft) {
                                string str = string(ss);
                                buff << str << "\n";
                            }
                            string str = string(buff.str());
                            string dt = type->get_type_name();
                            string dtp = type->get_type_ptr();

                            CHECK_NOT_EMPTY(dt);
                            string tn = common_utils::get_normalized_name(dt);

                            str = string_utils::set_token(tk_type, dt, str);
                            str = string_utils::set_token(tk_name, tn, str);
                            str = string_utils::set_token(tk_type_ptr, dtp, str);

                            TRACE("SERIALIZER [%s]", str.c_str());
                            add_private_method(str);
                        }

                        void generate_type_deserializer(__native_type *type) {
                            __complex_type *ct = dynamic_cast<__complex_type *>(type);
                            CHECK_CAST(ct, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));

                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_TYPE_DESERIALIZER);
                            CHECK_NOT_EMPTY_P(ft);
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;

                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }

                            string str = string(buff.str());
                            string dt = type->get_type_name();
                            string dtp = type->get_type_ptr();
                            CHECK_NOT_EMPTY(dt);
                            string tn = common_utils::get_normalized_name(dt);
                            CHECK_NOT_EMPTY(tn);

                            str = string_utils::set_token(tk_type, dt, str);
                            str = string_utils::set_token(tk_name, tn, str);
                            str = string_utils::set_token(tk_type_ptr, dtp, str);

                            TRACE("DESERIALIZER [%s]", str.c_str());
                            add_private_method(str);
                        }

                        void generate_native_setter_to_map(__native_type *type) {
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_SETTER_TO_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());

                            ft = template_header.find_token(CPPT_TOKEN_CALL_NATIVE_SETTER_TO_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream c_buff;
                            for (string ss : *ft) {
                                c_buff << ss << "\n";
                            }
                            string c_str = string(c_buff.str());

                            string tk_name = CPPT_TOKEN_DEF_NAME;

                            string tn = type->get_name();

                            c_str = string_utils::set_token(tk_name, tn, c_str);

                            string tk_read = CPPT_TOKEN_DEF_SET_MAP_CALLS;
                            f_str = string_utils::set_token(tk_name, tn, f_str);
                            f_str = string_utils::set_token(tk_read, c_str, f_str);

                            TRACE("WRITE FROM MAP [%s]", f_str.c_str());
                            add_private_method(f_str);
                        }

                        void generate_native_setter_from_map(__native_type *type) {
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_SETTER_FROM_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());

                            ft = template_header.find_token(CPPT_TOKEN_CALL_NATIVE_SETTER_FROM_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream c_buff;
                            for (string ss : *ft) {
                                c_buff << ss << "\n";
                            }
                            string c_str = string(c_buff.str());

                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;

                            string tn = type->get_name();
                            string dt = __type_enum_helper::get_datatype(type->get_datatype());
                            string tp = type->get_type_ptr();

                            c_str = string_utils::set_token(tk_name, tn, c_str);
                            c_str = string_utils::set_token(tk_type, dt, c_str);
                            c_str = string_utils::set_token(tk_type_ptr, tp, c_str);

                            string tk_read = CPPT_TOKEN_DEF_READ_MAP_CALLS;
                            f_str = string_utils::set_token(tk_name, tn, f_str);
                            f_str = string_utils::set_token(tk_read, c_str, f_str);

                            TRACE("READ FROM MAP [%s]", f_str.c_str());
                            add_private_method(f_str);

                            m_call_serde.push_back(tn);
                        }

                        void generate_type_setter_to_map(__complex_type *type) {
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_SETTER_TO_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());

                            ft = template_header.find_token(CPPT_TOKEN_CALL_TYPE_SETTER_TO_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream c_buff;
                            for (string ss : *ft) {
                                c_buff << ss << "\n";
                            }
                            string c_str = string(c_buff.str());

                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;

                            string tn = type->get_name();
                            string dt = type->get_type_name();

                            c_str = string_utils::set_token(tk_name, tn, c_str);
                            c_str = string_utils::set_token(tk_type, dt, c_str);

                            string tk_read = CPPT_TOKEN_DEF_SET_MAP_CALLS;
                            f_str = string_utils::set_token(tk_name, tn, f_str);
                            f_str = string_utils::set_token(tk_read, c_str, f_str);

                            TRACE("WRITE FROM MAP [%s]", f_str.c_str());
                            add_private_method(f_str);

                        }

                        void generate_type_setter_from_map(__complex_type *type) {
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_SETTER_FROM_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());

                            ft = template_header.find_token(CPPT_TOKEN_CALL_TYPE_SETTER_FROM_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream c_buff;
                            for (string ss : *ft) {
                                c_buff << ss << "\n";
                            }
                            string c_str = string(c_buff.str());

                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;

                            string tn = type->get_name();
                            string dt = type->get_type_name();
                            string tp = type->get_type_ptr();

                            c_str = string_utils::set_token(tk_name, tn, c_str);
                            c_str = string_utils::set_token(tk_type, dt, c_str);
                            c_str = string_utils::set_token(tk_type_ptr, tp, c_str);

                            string tk_read = CPPT_TOKEN_DEF_READ_MAP_CALLS;
                            f_str = string_utils::set_token(tk_name, tn, f_str);
                            f_str = string_utils::set_token(tk_read, c_str, f_str);

                            TRACE("READ FROM MAP [%s]", f_str.c_str());
                            add_private_method(f_str);

                            m_call_serde.push_back(tn);
                        }

                        void generate_type_list_setter_to_map(__list_type *type) {
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_SETTER_TO_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());

                            ft = template_header.find_token(CPPT_TOKEN_CALL_TYPE_LIST_SETTER_TO_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream c_buff;
                            for (string ss : *ft) {
                                c_buff << ss << "\n";
                            }
                            string c_str = string(c_buff.str());

                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_m_name = CPPT_TOKEN_DEF_M_NAME;

                            string tn = type->get_name();

                            string mn = type->get_type_name();
                            mn = common_utils::get_normalized_name(mn);

                            c_str = string_utils::set_token(tk_name, tn, c_str);
                            c_str = string_utils::set_token(tk_m_name, mn, c_str);

                            string tk_read = CPPT_TOKEN_DEF_SET_MAP_CALLS;
                            f_str = string_utils::set_token(tk_name, tn, f_str);
                            f_str = string_utils::set_token(tk_read, c_str, f_str);

                            TRACE("WRITE FROM MAP [%s]", f_str.c_str());
                            add_private_method(f_str);

                        }

                        void generate_type_list_setter_from_map(__list_type *type) {
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_SETTER_FROM_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());

                            ft = template_header.find_token(CPPT_TOKEN_CALL_TYPE_LIST_SETTER_FROM_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream c_buff;
                            for (string ss : *ft) {
                                c_buff << ss << "\n";
                            }
                            string c_str = string(c_buff.str());

                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;
                            string tk_type = CPPT_TOKEN_DEF_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_m_name = CPPT_TOKEN_DEF_M_NAME;

                            __native_type *it = type->get_inner_type();
                            string tn = type->get_name();

                            string dt = it->get_type_name();
                            string tp = it->get_type_ptr();
                            string mn = type->get_type_name();
                            mn = common_utils::get_normalized_name(mn);

                            c_str = string_utils::set_token(tk_name, tn, c_str);
                            c_str = string_utils::set_token(tk_type, dt, c_str);
                            c_str = string_utils::set_token(tk_type_ptr, tp, c_str);
                            c_str = string_utils::set_token(tk_m_name, mn, c_str);

                            string tk_read = CPPT_TOKEN_DEF_READ_MAP_CALLS;
                            f_str = string_utils::set_token(tk_name, tn, f_str);
                            f_str = string_utils::set_token(tk_read, c_str, f_str);

                            TRACE("READ FROM MAP [%s]", f_str.c_str());
                            add_private_method(f_str);

                            m_call_serde.push_back(tn);
                        }

                        void generate_type_map_setter_to_map(__map_type *type) {
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_SETTER_TO_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());

                            ft = template_header.find_token(CPPT_TOKEN_CALL_TYPE_MAP_SETTER_TO_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream c_buff;
                            for (string ss : *ft) {
                                c_buff << ss << "\n";
                            }
                            string c_str = string(c_buff.str());

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_m_name = CPPT_TOKEN_DEF_M_NAME;

                            string mn = type->get_type_name();
                            mn = common_utils::get_normalized_name(mn);

                            string tn = type->get_name();

                            c_str = string_utils::set_token(tk_name, tn, c_str);
                            c_str = string_utils::set_token(tk_m_name, mn, c_str);

                            string tk_read = CPPT_TOKEN_DEF_SET_MAP_CALLS;
                            f_str = string_utils::set_token(tk_name, tn, f_str);
                            f_str = string_utils::set_token(tk_read, c_str, f_str);

                            TRACE("READ FROM MAP [%s]", f_str.c_str());
                            add_private_method(f_str);

                        }

                        void generate_type_map_setter_from_map(__map_type *type) {
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_FUNC_SETTER_FROM_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());

                            ft = template_header.find_token(CPPT_TOKEN_CALL_TYPE_MAP_SETTER_FROM_MAP);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream c_buff;
                            for (string ss : *ft) {
                                c_buff << ss << "\n";
                            }
                            string c_str = string(c_buff.str());

                            string tk_v_type_ptr = CPPT_TOKEN_DEF_VALUE_TYPE_PTR;
                            string tk_k_type = CPPT_TOKEN_DEF_KEY_TYPE;
                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_m_name = CPPT_TOKEN_DEF_M_NAME;

                            __native_type *kt = type->get_key_type();
                            __native_type *vt = type->get_value_type();

                            string mn = type->get_type_name();
                            mn = common_utils::get_normalized_name(mn);

                            string tn = type->get_name();
                            string kdt = kt->get_type_name();
                            if (type->get_key_type()->get_datatype() == __type_def_enum::TYPE_STRING) {
                                kdt = "std::string";
                            }
                            string vdtp = vt->get_type_ptr();

                            c_str = string_utils::set_token(tk_name, tn, c_str);
                            c_str = string_utils::set_token(tk_k_type, kdt, c_str);
                            c_str = string_utils::set_token(tk_v_type_ptr, vdtp, c_str);
                            c_str = string_utils::set_token(tk_m_name, mn, c_str);

                            string tk_read = CPPT_TOKEN_DEF_READ_MAP_CALLS;
                            f_str = string_utils::set_token(tk_name, tn, f_str);
                            f_str = string_utils::set_token(tk_read, c_str, f_str);

                            TRACE("READ FROM MAP [%s]", f_str.c_str());
                            add_private_method(f_str);

                            m_call_serde.push_back(tn);
                        }

                        void generate_free_call(__native_type *type, string token) {
                            vector<string> *ft = template_header.find_token(token);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());
                            string tk_name = CPPT_TOKEN_DEF_NAME;

                            string tn = type->get_name();
                            f_str = string_utils::set_token(tk_name, tn, f_str);

                            free_calls.push_back(f_str);
                        }

                        void generate_var_free_call(__native_type *type, string token) {
                            vector<string> *ft = template_header.find_token(token);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tn = type->get_name();

                            f_str = string_utils::set_token(tk_name, tn, f_str);

                            struct_free_calls.push_back(f_str);
                        }

                        void generate_type_free_call(__native_type *type, string token) {
                            vector<string> *ft = template_header.find_token(token);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;

                            string tn = type->get_name();
                            string tt = type->get_type_ptr();

                            f_str = string_utils::set_token(tk_name, tn, f_str);
                            f_str = string_utils::set_token(tk_type_ptr, tt, f_str);

                            struct_free_calls.push_back(f_str);
                        }

                        void generate_list_free_call(__list_type *type, string token) {
                            vector<string> *ft = template_header.find_token(token);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_type_ptr = CPPT_TOKEN_DEF_TYPE_PTR;

                            __native_type *it = type->get_inner_type();

                            string tn = type->get_name();
                            string dtp = it->get_type_ptr();

                            f_str = string_utils::set_token(tk_name, tn, f_str);
                            f_str = string_utils::set_token(tk_type_ptr, dtp, f_str);

                            struct_free_calls.push_back(f_str);
                        }

                        void generate_map_free_call(__map_type *type, string token) {
                            vector<string> *ft = template_header.find_token(token);
                            CHECK_NOT_EMPTY_P(ft);
                            stringstream buff;
                            for (string ss : *ft) {
                                buff << ss << "\n";
                            }
                            string f_str = string(buff.str());

                            string tk_name = CPPT_TOKEN_DEF_NAME;
                            string tk_k_type = CPPT_TOKEN_DEF_KEY_TYPE;
                            string tk_v_type_ptr = CPPT_TOKEN_DEF_VALUE_TYPE_PTR;

                            __native_type *kt = type->get_key_type();
                            __native_type *vt = type->get_value_type();

                            string tn = type->get_name();
                            string tk = kt->get_type_name();
                            if (kt->get_datatype() == __type_def_enum::TYPE_STRING) {
                                tk = "std::string";
                            }
                            string tvp = vt->get_type_ptr();

                            f_str = string_utils::set_token(tk_name, tn, f_str);
                            f_str = string_utils::set_token(tk_k_type, tk, f_str);
                            f_str = string_utils::set_token(tk_v_type_ptr, tvp, f_str);

                            struct_free_calls.push_back(f_str);
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
                            vector<string> *ft = template_header.find_token(CPPT_TOKEN_INCLUDE_BASE);
                            CHECK_NOT_EMPTY_P(ft);

                            string tk_includes = CPPT_TOKEN_DEF_INCLUDES;
                            CHECK_NOT_EMPTY(tk_includes);

                            stringstream ibuff;
                            for (string str : *ft) {
                                ibuff << str << "\n";
                            }
                            string header = string(ibuff.str());
                            header = string_utils::set_token(tk_includes, includes, header);

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
