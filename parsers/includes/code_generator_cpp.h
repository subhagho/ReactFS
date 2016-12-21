//
// Created by Subhabrata Ghosh on 21/12/16.
//

#ifndef REACTFS_CODE_GENERATOR_CPP_H
#define REACTFS_CODE_GENERATOR_CPP_H

#include "core/includes/core.h"
#include "code_generator.h"

#define BASE_INCLUDE_NAME "types/includes/type_interface.h"
#define COMMON_INCLUDE_NAME "common/includes/common.h"
#define BASE_ERROR_INCLUDE_NAME "common/includes/base_error.h"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace parsers {
                    class code_generator_cpp : public code_generator {
                    protected:
                        string create_header_file(const string &type_name) {
                            CHECK_NOT_EMPTY(type_name);

                            Path p = Path(outpath->get_path());
                            string filename = common_utils::format("%s.h", type_name.c_str());
                            p.append(filename);
                            if (p.exists()) {
                                p.remove();
                            }

                            return p.get_path();
                        }

                        uint32_t
                        add_header_defs(vector<string> *lines, const string &type_name, const string &name_space,
                                        const string &p_class) {
                            string guard_name = common_utils::format("REACTFS_%s_CPP_H",
                                                                     string_utils::toupper(type_name).c_str());
                            // Include guard
                            string line = common_utils::format("#ifndef %s", guard_name.c_str());
                            lines->push_back(line);
                            line = common_utils::format("#define %s", guard_name.c_str());
                            lines->push_back(line);

                            // Include base definitions
                            line = common_utils::format("#include \"%s\"", COMMON_INCLUDE_NAME);
                            lines->push_back(line);
                            line = common_utils::format("#include \"%s\"", BASE_ERROR_INCLUDE_NAME);
                            lines->push_back(line);
                            line = common_utils::format("#include \"%s\"\n", BASE_INCLUDE_NAME);
                            lines->push_back(line);

                            string prefix;
                            uint8_t ns_count = 0;
                            if (!IS_EMPTY(ns)) {
                                vector<string> ns;
                                string_utils::split(name_space, '.', &ns);
                                CHECK_NOT_EMPTY(ns);

                                for (string n : ns) {
                                    if (IS_EMPTY(prefix))
                                        line = common_utils::format("namespace %s {", n.c_str());
                                    else
                                        line = common_utils::format("%snamespace %s {", prefix.c_str(), n.c_str());
                                    lines->push_back(line);
                                    ns_count++;
                                    prefix.append("\t");
                                }
                            }
                            // Define class
                            prefix.append("\t");
                            line = common_utils::format("%sclass %s : public %s {", prefix.c_str(), type_name.c_str(),
                                                        p_class.c_str());
                            lines->push_back(line);
                            uint32_t code_index = lines.size() - 1;
                            line = common_utils::format("%s};", prefix.c_str());
                            lines->push_back(line);
                            // End namespace
                            if (ns_count > 0) {
                                for (uint8_t ii = 0; ii < ns_count; ii++) {
                                    line = common_utils::format("%s}", preifx.substr(ii, ns_count).c_str());
                                    lines->push_back(line);
                                }
                            }
                            // End for include guard.
                            line = common_utils::format("#endif //%s", guard_name.c_str());
                            lines->push_back(line);

                            return code_index;
                        }

                    public:
                        code_generator_cpp(string output_path) {
                            CHECK_NOT_EMPTY(output_path);
                            this->outpath = new Path(output_path);
                            if (!this->outpath->exists()) {
                                this->outpath->create(DEFAULT_RESOURCE_MODE);
                            }
                        }

                        virtual string generate_type_class(__reference_type *type) override {

                        }

                        virtual string generate_schema_class(const string &name_space, __schema_def *schema,
                                                             unordered_map<string, __reference_type *> *types) override {
                            CHECK_NOT_NULL(schema);
                            if (!IS_EMPTY_P(types)) {
                                unordered_map<string, __reference_type *>::iterator iter;
                                for(iter = types->begin(); iter != types->end(); iter++) {

                                }
                            }
                        }
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_CODE_GENERATOR_CPP_H
