//
// Created by Subhabrata Ghosh on 21/12/16.
//

#ifndef REACTFS_CODE_GENERATOR_CPP_H
#define REACTFS_CODE_GENERATOR_CPP_H

#include "core/includes/core.h"
#include "code_generator.h"
#include "cpp_template_header.h"
#include "cpp_template_defs.h"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace parsers {
                    class code_generator_cpp : public code_generator {
                    protected:
                        cpp_template_header template_h;
                        unordered_map<string, string> headers;

                        bool is_generated(const string &name) {
                            unordered_map<string, string>::iterator iter = headers.find(name);
                            if (iter != headers.end())
                                return true;
                            return false;
                        }

                        string get_generated_header(const string &name) {
                            unordered_map<string, string>::iterator iter = headers.find(name);
                            if (iter != headers.end())
                                return iter->second;
                            return common_consts::EMPTY_STRING;
                        }

                        void check_and_generate_type(const string &name_space, __native_type *type) {
                            const string name = type->get_name();
                            if (!is_generated(name)) {
                                __complex_type *nt = dynamic_cast<__complex_type *>(type);
                                CHECK_CAST(nt, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));
                                string hf = generate_type_class(name_space, nt);
                                CHECK_NOT_EMPTY(hf);
                                headers.insert({name, hf});
                            }
                        }

                        string get_type_filename(string &type_name) {
                            return common_utils::format("%s.h", type_name.c_str());
                        }

                        string create_type_file(const string &name_space, __complex_type *type) {
                            CHECK_NOT_NULL(type);
                            string type_name = type->get_name();
                            string filename = get_type_filename(type_name);
                            Path p = Path(filename);
                            if (p.exists()) {
                                if (overwrite) {
                                    p.remove();
                                } else {
                                    LOG_DEBUG("Type file already exists. [type=%s][file=%s]", type_name.c_str(),
                                              p.get_path().c_str());
                                    return p.get_path();
                                }
                            }
                            unordered_map<uint8_t, __native_type *> fields = type->get_fields();
                            unordered_map<uint8_t, __native_type *>::iterator iter;

                            stringstream f_declares;

                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                __native_type *t = iter->second;
                                CHECK_NOT_NULL(t);
                                if (t->get_type() == __field_type::NATIVE || t->get_type() == __field_type::COMPLEX) {
                                    string str = get_native_declare(t);
                                    CHECK_NOT_EMPTY(str);
                                    f_declares << str;
                                } else if (t->get_type() == __field_type::LIST) {
                                    string str = get_list_declare(t);
                                    CHECK_NOT_EMPTY(str);
                                    f_declares << str;
                                } else if (t->get_type() == __field_type::MAP) {
                                    string str = get_map_declare(t);
                                    CHECK_NOT_EMPTY(str);
                                    f_declares << str;
                                }
                            }
                        }

                        string get_list_declare(__native_type *type) {
                            __list_type *list = dynamic_cast<__list_type *>(type);
                            __native_type *t = list->get_inner_type();
                            CHECK_NOT_NULL(t);
                            vector<string> *ft = template_h.find_token(CPPT_TOKEN_VARIABLE_LIST_DEF);
                            CHECK_NOT_EMPTY_P(ft);
                            string tk_type = CPPT_VARNAME__TYPE;
                            string tk_name = CPPT_VARNAME__NAME;
                            stringstream buff;
                            for (string ss : *ft) {
                                string str = string(ss);
                                string dt = __type_enum_helper::get_datatype(t->get_datatype());
                                CHECK_NOT_EMPTY(dt);
                                string tn = type->get_name();
                                CHECK_NOT_EMPTY(tn);

                                str = string_utils::set_token(tk_type, dt, str);
                                str = string_utils::set_token(tk_name, tn, str);

                                buff << str << "\n";
                            }
                            string str = string(buff.str());
                            TRACE("DECLARE [%s]", str.c_str());
                            return str;
                        }

                        string get_map_declare(__native_type *type) {
                            __map_type *map = dynamic_cast<__map_type *>(type);
                            __native_type *kt = map->get_key_type();
                            CHECK_NOT_NULL(kt);
                            __native_type *vt = map->get_value_type();
                            CHECK_NOT_NULL(vt);

                            vector<string> *ft = template_h.find_token(CPPT_TOKEN_VARIABLE_LIST_DEF);
                            CHECK_NOT_EMPTY_P(ft);
                            string tk_k_type = CPPT_VARNAME_KEY_TYPE;
                            string tk_v_type = CPPT_VARNAME_VALUE_TYPE;
                            string tk_name = CPPT_VARNAME__NAME;
                            stringstream buff;
                            for (string ss : *ft) {
                                string str = string(ss);
                                string kdt = __type_enum_helper::get_datatype(kt->get_datatype());
                                CHECK_NOT_EMPTY(kdt);
                                string vdt = __type_enum_helper::get_datatype(vt->get_datatype());
                                CHECK_NOT_EMPTY(vdt);
                                string tn = type->get_name();
                                CHECK_NOT_EMPTY(tn);

                                str = string_utils::set_token(tk_k_type, kdt, str);
                                str = string_utils::set_token(tk_v_type, vdt, str);
                                str = string_utils::set_token(tk_name, tn, str);

                                buff << str << "\n";
                            }
                            string str = string(buff.str());
                            TRACE("DECLARE [%s]", str.c_str());
                            return str;
                        }

                        string get_native_declare(__native_type *type) {
                            vector<string> *ft = template_h.find_token(CPPT_TOKEN_VARIABLE_NATIVE_DEF);
                            CHECK_NOT_EMPTY_P(ft);
                            string tk_type = CPPT_VARNAME__TYPE;
                            string tk_name = CPPT_VARNAME__NAME;
                            stringstream buff;
                            for (string ss : *ft) {
                                string str = string(ss);
                                string dt = __type_enum_helper::get_datatype(type->get_datatype());
                                CHECK_NOT_EMPTY(dt);
                                string tn = type->get_name();
                                CHECK_NOT_EMPTY(tn);

                                str = string_utils::set_token(tk_type, dt, str);
                                str = string_utils::set_token(tk_name, tn, str);

                                buff << str << "\n";
                            }
                            string str = string(buff.str());
                            TRACE("DECLARE [%s]", str.c_str());
                            return str;
                        }

                    public:
                        code_generator_cpp(string output_path, bool overwrite = true) {
                            CHECK_NOT_EMPTY(output_path);
                            this->outpath = new Path(output_path);
                            if (!this->outpath->exists()) {
                                this->outpath->create(DEFAULT_RESOURCE_MODE);
                            }
                            this->overwrite = overwrite;
                        }

                        virtual string generate_type_class(const string &name_space, __complex_type *type) override {
                            CHECK_NOT_NULL(type);
                            unordered_map<uint8_t, __native_type *> fields = type->get_fields();
                            unordered_map<uint8_t, __native_type *>::iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                __native_type *t = iter->second;
                                CHECK_NOT_NULL(t);
                                if (t->get_type() == __field_type::COMPLEX) {
                                    check_and_generate_type(name_space, t);
                                } else if (t->get_type() == __field_type::LIST) {
                                    __list_type *lt = dynamic_cast<__list_type *>(t);
                                    CHECK_CAST(lt, TYPE_NAME(__native_type), TYPE_NAME(__list_type));
                                    if (lt->get_inner_datatype() == __type_def_enum::TYPE_STRUCT) {
                                        __native_type *it = lt->get_inner_type();
                                        CHECK_NOT_NULL(it);
                                        check_and_generate_type(name_space, it);
                                    }
                                } else if (t->get_type() == __field_type::MAP) {
                                    __map_type *mt = dynamic_cast<__map_type *>(t);
                                    CHECK_CAST(mt, TYPE_NAME(__native_type), TYPE_NAME(__map_type));
                                    if (mt->get_value_datatype() == __type_def_enum::TYPE_STRUCT) {
                                        __native_type *it = mt->get_value_type();
                                        CHECK_NOT_NULL(it);
                                        check_and_generate_type(name_space, it);
                                    }
                                }
                            }
                            return create_type_file(name_space, type);
                        }
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_CODE_GENERATOR_CPP_H
