/* 
 * Copyright [2016] [Subhabrata Ghosh] 
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 *       limitations under the License. 
 * 
 */
#ifndef _REACTFS_COMMON_CONFIG_H_
#define _REACTFS_COMMON_CONFIG_H_

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <time.h>
#include <regex>
#include <iostream>

#include "common/src/json11/json.h"

#include "common.h"
#include "common_utils.h"
#include "__state__.h"
#include "base_error.h"

#define CONST_CONFIG_ERROR_PREFIX "Configuration Error : "
#define CONST_CONFIG_ROOT_NAME "configuration"
#define CONST_CONFIG_NODE_PARAM "params"
#define CONST_CONFIG_PATH_PARAM "./params"

#define CONFIG_ERROR(fmt, ...) config_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))
#define CONFIG_ERROR_PTR(fmt, ...) new config_error(__FILE__, __LINE__, common_utils::format(fmt, ##__VA_ARGS__))

#define ERROR_MISSING_CONFIG(name) CONFIG_ERROR("Missing configuration parameter. [name=%s]", name)
#define ERROR_MISSING_CONFIG_NODE(name) CONFIG_ERROR("Missing configuration node. [name=%s]", name)

using namespace json11;

REACTFS_NS_COMMON
                class config_error : public base_error {
                public:
                    config_error(char const *file, const int line, string mesg) : base_error(file, line,
                                                                                             CONST_CONFIG_ERROR_PREFIX,
                                                                                             mesg) {
                    }

                };


                enum ConfigValueTypeEnum {
                    Basic, Params, List, Node
                };

                class ConfigValue {
                private:
                    string key;
                    ConfigValue *parent;

                protected:
                    ConfigValueTypeEnum type = Basic;

                    string *parse_key_name(const string key) const {
                        regex r("(.*)(\\[.*\\])");
                        smatch m;

                        string k(key);
                        if (regex_search(k, m, r)) {
                            string *s = new string(m[1]);
                            CHECK_ALLOC(s, TYPE_NAME(string));
                            if (is_self(*s)) {
                                s = new string(this->key);
                                CHECK_ALLOC(s, TYPE_NAME(string));
                            }
                            return s;
                        } else if (is_self(key)) {
                            return new string(this->key);
                        }
                        return new string(key);
                    }

                    string *parse_key_index(const string key) const {
                        regex r("(.*)(\\[(.*)\\])");
                        smatch m;

                        string s(key);
                        if (regex_search(s, m, r)) {
                            return new string(m[3]);
                        }

                        return nullptr;
                    }

                    bool is_self(string node) const {
                        char c0 = node[0];

                        if (c0 == '.') {
                            if (node.length() > 1) {
                                char c1 = node[1];
                                if (c1 == '[' || c1 == '/') {
                                    return true;
                                }
                            } else {
                                return true;
                            }
                        }
                        return false;
                    }


                public:
                    ConfigValue(string key, ConfigValue *parent) {
                        this->key = key;
                        this->parent = parent;
                    }

                    virtual ~ConfigValue() {}

                    const string get_key() const {
                        return this->key;
                    }

                    const ConfigValueTypeEnum get_type() const {
                        return type;
                    }

                    const ConfigValue *find(string path) const {
                        vector<string> parts;
                        string_utils::split(path, '/', &parts);
                        if (!IS_EMPTY(parts)) {
                            int index = 0;
                            if (IS_EMPTY(parts[0])) {
                                if (is_root()) {
                                    index = 1;
                                } else {
                                    const ConfigValue *root = find_root();
                                    return root->find(path);
                                }
                            } else {
                                string *key = parse_key_name(parts[0]);
                                if (NOT_EMPTY_P(key) && *key != "." && *key != this->key) {
                                    parts.insert(parts.begin(), ".");
                                }
                                CHECK_AND_FREE(key);
                            }
                            return find(parts, index);
                        }
                        return nullptr;
                    }

                    bool is_root() const {
                        return (parent == nullptr);
                    }

                    const ConfigValue *find_root() const {
                        if (is_root()) {
                            return this;
                        } else {
                            return parent->find_root();
                        }
                    }

                    virtual const ConfigValue *find(vector<string> path, uint32_t offset) const = 0;

                    virtual const void print(string offset) const = 0;
                };


                class BasicConfigValue : public ConfigValue {
                private:
                    string *value;

                public:
                    BasicConfigValue(string key, ConfigValue *parent) : ConfigValue(key, parent) {
                        type = Basic;
                    }

                    ~BasicConfigValue() override {
                        CHECK_AND_FREE(value);
                    }

                    const void print(string offset) const override {
                        if (NOT_EMPTY_P(value)) {
                            cout << offset << "{KEY=" << get_key() << ", VALUE=" << *value << "}\n";
                        }
                    }

                    void set_value(string value) {
                        this->value = new string(value);
                        CHECK_ALLOC(this->value, TYPE_NAME(string));
                    }

                    const string get_value() const {
                        return *this->value;
                    }

                    const short get_short_value(short def_value) const {
                        if (NOT_EMPTY_P(value)) {
                            int sv = stoi(*value);
                            if (sv >= numeric_limits<short>::lowest() && sv <= numeric_limits<short>::max()) {
                                return (short) sv;
                            }
                        }
                        return def_value;
                    }

                    const int get_int_value(int def_value) const {
                        if (NOT_EMPTY_P(value)) {
                            return stoi(*value);
                        }
                        return def_value;
                    }

                    const long get_long_value(long def_value) const {
                        if (NOT_EMPTY_P(value)) {
                            return stol(*value);
                        }
                        return def_value;
                    }

                    const float get_float_value(float def_value) const {
                        if (NOT_EMPTY_P(value)) {
                            return stof(*value);
                        }
                        return def_value;
                    }

                    const double get_double_value(double def_value) const {
                        if (NOT_EMPTY_P(value)) {
                            return stod(*value);
                        }
                        return def_value;
                    }

                    const bool get_bool_value(bool def_value) const {
                        if (NOT_EMPTY_P(value)) {
                            transform(value->begin(), value->end(), value->begin(), ::tolower);
                            istringstream is(*value);
                            bool b;
                            is >> boolalpha >> b;
                            return b;
                        }
                        return def_value;
                    }

                    const time_t get_time_value(string format, time_t def_value) const {
                        PRECONDITION(!IS_EMPTY(format));

                        if (NOT_EMPTY_P(value)) {
                            time_t t = common_utils::get_time(*value, format);
                            return (t > 0 ? t : def_value);
                        }

                        return def_value;
                    }

                    const time_t get_time_value(time_t def_value) const {
                        return get_time_value(common_consts::DEFAULT_DATE_FORMAT, def_value);
                    }

                    const ConfigValue *find(vector<string> path, uint32_t offset) const override {
                        string key = path[offset];
                        if (offset == path.size() - 1) {
                            if (this->get_key() == key) {
                                return this;
                            }
                        }
                        return nullptr;
                    }
                };

                class ParamConfigValue : public ConfigValue {
                private:
                    unordered_map<string, ConfigValue *> params;


                public:
                    ParamConfigValue(string key, ConfigValue *parent) : ConfigValue(key, parent) {
                        type = Params;
                    }

                    ~ParamConfigValue() override {
                        for (auto kv : params) {
                            CHECK_AND_FREE(kv.second);
                        }
                        params.clear();
                    }

                    const void print(string offset) const override {
                        if (!IS_EMPTY(params)) {
                            cout << offset << "params : [\n";
                            unordered_map<string, ConfigValue *>::const_iterator iter;
                            for (iter = params.begin(); iter != params.end(); iter++) {
                                ConfigValue *v = iter->second;
                                if (!IS_NULL(v)) {
                                    string off = string(offset);
                                    off.append("\t");
                                    v->print(off);
                                }
                            }
                            cout << offset << "]\n";
                        }
                    }

                    void put(string key, string value, ConfigValue *parent) {
                        PRECONDITION(!IS_EMPTY(key));
                        PRECONDITION(!IS_EMPTY(value));

                        BasicConfigValue *v = new BasicConfigValue(key, parent);
                        CHECK_ALLOC(v, TYPE_NAME(BasicConfigValue));

                        v->set_value(value);

                        params.insert(make_pair(key, v));
                    }

                    void put(ConfigValue *value) {
                        PRECONDITION(!IS_NULL(value));

                        string key = value->get_key();
                        params.insert(make_pair(key, value));
                    }

                    const ConfigValue *get(string key) const {
                        PRECONDITION(!IS_EMPTY(key));

                        unordered_map<string, ConfigValue *>::const_iterator got = params.find(key);
                        if (got != params.end())
                            return got->second;

                        return nullptr;
                    }

                    const string get_string(string key) const {
                        const ConfigValue *ptr = get(key);
                        if (NOT_NULL(ptr)) {
                            const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(ptr);

                            return bv->get_value();
                        }
                        return common_consts::EMPTY_STRING;
                    }

                    const int get_int(string key, int def_value) const {
                        const ConfigValue *ptr = get(key);
                        if (NOT_NULL(ptr)) {
                            const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(ptr);

                            return bv->get_int_value(def_value);
                        }
                        return def_value;
                    }

                    const long get_long(string key, long def_value) const {
                        const ConfigValue *ptr = get(key);
                        if (NOT_NULL(ptr)) {
                            const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(ptr);

                            return bv->get_long_value(def_value);
                        }
                        return def_value;
                    }

                    const float get_float(string key, float def_value) const {
                        const ConfigValue *ptr = get(key);
                        if (NOT_NULL(ptr)) {
                            const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(ptr);

                            return bv->get_float_value(def_value);
                        }
                        return def_value;
                    }

                    const double get_double(string key, double def_value) const {
                        const ConfigValue *ptr = get(key);
                        if (NOT_NULL(ptr)) {
                            const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(ptr);

                            return bv->get_double_value(def_value);
                        }
                        return def_value;
                    }

                    const bool get_bool(string key, bool def_value) const {
                        const ConfigValue *ptr = get(key);
                        if (NOT_NULL(ptr)) {
                            const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(ptr);

                            return bv->get_bool_value(def_value);
                        }
                        return def_value;
                    }

                    const time_t get_time(string key, string fmt, time_t def_value) const {
                        const ConfigValue *ptr = get(key);
                        if (NOT_NULL(ptr)) {
                            const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(ptr);

                            return bv->get_time_value(fmt, def_value);
                        }
                        return def_value;
                    }

                    const time_t get_time(string key, time_t def_value) const {
                        return get_time(key, common_consts::DEFAULT_DATE_FORMAT, def_value);
                    }

                    const ConfigValue *find(vector<string> path, uint32_t offset) const override {
                        if (offset == path.size() - 1) {
                            string *key = parse_key_name(path[offset]);
                            if (this->get_key() == *key) {
                                string *index = parse_key_index(path[offset]);
                                if (IS_NULL(index)) {
                                    return this;
                                } else {
                                    unordered_map<string, ConfigValue *>::const_iterator got = params.find(*index);
                                    if (got != params.end()) {
                                        return got->second;
                                    }
                                }
                                CHECK_AND_FREE(index);
                            }
                            CHECK_AND_FREE(key);
                        }
                        return nullptr;
                    }

                };

                class ListConfigValue : public ConfigValue {
                private:
                    vector<ConfigValue *> values;


                public:
                    ListConfigValue(string key, ConfigValue *parent) : ConfigValue(key, parent) {
                        type = List;
                    }

                    ~ListConfigValue() override {
                        for (auto v : values) {
                            CHECK_AND_FREE(v);
                        }
                        values.clear();
                    }

                    const void print(string offset) const override {
                        if (!IS_EMPTY(values)) {
                            cout << offset << get_key() << ": [\n";
                            for (uint32_t ii = 0; ii < values.size(); ii++) {
                                ConfigValue *v = values[ii];
                                if (!IS_NULL(v)) {
                                    string off = string(offset);
                                    off.append("\t");
                                    v->print(off);
                                }
                            }
                            cout << offset << "]\n";
                        }
                    }

                    void add_value(ConfigValue *value) {
                        PRECONDITION(NOT_NULL(value));

                        values.push_back(value);
                    }

                    const vector<ConfigValue *> get_values() const {
                        return this->values;
                    }

                    const ConfigValue *find(vector<string> path, uint32_t offset) const override {
                        string *key = parse_key_name(path[offset]);
                        if (get_key() == *key) {
                            if (offset == path.size() - 1) {
                                string *index = parse_key_index(path[offset]);
                                if (NOT_EMPTY_P(index)) {
                                    uint32_t ii = stoi(*index);
                                    if (ii > 0 && ii < values.size()) {
                                        return values[ii];
                                    }
                                } else
                                    return this;
                                CHECK_AND_FREE(index);
                            } else {
                                string *index = parse_key_index(path[offset]);
                                if (NOT_EMPTY_P(index)) {
                                    uint32_t ii = stoi(*index);
                                    if (ii > 0 && ii < values.size()) {
                                        ConfigValue *cv = values[ii];
                                        return cv->find(path, offset);
                                    }
                                }
                                CHECK_AND_FREE(index);
                            }
                        }
                        CHECK_AND_FREE(key);
                        return nullptr;
                    }

                };

                class NodeConfigValue : public ConfigValue {
                private:
                    unordered_map<string, ConfigValue *> children;


                public:
                    NodeConfigValue(string key, ConfigValue *parent) : ConfigValue(key, parent) {
                        type = Node;
                    }

                    ~NodeConfigValue() override {
                        for (auto kv : children) {
                            CHECK_AND_FREE(kv.second);
                        }
                        children.clear();
                    }

                    const void print(string offset) const override {
                        if (!IS_EMPTY(children)) {
                            cout << offset << get_key() << " : {\n";
                            unordered_map<string, ConfigValue *>::const_iterator iter;
                            for (iter = children.begin(); iter != children.end(); iter++) {
                                string off = string(offset);
                                off.append("\t");
                                ConfigValue *v = iter->second;
                                if (!IS_NULL(v)) {
                                    v->print(off);
                                }
                            }
                            cout << offset << "}\n";
                        }
                    }

                    const ConfigValue *add_child(ConfigValue *child) {
                        PRECONDITION(!IS_NULL(child));
                        string key = child->get_key();
                        PRECONDITION(!IS_EMPTY(key));

                        children.insert(make_pair(key, child));

                        return this;
                    }

                    const ConfigValue *get_value(string key) {
                        PRECONDITION(!IS_EMPTY(key));
                        vector<string> parts;
                        string_utils::split(key, '/', &parts);
                        string k = parts[0];
                        if (k == ".") {
                            k = parts[1];
                            if (!k.empty()) {
                                return find(parts, 1);
                            }
                            return nullptr;
                        } else {
                            return find(parts, 0);
                        }
                    }

                    const ConfigValue *find(vector<string> path, uint32_t offset) const override {
                        string *key = parse_key_name(path[offset]);
                        if (get_key() == *key) {
                            if (offset == path.size() - 1) {
                                return this;
                            } else {
                                string *ck = parse_key_name(path[offset + 1]);
                                unordered_map<string, ConfigValue *>::const_iterator got = children.find(*ck);
                                if (got != children.end()) {
                                    return got->second->find(path, offset + 1);
                                }
                                CHECK_AND_FREE(ck);
                            }
                        }
                        CHECK_AND_FREE(key);
                        return nullptr;
                    }
                };

                class Config {
                private:
                    __state__ state;
                    ConfigValue *config;

                    ConfigValue *process_node(Json json);

                    ConfigValue *process_node(string name, Json json, ConfigValue *parent);

                    ConfigValue *process_object_node(string name, Json json, ConfigValue *parent);

                    ConfigValue *process_list_node(string name, Json json, ConfigValue *parent);

                    ConfigValue *process_param_node(string name, Json json, ConfigValue *parent);

                    ConfigValue *process_value_node(string name, Json json, ConfigValue *parent);

                public:
                    ~Config();

                    void create(string filename);

                    const ConfigValue *find(string path, bool error) const;

                    const ConfigValue *find(string path) const;

                    const void print() const {
                        CHECK_STATE_AVAILABLE(this->state);
                        if (!IS_NULL(config)) {
                            config->print("");
                        }
                    }

                    const string get_value(string path) const {
                        CHECK_STATE_AVAILABLE(this->state);
                        string s;
                        const ConfigValue *v = find(path);
                        if (NOT_NULL(v)) {
                            if (v->get_type() == Basic) {
                                const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(v);
                                s = bv->get_value();
                            }
                        }
                        return s;
                    }

                    short get_short_value(string path, short def_value) {
                        CHECK_STATE_AVAILABLE(this->state);
                        const ConfigValue *v = find(path);
                        if (NOT_NULL(v)) {
                            if (v->get_type() == Basic) {
                                const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(v);
                                return bv->get_short_value(def_value);
                            }
                        }
                        return def_value;
                    }

                    int get_int_value(string path, int def_value) const {
                        CHECK_STATE_AVAILABLE(this->state);
                        const ConfigValue *v = find(path);
                        if (NOT_NULL(v)) {
                            if (v->get_type() == Basic) {
                                const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(v);
                                return bv->get_int_value(def_value);
                            }
                        }
                        return def_value;
                    }

                    long get_long_value(string path, long def_value) const {
                        CHECK_STATE_AVAILABLE(this->state);
                        const ConfigValue *v = find(path);
                        if (NOT_NULL(v)) {
                            if (v->get_type() == Basic) {
                                const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(v);
                                return bv->get_long_value(def_value);
                            }
                        }
                        return def_value;
                    }

                    float get_float_value(string path, float def_value) const {
                        CHECK_STATE_AVAILABLE(this->state);
                        const ConfigValue *v = find(path);
                        if (NOT_NULL(v)) {
                            if (v->get_type() == Basic) {
                                const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(v);
                                return bv->get_float_value(def_value);
                            }
                        }
                        return def_value;
                    }

                    double get_double_value(string path, double def_value) const {
                        CHECK_STATE_AVAILABLE(this->state);
                        const ConfigValue *v = find(path);
                        if (NOT_NULL(v)) {
                            if (v->get_type() == Basic) {
                                const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(v);
                                return bv->get_double_value(def_value);
                            }
                        }
                        return def_value;
                    }

                    bool get_bool_value(string path, bool def_value) const {
                        CHECK_STATE_AVAILABLE(this->state);
                        const ConfigValue *v = find(path);
                        if (NOT_NULL(v)) {
                            if (v->get_type() == Basic) {
                                const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(v);
                                return bv->get_bool_value(def_value);
                            }
                        }
                        return def_value;
                    }

                    time_t get_time_value(string path, string format, time_t def_value) const {
                        CHECK_STATE_AVAILABLE(this->state);
                        const ConfigValue *v = find(path);
                        if (NOT_NULL(v)) {
                            if (v->get_type() == Basic) {
                                const BasicConfigValue *bv = static_cast<const BasicConfigValue *>(v);
                                return bv->get_time_value(format, def_value);
                            }
                        }
                        return def_value;
                    }

                    time_t get_time_value(string path, time_t def_value) const {
                        CHECK_STATE_AVAILABLE(this->state);
                        return get_time_value(path, common_consts::DEFAULT_DATE_FORMAT, def_value);
                    }

                    static const ParamConfigValue *get_params(const ConfigValue *node) {
                        if (node->get_type() == Node) {
                            const ConfigValue *p = node->find(CONST_CONFIG_PATH_PARAM);
                            if (NOT_NULL(p)) {
                                if (p->get_type() == Params) {
                                    return static_cast<const ParamConfigValue *>(p);
                                }
                            }
                        }
                        return nullptr;
                    }

                    static const BasicConfigValue *get_value(const string name, const ConfigValue *node) {
                        if (node->get_type() == Node) {
                            const ConfigValue *p = node->find(name);
                            if (NOT_NULL(p)) {
                                if (p->get_type() == Basic) {
                                    return static_cast<const BasicConfigValue *>(p);
                                }
                            }
                        }
                        return nullptr;
                    }
                };

                class __configurable {
                protected:
                    const ConfigValue *config = nullptr;
                    __state__ state;

                    virtual void setup() = 0;

                public:
                    virtual ~__configurable() {
                        CHECK_AND_DISPOSE(state);
                        config = nullptr;
                    }

                    void configure(const ConfigValue *config) {
                        this->config = config;
                        try {
                            this->setup();
                            state.set_state(__state_enum::Available);
                        } catch (const exception &e) {
                            config_error ce = CONFIG_ERROR("%s", e.what());
                            state.set_error(&ce);
                            throw ce;
                        } catch (...) {
                            config_error ce = CONFIG_ERROR("Un-typed exception caught.");
                            state.set_error(&ce);
                            throw ce;
                        }
                    }
                };
REACTFS_NS_COMMON_END
#endif // _REACTFS_COMMON_CONFIG_H_
