//
// Created by Subhabrata Ghosh on 21/12/16.
//

#ifndef REACTFS_CODE_GENERATOR_CPP_H
#define REACTFS_CODE_GENERATOR_CPP_H

#include "core/includes/core.h"

#include "code_generator.h"
#include "cpp_template_header.h"
#include "cpp_template_defs.h"
#include "cpp_file_template.h"
#include "style.h"

#define CPPT_CODE_TYPE "cpp"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace parsers {
                    class code_generator_cpp : public code_generator {
                    protected:
                        unordered_map<string, string> headers;
                        string style_opts;

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

                        void check_and_generate_type(const string &name_space, const __native_type *type,
                                                     const string &schema_name, __version_header version) {
                            const string name = type->get_name();
                            if (!is_generated(name)) {
                                const __complex_type *nt = dynamic_cast<const __complex_type *>(type);
                                CHECK_CAST(nt, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));
                                string hf = generate_type_class(name_space, nt, schema_name, version);
                                CHECK_NOT_EMPTY(hf);
                                headers.insert({name, hf});
                            }
                        }

                        string get_type_filename(string &type_name) {
                            return common_utils::format("%s.h", type_name.c_str());
                        }

                        string
                        create_type_file(const string &name_space, const __complex_type *type, const string &schema_name,
                                         __version_header version) {
                            CHECK_NOT_NULL(outpath);
                            CHECK_NOT_NULL(type);
                            string type_name = type->get_type_name();
                            string filename = get_type_filename(type_name);
                            Path p = Path(this->outpath->get_path());
                            p.append(filename);
                            if (p.exists()) {
                                if (overwrite) {
                                    p.remove();
                                } else {
                                    LOG_DEBUG("Type file already exists. [type=%s][file=%s]", type_name.c_str(),
                                              p.get_path().c_str());
                                    return p.get_path();
                                }
                            }

                            cpp_file_template cpp_template(type->get_type_name(), schema_name, name_space, version);
                            unordered_map<string, bool> processed;
                            unordered_map<uint8_t, __native_type *> fields = type->get_fields();
                            unordered_map<uint8_t, __native_type *>::iterator iter;

                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                __native_type *t = iter->second;
                                CHECK_NOT_NULL(t);
                                TRACE("Type --> [name=%s][type=%s]", t->get_name().c_str(), t->get_type_name().c_str());
                                if (t->get_type() == __field_type::NATIVE) {
                                    string str = cpp_template.get_declare(t);
                                    CHECK_NOT_EMPTY(str);
                                    cpp_template.generate_setter(t, CPPT_TOKEN_FUNC_SETTER_PTR_DEF);
                                    cpp_template.generate_getter(t, CPPT_TOKEN_FUNC_GETTER_DEF);
                                    if (t->get_datatype() == __type_def_enum::TYPE_TEXT ||
                                        t->get_datatype() == __type_def_enum::TYPE_STRING) {
                                        cpp_template.generate_setter(t, CPPT_TOKEN_FUNC_STRING_SETTER_DEF);
                                        cpp_template.add_native_copy(t, CPPT_TOKEN_COPY_CALL_STRING);
                                        cpp_template.add_native_copy_ptr(t, CPPT_TOKEN_COPY_CALL_STRING_PTR);
                                    } else {
                                        cpp_template.generate_native_setter(t);
                                        cpp_template.add_native_copy_ptr(t, CPPT_TOKEN_COPY_CALL_NATIVE_PTR);
                                        cpp_template.add_native_copy(t, CPPT_TOKEN_COPY_CALL_NATIVE);
                                    }
                                    cpp_template.generate_native_setter_from_map(t);
                                    cpp_template.generate_native_setter_to_map(t);
                                    cpp_template.generate_free_call(t, CPPT_TOKEN_VARIABLE_NATIVE_FREE);
                                    cpp_template.generate_var_free_call(t, CPPT_TOKEN_CALL_FREE_NATIVE_DATA_PTR);
                                } else if (t->get_type() == __field_type::LIST) {
                                    string str = cpp_template.get_declare(t);
                                    CHECK_NOT_EMPTY(str);

                                    __list_type *list = dynamic_cast<__list_type *>(t);
                                    CHECK_CAST(list, TYPE_NAME(__native_type), TYPE_NAME(__list_type));

                                    const __native_type *it = list->get_inner_type();
                                    CHECK_NOT_NULL(it);

                                    cpp_template.generate_getter(t, CPPT_TOKEN_FUNC_GETTER_DEF);
                                    cpp_template.generate_setter(t, CPPT_TOKEN_FUNC_SETTER_PTR_DEF);

                                    if (it->get_type() == __field_type::NATIVE) {
                                        if (it->get_datatype() == __type_def_enum::TYPE_TEXT ||
                                            it->get_datatype() == __type_def_enum::TYPE_STRING) {
                                            cpp_template.generate_list_add(t, CPPT_TOKEN_FUNC_LIST_STRING_ADD_DEF);
                                        } else {
                                            cpp_template.generate_list_add(t, CPPT_TOKEN_FUNC_LIST_NATIVE_ADD_DEF);
                                        }
                                        cpp_template.generate_native_setter_from_map(t);
                                        cpp_template.generate_native_setter_to_map(t);
                                        cpp_template.generate_free_call(t, CPPT_TOKEN_VARIABLE_LIST_NATIVE_FREE);
                                        cpp_template.generate_list_free_call(list,
                                                                             CPPT_TOKEN_CALL_FREE_NATIVE_LIST_DATA_PTR);
                                        cpp_template.add_list_copy_ptr(list, CPPT_TOKEN_COPY_CALL_NATIVE_LIST_PTR);
                                        cpp_template.add_list_copy(list, CPPT_TOKEN_COPY_CALL_NATIVE_LIST);
                                    } else if (it->get_type() == __field_type::COMPLEX) {
                                        cpp_template.generate_list_add(t, CPPT_TOKEN_FUNC_LIST_TYPE_ADD_DEF);
                                        string tn = it->get_type_name();
                                        CHECK_NOT_EMPTY(tn);
                                        unordered_map<string, bool>::iterator iter = processed.find(tn);
                                        if (iter == processed.end()) {
                                            cpp_template.generate_type_serializer(it);
                                            cpp_template.generate_type_deserializer(it);
                                            processed.insert({tn, true});
                                        }
                                        string header = get_generated_header(it->get_name());
                                        CHECK_NOT_EMPTY(header);
                                        cpp_template.add_header(header);

                                        tn = t->get_type_name();
                                        iter = processed.find(tn);
                                        if (iter == processed.end()) {
                                            cpp_template.generate_list_serde(list,
                                                                             CPPT_TOKEN_FUNC_TYPE_LIST_SERIALIZER);
                                            cpp_template.generate_list_serde(list,
                                                                             CPPT_TOKEN_FUNC_TYPE_LIST_DESERIALIZER);
                                            processed.insert({tn, true});
                                        }
                                        cpp_template.generate_type_list_setter_from_map(list);
                                        cpp_template.generate_type_list_setter_to_map(list);
                                        cpp_template.generate_free_call(t, CPPT_TOKEN_VARIABLE_LIST_TYPE_FREE);
                                        cpp_template.generate_list_free_call(list,
                                                                             CPPT_TOKEN_CALL_FREE_TYPE_LIST_DATA_PTR);
                                        cpp_template.add_list_copy_ptr(list, CPPT_TOKEN_COPY_CALL_TYPE_LIST_PTR);
                                        cpp_template.add_list_copy(list, CPPT_TOKEN_COPY_CALL_TYPE_LIST);
                                    }
                                } else if (t->get_type() == __field_type::MAP) {
                                    string str = cpp_template.get_declare(t);
                                    CHECK_NOT_EMPTY(str);

                                    __map_type *map = dynamic_cast<__map_type *>(t);
                                    CHECK_CAST(map, TYPE_NAME(__native_type), TYPE_NAME(__map_type));

                                    const __native_type *vt = map->get_value_type();
                                    CHECK_NOT_NULL(vt);

                                    cpp_template.generate_getter(t, CPPT_TOKEN_FUNC_GETTER_DEF);
                                    cpp_template.generate_setter(t, CPPT_TOKEN_FUNC_SETTER_PTR_DEF);

                                    if (vt->get_type() == __field_type::NATIVE) {
                                        if (vt->get_datatype() == __type_def_enum::TYPE_TEXT ||
                                            vt->get_datatype() == __type_def_enum::TYPE_STRING) {
                                            cpp_template.generate_map_add(t, CPPT_TOKEN_FUNC_MAP_STRING_ADD_DEF);
                                        } else {
                                            cpp_template.generate_map_add(t, CPPT_TOKEN_FUNC_MAP_NATIVE_ADD_DEF);
                                        }
                                        cpp_template.generate_native_setter_from_map(t);
                                        cpp_template.generate_native_setter_to_map(t);
                                        cpp_template.generate_free_call(t, CPPT_TOKEN_VARIABLE_MAP_NATIVE_FREE);
                                        cpp_template.generate_map_free_call(map,
                                                                            CPPT_TOKEN_CALL_FREE_NATIVE_MAP_DATA_PTR);
                                        cpp_template.add_map_copy_ptr(map, CPPT_TOKEN_COPY_CALL_NATIVE_MAP_PTR);
                                        cpp_template.add_map_copy(map, CPPT_TOKEN_COPY_CALL_NATIVE_MAP);
                                    } else if (vt->get_type() == __field_type::COMPLEX) {
                                        cpp_template.generate_map_add(t, CPPT_TOKEN_FUNC_MAP_TYPE_ADD_DEF);
                                        string tn = vt->get_type_name();
                                        CHECK_NOT_EMPTY(tn);
                                        unordered_map<string, bool>::iterator iter = processed.find(tn);
                                        if (iter == processed.end()) {
                                            cpp_template.generate_type_serializer(vt);
                                            cpp_template.generate_type_deserializer(vt);
                                            processed.insert({tn, true});
                                        }
                                        string header = get_generated_header(vt->get_name());
                                        CHECK_NOT_EMPTY(header);
                                        cpp_template.add_header(header);
                                        tn = t->get_type_name();
                                        iter = processed.find(tn);
                                        if (iter == processed.end()) {
                                            cpp_template.generate_map_serde(map,
                                                                            CPPT_TOKEN_FUNC_TYPE_MAP_SERIALIZER);
                                            cpp_template.generate_map_serde(map,
                                                                            CPPT_TOKEN_FUNC_TYPE_MAP_DESERIALIZER);
                                            processed.insert({tn, true});
                                        }
                                        cpp_template.generate_type_map_setter_from_map(map);
                                        cpp_template.generate_type_map_setter_to_map(map);
                                        cpp_template.generate_free_call(t, CPPT_TOKEN_VARIABLE_MAP_TYPE_FREE);
                                        cpp_template.generate_map_free_call(map,
                                                                            CPPT_TOKEN_CALL_FREE_TYPE_MAP_DATA_PTR);
                                        cpp_template.add_map_copy_ptr(map, CPPT_TOKEN_COPY_CALL_TYPE_MAP_PTR);
                                        cpp_template.add_map_copy(map, CPPT_TOKEN_COPY_CALL_TYPE_MAP);
                                    }
                                } else if (t->get_type() == __field_type::COMPLEX) {
                                    string str = cpp_template.get_declare(t);
                                    CHECK_NOT_EMPTY(str);

                                    cpp_template.generate_getter(t, CPPT_TOKEN_FUNC_GETTER_DEF);
                                    cpp_template.generate_setter(t, CPPT_TOKEN_FUNC_SETTER_PTR_DEF);
                                    string tn = t->get_type_name();
                                    CHECK_NOT_EMPTY(tn);
                                    unordered_map<string, bool>::iterator iter = processed.find(tn);
                                    if (iter == processed.end()) {
                                        cpp_template.generate_type_serializer(t);
                                        cpp_template.generate_type_deserializer(t);
                                        processed.insert({tn, true});
                                    }
                                    string header = get_generated_header(t->get_name());
                                    CHECK_NOT_EMPTY(header);
                                    cpp_template.add_header(header);

                                    __complex_type *ct = dynamic_cast<__complex_type *>(t);
                                    CHECK_CAST(ct, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));
                                    cpp_template.generate_type_setter_from_map(ct);
                                    cpp_template.generate_type_setter_to_map(ct);
                                    cpp_template.generate_free_call(t, CPPT_TOKEN_VARIABLE_TYPE_FREE);
                                    cpp_template.generate_type_free_call(t, CPPT_TOKEN_CALL_FREE_TYPE_DATA_PTR);
                                    cpp_template.add_type_copy(t);
                                    cpp_template.add_type_copy_ptr(t);
                                }
                            }
                            string str = cpp_template.finish();
                            CHECK_NOT_EMPTY(str);

                            std::ofstream outfs(p.get_path());
                            outfs << str << "\n";
                            outfs.flush();
                            outfs.close();

                            styler style(this->style_opts);
                            style.style(p.get_path());

                            LOG_DEBUG("Written header file for class. [class=%s][file=%s]",
                                      cpp_template.get_class_name().c_str(), p.get_path().c_str());
                            return filename;
                        }

                    public:
                        code_generator_cpp(string output_path, const string &style_opts, bool overwrite = true) {
                            CHECK_NOT_EMPTY(output_path);
                            this->outpath = new Path(output_path);
                            if (!this->outpath->exists()) {
                                this->outpath->create(DEFAULT_RESOURCE_MODE);
                            }
                            this->overwrite = overwrite;
                            this->style_opts = string(style_opts);
                        }

                        virtual string
                        generate_type_class(const string &name_space, const __complex_type *type, const string &schema_name,
                                            __version_header version) override {
                            CHECK_NOT_NULL(type);
                            unordered_map<uint8_t, __native_type *> fields = type->get_fields();
                            unordered_map<uint8_t, __native_type *>::const_iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                __native_type *t = iter->second;
                                CHECK_NOT_NULL(t);
                                if (t->get_type() == __field_type::COMPLEX) {
                                    check_and_generate_type(name_space, t, schema_name, version);
                                } else if (t->get_type() == __field_type::LIST) {
                                    __list_type *lt = dynamic_cast<__list_type *>(t);
                                    CHECK_CAST(lt, TYPE_NAME(__native_type), TYPE_NAME(__list_type));
                                    if (lt->get_inner_datatype() == __type_def_enum::TYPE_STRUCT) {
                                        const __native_type *it = lt->get_inner_type();
                                        CHECK_NOT_NULL(it);
                                        check_and_generate_type(name_space, it, schema_name, version);
                                    }
                                } else if (t->get_type() == __field_type::MAP) {
                                    __map_type *mt = dynamic_cast<__map_type *>(t);
                                    CHECK_CAST(mt, TYPE_NAME(__native_type), TYPE_NAME(__map_type));
                                    if (mt->get_value_datatype() == __type_def_enum::TYPE_STRUCT) {
                                        const __native_type *it = mt->get_value_type();
                                        CHECK_NOT_NULL(it);
                                        check_and_generate_type(name_space, it, schema_name, version);
                                    }
                                }
                            }
                            return create_type_file(name_space, type, schema_name, version);
                        }
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_CODE_GENERATOR_CPP_H
