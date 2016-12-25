//
// Created by Subhabrata Ghosh on 21/12/16.
//

#ifndef REACTFS_CPP_TEMPLATE_H
#define REACTFS_CPP_TEMPLATE_H

#include <fstream>
#include <sstream>
#include <unordered_map>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/file_utils.h"
#include "common/includes/base_error.h"
#include "common/includes/log_utils.h"
#include "core/includes/core.h"
#include "parsers/includes/cpp_template_defs.h"

#define CPPT_TOKEN_PREFIX "CPPT_TOKEN_"
#define CPPT_TOKEN_DEF_PREFIX "CPPT_TOKEN_DEF_"
#define CPPT_TOKEN_START "${"
#define CPPT_TOKEN_END "}"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace parsers {
                    class cpp_template {
                    private:
                        unordered_map<string, vector<string> *> __cpp_template;
                        unordered_map<string, string> __tokens;

                        bool is_comment_end(string &str) {
                            return (str == CPPT_COMMENT_END);
                        }

                        bool is_comment_start(string &str) {
                            return (str == CPPT_COMMENT_START);
                        }

                        bool is_declare_start(string &line) {
                            return (line.back() == CPPT_DECLARE_KEY[0]);
                        }

                        bool is_declare_end(string &line) {
                            return (line == CPPT_DECLARE_END);
                        }

                        string get_next_token(string &input, uint32_t *offset) {
                            string ts(CPPT_TOKEN_START);
                            string te(CPPT_TOKEN_END);

                            size_t index = input.find(ts, *offset);
                            if (index != string::npos) {
                                size_t endi = input.find(te, index);
                                if (endi == string::npos) {
                                    throw BASE_ERROR("Invalid token definition. Token terminator missing.");
                                }
                                string::size_type len = (endi - index + 1);
                                string token = input.substr(index, len);
                                CHECK_NOT_EMPTY(token);
                                if (!is_token_defined(token)) {
                                    throw BASE_ERROR("Missing token definition. [token=%s]", token.c_str());
                                }
                                *offset = endi + 1;
                                return token;
                            }
                            return common_consts::EMPTY_STRING;
                        }

                        void extract_tokens(string &input) {
                            uint32_t offset = 0;
                            while (offset < input.length()) {
                                string token = get_next_token(input, &offset);
                                if (IS_EMPTY(token)) {
                                    break;
                                }
                                TRACE("Found token [%s]", token.c_str());
                            }
                        }

                        bool is_token_defined(string &token) {
                            unordered_map<string, string>::iterator iter = __tokens.find(token);
                            if (iter != __tokens.end())
                                return true;
                            return false;
                        }

                        void write_includes(std::ofstream &outf) {
                            short size = 6;
                            string includes[] = {"#include <unordered_map>",
                                                 "#include \"common/includes/common.h\"",
                                                 "#include \"common/includes/common_utils.h\"",
                                                 "#include \"common/includes/base_error.h\"",
                                                 "#include \"common/includes/log_utils.h\"",
                                                 "#include \"core/includes/core.h\""};

                            for (int ii = 0; ii < size; ii++) {
                                outf << common_utils::format("%s\n", includes[ii].c_str());
                            }
                            outf << "\n";
                        }

                        void write_init_func(std::ofstream &outf, string tab, string map_name) {
                            tab.append("\t");
                            outf << common_utils::format("%svoid __init() {\n", tab.c_str());
                            string v_name = "values";
                            outf << common_utils::format("%s\tstd::vector<string> *%s = nullptr;\n\n", tab.c_str(),
                                                         v_name.c_str());

                            unordered_map<string, vector<string> *>::iterator iter;
                            for (iter = __cpp_template.begin(); iter != __cpp_template.end(); iter++) {
                                const string key = iter->first;
                                vector<string> *values = iter->second;
                                CHECK_NOT_EMPTY_P(values);
                                outf << common_utils::format("%s\t// KEY [%s]\n", tab.c_str(), key.c_str());
                                outf << common_utils::format("%s\t%s = new std::vector<string>();\n", tab.c_str(),
                                                             v_name.c_str());
                                outf << common_utils::format("%s\tCHECK_ALLOC(%s, TYPE_NAME(vector));\n", tab.c_str(),
                                                             v_name.c_str());
                                outf << common_utils::format("%s\t%s.insert({\"%s\", %s});\n", tab.c_str(),
                                                             map_name.c_str(), key.c_str(), v_name.c_str());
                                for (string v : *values) {
                                    outf << common_utils::format("%s\t%s->push_back(\"%s\");\n", tab.c_str(),
                                                                 v_name.c_str(), v.c_str());
                                }
                                outf << common_utils::format("%s\t// END KEY [%s]\n", tab.c_str(), key.c_str());
                                outf << "\n";
                            }

                            outf << common_utils::format("%s}\n\n", tab.c_str());
                        }

                        void write_constructor(std::ofstream &outf, string tab) {
                            tab.append("\t");
                            outf << common_utils::format("%s%s() {\n", tab.c_str(), CPPT_TEMPLATE_CLASSNAME);
                            outf << common_utils::format("%s\tthis->__init();\n", tab.c_str());
                            outf << common_utils::format("%s}\n\n", tab.c_str());
                        }

                        void write_desructor(std::ofstream &outf, string tab, string map_name) {
                            tab.append("\t");
                            outf << common_utils::format("%s~%s() {\n", tab.c_str(), CPPT_TEMPLATE_CLASSNAME);

                            outf << common_utils::format(
                                    "%s\tunordered_map<string, vector<string> *>::iterator iter;\n", tab.c_str());
                            outf << common_utils::format("%s\tfor (iter = %s.begin(); iter != %s.end(); iter++) {\n",
                                                         tab.c_str(), map_name.c_str(), map_name.c_str());
                            outf << common_utils::format("%s\t\tCHECK_AND_FREE(iter->second);", tab.c_str());
                            outf << common_utils::format("%s\t}\n", tab.c_str());

                            outf << common_utils::format("%s}\n\n", tab.c_str());
                        }

                        void write_getter(std::ofstream &outf, string tab, string map_name) {
                            tab.append("\t");
                            string k_name = "token";

                            outf << common_utils::format("%sstd::vector<string> * find_%s(string %s) {\n", tab.c_str(),
                                                         k_name.c_str(), k_name.c_str());
                            outf << common_utils::format(
                                    "%s\tunordered_map<string, vector<string> *>::iterator iter = %s.find(%s);\n",
                                    tab.c_str(), map_name.c_str(), k_name.c_str());
                            outf << common_utils::format("%s\tif (iter != %s.end())\n", tab.c_str(), map_name.c_str());
                            outf << common_utils::format("%s\t\treturn iter->second;\n", tab.c_str());
                            outf << common_utils::format("%s\treturn nullptr;\n", tab.c_str());
                            outf << common_utils::format("%s}\n\n", tab.c_str());
                        }

                        void write_class(std::ofstream &outf) {
                            string tab = "\t\t\t\t\t";
                            outf << common_utils::format("%sclass %s {\n", tab.c_str(), CPPT_TEMPLATE_CLASSNAME);

                            string map_name = "__cpp_template";
                            outf << common_utils::format("%sprivate:\n", tab.c_str());
                            outf << common_utils::format("%s\tunordered_map<string, vector<string> *> %s;\n\n",
                                                         tab.c_str(),
                                                         map_name.c_str());
                            write_init_func(outf, tab, map_name);
                            outf << common_utils::format("%spublic:\n", tab.c_str());
                            write_constructor(outf, tab);
                            write_desructor(outf, tab, map_name);
                            write_getter(outf, tab, map_name);
                            outf << common_utils::format("%s};\n", tab.c_str());
                        }

                        void write_template_class(std::ofstream &outf) {
                            write_includes(outf);
                            outf << common_utils::format("%s\n", "using namespace REACTFS_NS_COMMON_PREFIX;");
                            outf << common_utils::format("%s\n", "REACTFS_NS_CORE");
                            outf << common_utils::format("%s\t\t\n", "namespace parsers {");
                            write_class(outf);
                            outf << common_utils::format("\t\t}\n%s\n", "REACTFS_NS_CORE_END");
                        }

                        void write_defines(std::ofstream &outf) {
                            unordered_map<string, vector<string> *>::iterator iter;
                            for (iter = __cpp_template.begin(); iter != __cpp_template.end(); iter++) {
                                const string key = iter->first;
                                outf << common_utils::format("#define %s%s \"%s\"\n", CPPT_TOKEN_PREFIX, key.c_str(),
                                                             key.c_str());
                            }
                            unordered_map<string, string>::iterator titer;
                            for (titer = __tokens.begin(); titer != __tokens.end(); titer++) {
                                string tn = string_utils::toupper(titer->second);
                                uint16_t len = tn.length() - 3;
                                tn = tn.substr(2, len);
                                outf << common_utils::format("#define %s%s \"%s\"\n", CPPT_TOKEN_DEF_PREFIX, tn.c_str(),
                                                             titer->first.c_str());
                            }
                        }

                    public:
                        void read_template(string filename) {
                            std::ifstream tfile(filename);
                            std::string line;

                            bool in_declare = false;
                            string token;
                            bool in_comment = false;

                            while (std::getline(tfile, line)) {
                                if (in_comment) {
                                    line = string_utils::trim(line);
                                    if (is_comment_end(line)) {
                                        in_comment = false;
                                        continue;
                                    }
                                    continue;
                                }
                                if (!in_declare) {
                                    if (IS_EMPTY(line)) {
                                        continue;
                                    }
                                    line = string_utils::trim(line);
                                    if (is_comment_start(line)) {
                                        in_comment = true;
                                        continue;
                                    }
                                    if (is_declare_start(line)) {
                                        size_t index = line.find(CPPT_DECLARE_KEY);
                                        POSTCONDITION(index > 0);
                                        token = line.substr(0, index);
                                        CHECK_NOT_EMPTY(token);
                                        token = string_utils::trim(token);

                                        vector<string> *v = new vector<string>();
                                        CHECK_ALLOC(v, TYPE_NAME(vector));

                                        __cpp_template.insert({token, v});
                                        in_declare = true;

                                        LOG_DEBUG("Read token : %s", token.c_str());

                                        continue;
                                    }
                                    size_t index = line.find(CPPT_TOKEN_KEY);
                                    if (index != string::npos) {
                                        index += strlen(CPPT_TOKEN_KEY);
                                        uint16_t len = (line.length() - index);
                                        string str = line.substr(index, len);
                                        CHECK_NOT_EMPTY(str);
                                        str = string_utils::trim(str);
                                        if (!is_token_defined(str)) {
                                            __tokens.insert({str, str});
                                        }
                                        continue;
                                    }
                                } else {
                                    string str = string(line);
                                    str = string_utils::trim(str);
                                    if (!IS_EMPTY(str)) {
                                        if (is_declare_end(str)) {
                                            in_declare = false;
                                            continue;
                                        }
                                    }

                                    line = string_utils::escape(line);
                                    LOG_DEBUG("LINE [%s]", line.c_str());
                                    CHECK_NOT_EMPTY(token);
                                    extract_tokens(line);
                                    unordered_map<string, vector<string> *>::iterator iter = __cpp_template.find(token);
                                    POSTCONDITION(iter != __cpp_template.end());
                                    iter->second->push_back(line);
                                }
                            }
                        }

                        void write_template_header(string outdir) {
                            CHECK_NOT_EMPTY(__cpp_template);
                            Path p = Path(outdir);
                            PRECONDITION(p.exists());
                            p.append(CPPT_HEADER_FILENAME);
                            if (p.exists()) {
                                p.remove();
                            }

                            std::ofstream outf(p.get_path());
                            outf << common_utils::format("#ifndef %s\n", CPPT_HEADER_GUARD);
                            outf << common_utils::format("#define %s\n", CPPT_HEADER_GUARD);
                            write_template_class(outf);
                            write_defines(outf);
                            outf << common_utils::format("#endif // %s\n", CPPT_HEADER_GUARD);
                        }
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_CPP_TEMPLATE_H
