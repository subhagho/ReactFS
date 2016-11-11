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
#include <fstream>
#include <streambuf>

#include "common/includes/config.h"

using namespace com::wookler::reactfs::common;

com::wookler::reactfs::common::Config::~Config() {
    state.set_state(Disposed);
    CHECK_AND_FREE(config);
}

void com::wookler::reactfs::common::Config::create(string filename) {
    CHECK_NOT_EMPTY(filename);

    try {
        cout << "Opening configuration file. [file=" << filename << "]\n";
        std::ifstream t(filename, std::ifstream::in);
        if (!t.is_open()) {
            throw CONFIG_ERROR("Error opening configuration file. [file=%s][error=%s]", filename.c_str(),
                               strerror(errno));
        }

        stringstream ss;
        string line;
        while (getline(t, line)) {
            ss << line;
        }
        t.close();

        string data(ss.str());

        CHECK_NOT_EMPTY(data);

        string err;
        Json json = Json::parse(data.c_str(), err);

        if (json.type() == Json::OBJECT) {
            Json c_node = json[CONST_CONFIG_ROOT_NAME];
            if (!c_node.is_null()) {
                process_node(c_node);
            } else
                process_node(json);
        }

        state.set_state(Available);
    } catch (exception &e) {
        config_error ce = CONFIG_ERROR("Error initializing configuration. [error=%s]", e.what());
        state.set_error(&ce);
        cout << "Error creating configuration...\n";
        cout << "ERROR : " << ce.what() << "\n";
        throw ce;
    } catch (...) {
        config_error e = CONFIG_ERROR("Error creating configuration instance.");
        state.set_error(&e);
        cout << "Error creating configuration...\n";
        cout << "ERROR : " << e.what() << "\n";
        throw e;
    }
}

ConfigValue *com::wookler::reactfs::common::Config::process_node(Json json) {
    config = new NodeConfigValue(CONST_CONFIG_ROOT_NAME, nullptr);

    map<string, Json> values = json.object_items();
    if (!IS_EMPTY(values)) {
        map<string, Json>::iterator iter;
        for (iter = values.begin(); iter != values.end(); iter++) {
            string key = iter->first;
            Json value = iter->second;
            if (!IS_EMPTY(key)) {
                if (!value.is_null()) {
                    process_node(key, value, config);
                }
            }
        }
    }

    return config;
}

ConfigValue *com::wookler::reactfs::common::Config::process_node(string name, Json json, ConfigValue *parent) {
    if (!json.is_null()) {
        if (json.is_string() || json.is_number() || json.is_bool()) {
            return process_value_node(name, json, parent);
        } else if (json.is_object()) {
            return process_object_node(name, json, parent);
        } else if (json.is_array()) {
            return process_list_node(name, json, parent);
        }
    }
    return nullptr;
}

ConfigValue *com::wookler::reactfs::common::Config::process_value_node(string name, Json json, ConfigValue *parent) {
    assert(!json.is_null());
    assert(!IS_NULL(parent));

    if (json.is_string() || json.is_number() || json.is_bool()) {
        BasicConfigValue *c = new BasicConfigValue(name, parent);
        if (json.is_string()) {
            c->set_value(json.string_value());
        } else if (json.is_number()) {
            double d = json.number_value();
            c->set_value(to_string(d));
        } else if (json.is_bool()) {
            bool b = json.bool_value();
            if (b) {
                c->set_value(BOOL_TRUE);
            } else {
                c->set_value(BOOL_FALSE);
            }
        }

        if (parent->get_type() == ConfigValueTypeEnum::Node) {
            NodeConfigValue *ptr = static_cast<NodeConfigValue *>(parent);
            ptr->add_child(c);
        } else if (parent->get_type() == ConfigValueTypeEnum::List) {
            ListConfigValue *ptr = static_cast<ListConfigValue *>(parent);
            ptr->add_value(c);
        } else if (parent->get_type() == ConfigValueTypeEnum::Params) {
            ParamConfigValue *ptr = static_cast<ParamConfigValue *>(parent);
            ptr->put(c);
        }
        return c;
    }

    return nullptr;
}

