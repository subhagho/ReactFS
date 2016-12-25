//
// Created by Subhabrata Ghosh on 25/12/16.
//

#ifndef REACTFS_CPP_FILE_TEMPLATE_H
#define REACTFS_CPP_FILE_TEMPLATE_H

#include "core/includes/core.h"
#include "code_generator.h"
#include "cpp_template_header.h"
#include "cpp_template_defs.h"

#define CPPT_CLASS_PARENT_TYPE "com::wookler::reactfs::types::__base_type"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
    namespace parsers {
        class cpp_file_template {
        private:
            string classname;
            string file_str;
            string class_str;
            stringstream declare_s;
            stringstream pr_methods_s;
            stringstream pu_methods_s;

            void init_file_template(const string &classname, const string &schema_name, __version_header &version) {
                CHECK_NOT_EMPTY(classname);

                stringstream stream;
                vector<string> *ft = template_h.find_token(CPPT_TOKEN_FILE_COMMENT);
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

                ft = template_h.find_token(CPPT_TOKEN_FILE_DEF);
                CHECK_NOT_EMPTY_P(ft);
                for( string str : *ft) {
                    string ss = string(str);
                    ss = string_utils::set_token(tk_schema, schema_name, ss);
                    ss = string_utils::set_token(tk_date, date_s, ss);
                    ss = string_utils::set_token(tk_username, uname, ss);
                    ss = string_utils::set_token(tk_version, version_s, ss);

                    stream << ss << "\n";
                }

                string guard_name = string_utils::toupper(classname);
                guard_name = common_utils::format("%s_H", guard_name.c_str());

                string tk_guard = CPPT_TOKEN_DEF_GUARD_NAME;
                CHECK_NOT_EMPTY(tk_guard);

                for( string str : *ft) {
                    string vs = string(str);
                    vs = string_utils::set_token(tk_guard, guard_name, vs);

                    stream << vs << "\n";
                }
                this->file_str = string(stream.str());
                this->classname = string(classname);
            }

            void init_class_template() {
                stringstream stream;
                vector<string> *ft = template_h.find_token(CPPT_TOKEN_CLASS_DEF);
                CHECK_NOT_EMPTY_P(ft);

                string tk_class = CPPT_TOKEN_DEF_NAME;
                string tk_parent = CPPT_TOKEN_DEF_PARENT;

                for( string str : *ft) {
                    string vs = string(str);
                    vs = string_utils::set_token(tk_class, this->classname, vs);
                    vs = string_utils::set_token(tk_parent, CPPT_CLASS_PARENT_TYPE, vs);

                    stream << vs << "\n";
                }
                this->class_str = string(stream.str());
            }
        public:
            cpp_file_template(const string &classname, const string &schema_name, __version_header &version) {
                this->init_file_template(classname, schema_name, version);
                this->init_class_template();
            }

            void add_declare(const string &declare) {
                CHECK_NOT_EMPTY(declare);
                declare_s << declare << "\n";
            }

            void add_private_method(const string &method) {
                CHECK_NOT_EMPTY(method);
                pr_methods_s << method << "\n";
            }

            void add_publi_method(const string &method) {
                CHECK_NOT_EMPTY(method);
                pu_methods_s << method << "\n";
            }
        };
    }
REACTFS_NS_CORE_END
#endif //REACTFS_CPP_FILE_TEMPLATE_H
