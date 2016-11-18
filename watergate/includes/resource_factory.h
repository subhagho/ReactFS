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
//
// Created by Subhabrata Ghosh on 15/09/16.
//

#ifndef WATERGATE_RESOURCE_FACTORY_H
#define WATERGATE_RESOURCE_FACTORY_H

#include "common/includes/__registry.h"
#include "resource_creator.h"
#include "resource_def.h"

#define CONFIG_NODE_RESOUCEF "resources"

using namespace com::wookler::watergate::core;

namespace com {
    namespace wookler {
        namespace watergate {
            namespace core {
                class resource_factory {
                private:
                    static const ConfigValue *find(string name) {
                        CHECK_NOT_NULL(config);
                        if (config->get_type() == ConfigValueTypeEnum::List) {
                            const ListConfigValue *list = dynamic_cast<const ListConfigValue * >(config);
                            const vector<ConfigValue *> values = list->get_values();
                            if (!IS_EMPTY(values)) {
                                for (ConfigValue *c : values) {
                                    const BasicConfigValue *nn = Config::get_value(CONFIG_NODE_RESOURCE_NAME, c);
                                    CHECK_NOT_NULL(nn);
                                    const string nname = nn->get_value();
                                    if (name == nname) {
                                        return c;
                                    }
                                }
                            }
                        } else if (config->get_type() == ConfigValueTypeEnum::Node) {
                            const BasicConfigValue *c = dynamic_cast<const BasicConfigValue *>(config);
                            const BasicConfigValue *nn = Config::get_value(CONFIG_NODE_RESOURCE_NAME, c);
                            CHECK_NOT_NULL(nn);
                            const string nname = nn->get_value();
                            if (name == nname) {
                                return c;
                            }
                        }
                        return nullptr;
                    }

                public:
                    static __registry<string, resource_creator> resources;
                    static const ConfigValue *config;


                    static resource_def *get_resource(string name) {
                        PRECONDITION(!IS_EMPTY(name));
                        const ConfigValue *node = find(name);
                        CHECK_NOT_NULL(node);
                        const BasicConfigValue *c = Config::get_value(CONFIG_NODE_RESOURCE_CLASS, node);
                        CHECK_NOT_NULL(c);
                        const string classname = c->get_value();
                        resource_creator *r = resources.get(classname);
                        if (IS_NULL(r))
                            throw BASE_ERROR("No registered resource class found. [class=%s]", classname.c_str());

                        resource_def *rd = r->create(name, node);
                        return rd;
                    }

                    static void configure(const ConfigValue *config) {
                        CHECK_NOT_NULL(config);
                        const ConfigValue *node = config->find(CONFIG_NODE_RESOUCEF);
                        CHECK_NOT_NULL(node);
                        resources.set_config(node);
                    }

                    static void add_resource(string classname, resource_creator *resource) {
                        resources.add(classname, resource, true);
                    }

                    static void dispose() {
                        resources.clear();
                    }
                };
            }
        }
    }
}

#endif //WATERGATE_RESOURCE_FACTORY_H