ConfigValue *com::wookler::reactfs::common::Config::process_list_node(string name, Json json, ConfigValue *parent) {
    assert(!json.is_null());
    assert(!IS_NULL(parent));

    if (json.is_array()) {
        vector<Json> values = json.array_items();
        if (!IS_EMPTY(values)) {
            ListConfigValue *node = new ListConfigValue(name, parent);
            for (int ii = 0; ii < values.size(); ii++) {
                Json j = values[ii];
                process_node(name, j, node);
            }
            if (parent->get_type() == ConfigValueTypeEnum::Node) {
                NodeConfigValue *ptr = static_cast<NodeConfigValue *>(parent);
                ptr->add_child(node);
            } else if (parent->get_type() == ConfigValueTypeEnum::List) {
                ListConfigValue *ptr = static_cast<ListConfigValue *>(parent);
                ptr->add_value(node);
            }
            return node;
        }
    }

    return nullptr;
}

ConfigValue *com::wookler::reactfs::common::Config::process_object_node(string name, Json json, ConfigValue *parent) {
    assert(!json.is_null());
    assert(!IS_NULL(parent));

    if (json.is_object()) {
        map<string, Json> values = json.object_items();
        if (!IS_EMPTY(values)) {
            NodeConfigValue *node = new NodeConfigValue(name, parent);
            map<string, Json>::iterator iter;
            for (iter = values.begin(); iter != values.end(); iter++) {
                string key = iter->first;
                Json value = iter->second;

                if (key == CONST_CONFIG_NODE_PARAM) {
                    process_param_node(key, value, node);
                } else {
                    process_node(key, value, node);
                }
            }
            if (parent->get_type() == ConfigValueTypeEnum::Node) {
                NodeConfigValue *ptr = static_cast<NodeConfigValue *>(parent);
                ptr->add_child(node);
            } else if (parent->get_type() == ConfigValueTypeEnum::List) {
                ListConfigValue *ptr = static_cast<ListConfigValue *>(parent);
                ptr->add_value(node);
            }
            return node;
        }
    }
    return nullptr;
}

ConfigValue *com::wookler::reactfs::common::Config::process_param_node(string name, Json json, ConfigValue *parent) {
    assert(!json.is_null());
    assert(!IS_NULL(parent));

    if (json.is_object()) {
        map<string, Json> values = json.object_items();
        if (!IS_EMPTY(values)) {
            ParamConfigValue *node = new ParamConfigValue(name, parent);
            map<string, Json>::iterator iter;
            for (iter = values.begin(); iter != values.end(); iter++) {
                string key = iter->first;
                Json value = iter->second;

                process_value_node(key, value, node);
            }
            if (parent->get_type() == ConfigValueTypeEnum::Node) {
                NodeConfigValue *ptr = static_cast<NodeConfigValue *>(parent);
                ptr->add_child(node);
            } else if (parent->get_type() == ConfigValueTypeEnum::List) {
                ListConfigValue *ptr = static_cast<ListConfigValue *>(parent);
                ptr->add_value(node);
            }
            return node;
        }
    }
    return nullptr;
}

const ConfigValue *com::wookler::reactfs::common::Config::find(string path) const {
    return this->find(path, false);
}

const ConfigValue *com::wookler::reactfs::common::Config::find(string path, bool error) const {
    CHECK_STATE_AVAILABLE(this->state);
    if (!IS_NULL(config)) {
        const ConfigValue *cv = config->find(path);
        if (NOT_NULL(cv)) {
            return cv;
        }
    }
    if (error) {
        throw NOT_FOUND_ERROR("No configuration node found for path. [path=%s]", path.c_str());
    }
    return nullptr;
}

