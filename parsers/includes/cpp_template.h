//
// Created by Subhabrata Ghosh on 21/12/16.
//

#ifndef REACTFS_CPP_TEMPLATE_H
#define REACTFS_CPP_TEMPLATE_H

#include <fstream>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/file_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"

#define VARNAME_LOCAL_INIT "allocated"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace parsers {
                    class cpp_variable {
                    protected:
                        string name;
                        string type;

                    public:
                        cpp_variable(const string &name, const string &type) {
                            this->name = string(name);
                            this->type = string(type);
                        }

                        virtual ~cpp_variable() {}

                        virtual string get_type() {
                            return this->type;
                        }

                        string get_name() {
                            return this->name;
                        }

                        virtual string get_return() {
                            return common_utils::format("%s *", this->type);
                        }

                        virtual void print(ofstream &ofs, const string &tab_offset) {
                            string line = common_utils::format("%s%s%s = nullptr;\n", tab_offset.c_str(),
                                                               get_return().c_str(),
                                                               name.c_str());

                            ofs << line;
                        }

                        virtual void print_getter(ofstream &ofs, const string &tab_offset) {
                            string line = common_utils::format("%sconst %s get_%s(void) {\n", tab_offset.c_str(),
                                                               this->get_return().c_str(), this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s\treturn this->%s;\n", tab_offset.c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s}\n\n", tab_offset.c_str());
                            ofs << line;
                        }

                        virtual void print_setter(ofstream &ofs, const string &tab_offset) {
                            string line = common_utils::format("%svoid set_%s(%s %s) {\n", tab_offset.c_str(),
                                                               this->name.c_str(), this->get_return().c_str(),
                                                               this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s\tPRECONDITION(%s == false);\n", tab_offset.c_str(),
                                                        VARNAME_LOCAL_INIT);
                            ofs << line;
                            line = common_utils::format("%s\tthis->%s = %s;\n", tab_offset.c_str(), this->name.c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s}\n\n", tab_offset.c_str());
                            ofs << line;

                            line = common_utils::format("%svoid set_%s(%s %s) {\n", tab_offset.c_str(),
                                                        this->name.c_str(), this->get_type().c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s\tPRECONDITION(%s == true);\n", tab_offset.c_str(),
                                                        VARNAME_LOCAL_INIT);
                            ofs << line;
                            line = common_utils::format("%s\tFREE_PTR(this->%s);\n", tab_offset.c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s\tif (NOT_NULL(%s)) {\n", tab_offset.c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s\t\tthis->%s = (%s) malloc(sizeof(%s));\n",
                                                        tab_offset.c_str(),
                                                        this->name.c_str(), this->get_return().c_str(),
                                                        this->get_type().c_str());
                            ofs << line;
                            line = common_utils::format("%s\t\tCHECK_ALLOC(this->%s, TYPE_NAME(%s));\n",
                                                        tab_offset.c_str(),
                                                        this->name.c_str(),
                                                        this->get_type().c_str());
                            ofs << line;
                            line = common_utils::format("%s\t\t*this->%s = %s;\n", tab_offset.c_str(),
                                                        this->name.c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s\t}\n", tab_offset.c_str());
                            ofs << line;
                            line = common_utils::format("%s}\n\n", tab_offset.c_str());
                            ofs << line;
                        }

                        virtual void print_init(ofstream &ofs, const string &tab_offset) {
                            string line = common_utils::format("%sthis->%s = nullptr;\n", tab_offset.c_str(),
                                                               name.c_str());

                            ofs << line;
                        }

                        virtual void print_free(ofstream &ofs, const string &tab_offset) {
                            string line = common_utils::format("%sif(%s == true) {\n", tab_offset.c_str(),
                                                               VARNAME_LOCAL_INIT);
                            ofs << line;
                            line = common_utils::format("%s\tFREE_PTR(this->%s);\n", tab_offset.c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s} else {\n", tab_offset.c_str());
                            ofs << line;
                            line = common_utils::format("%s\tthis->%s = nullptr;\n", tab_offset.c_str(),
                                                        name.c_str());

                            ofs << line;
                            line = common_utils::format("%s}\n", tab_offset.c_str());
                            ofs << line;
                        }
                    };

                    class string_variable : public cpp_variable {
                    public:
                        string_variable(const string &name) : cpp_variable(name,
                                                                           "CHARBUFF") {
                        }

                        virtual string get_type() override {
                            return common_utils::format("%s *", this->type.c_str());
                        }

                        virtual string get_return() override {
                            return this->type;
                        }

                        virtual void print_setter(ofstream &ofs, const string &tab_offset) override {
                            string line = common_utils::format("%svoid set_%s(%s %s) {\n", tab_offset.c_str(),
                                                               this->name.c_str(), this->get_return().c_str(),
                                                               this->name.c_str());
                            ofs << line;

                            line = common_utils::format("%s\tthis->%s = %s;\n", tab_offset.c_str(), this->name.c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s}\n\n", tab_offset.c_str());
                            ofs << line;

                            line = common_utils::format("%svoid set_%s(string &%s) {\n", tab_offset.c_str(),
                                                        this->name.c_str(), this->get_type().c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s\tPRECONDITION(%s == true);\n", tab_offset.c_str(),
                                                        VARNAME_LOCAL_INIT);
                            ofs << line;
                            line = common_utils::format("%s\tFREE_PTR(this->%s);\n", tab_offset.c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s\tif (NOT_NULL(%s)) {\n", tab_offset.c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s\t\tuint32_t size = %s.length() + 1;\n", tab_offset.c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s\t\tthis->%s = (%s) malloc(sizeof(%s) * size);\n",
                                                        tab_offset.c_str(),
                                                        this->name.c_str(), this->get_return().c_str(),
                                                        this->get_type().c_str());
                            ofs << line;
                            line = common_utils::format("%s\t\tCHECK_ALLOC(this->%s, TYPE_NAME(%s));\n",
                                                        tab_offset.c_str(),
                                                        this->name.c_str(),
                                                        this->get_type().c_str());
                            ofs << line;
                            line = common_utils::format("%s\t\tmemset(this->%s, 0, size);\n", tab_offset.c_str(),
                                                        this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s\t\tmemcpy(this->%s, %s, (size - 1));\n", tab_offset.c_str(),
                                                        this->name.c_str(), this->name.c_str());
                            ofs << line;
                            line = common_utils::format("%s\t}\n", tab_offset.c_str());
                            ofs << line;
                            line = common_utils::format("%s}\n\n", tab_offset.c_str());
                            ofs << line;
                        }
                    };

                    class list_variable : public cpp_variable {
                    private:
                        cpp_variable inner_type;
                    public:
                        list_variable(cpp_variable &inner_type, const string &name, const string &type) : cpp_variable(
                                name,
                                "std::vector") {
                            this->inner_type = inner_type;
                        }

                        virtual string get_type() override {
                            return common_utils::format("std::vector<%s> *", this->inner_type.get_return());
                        }

                        virtual string get_return() {
                            return common_utils::format("&%s", this->name);
                        }

                        virtual void print(ofstream &ofs, const string &tab_offset) override {
                            string line = common_utils::format("%s std::vector<%s> %s;\n", tab_offset.c_str(),
                                                               type.c_str(),
                                                               name.c_str());
                            ofs << line;
                        }
                    };

                    class map_variable : public cpp_variable {
                        string key_type;
                        string value_type;
                    public:
                        map_variable(const string &name, const string &key_type, const string &value_type)
                                : cpp_variable(name,
                                               "map") {
                            this->key_type = string(key_type);
                            this->value_type = string(value_type);
                        }

                        string get_key_type() {
                            return this->key_type;
                        }

                        string get_value_type() {
                            return this->value_type;
                        }

                        virtual string get_type() override {
                            return common_utils::format("std::unordered_map<%s, %s> *", this->key_type.c_str(),
                                                        this->value_type.c_str());
                        }

                        virtual string get_return() {
                            return common_utils::format("&%s", this->name);
                        }

                        virtual void print(ofstream &ofs, const string &tab_offset) override {
                            string line = common_utils::format("%s std::unordered_map<%s, %s> %s;\n",
                                                               tab_offset.c_str(),
                                                               key_type.c_str(), value_type.c_str(),
                                                               name.c_str());
                            ofs << line;
                        }
                    };

                    class cpp_template {

                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_CPP_TEMPLATE_H
