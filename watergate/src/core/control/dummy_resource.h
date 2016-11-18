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

#ifndef WATERGATE_DUMMY_RESOURCE_H
#define WATERGATE_DUMMY_RESOURCE_H

#include "common/includes/common_utils.h"
#include "watergate/includes/resource_def.h"
#include "watergate/includes/resource_creator.h"

#define CONST_DR_CONFIG_PARAM_SIZE "max_concurrency"

namespace com {
    namespace wookler {
        namespace watergate {
            namespace core {
                class dummy_resource : public resource_def {
                private:
                    int size = 2048;
                    string *name;

                protected:
                    virtual void setup() override;

                public:
                    static const string DUMMY_RESOURCE_CLASS;

                    dummy_resource() : resource_def(resource_type_enum::IO) {
                        this->name = new string(common_utils::uuid());
                    }

                    ~dummy_resource() override {
                        if (NOT_NULL(name)) {
                            delete (name);
                        }
                    }

                    int get_control_size() override;

                    const string *get_resource_name() override;

                    bool accept(const string name) override {
                        return false;
                    }
                };

                class dummy_resource_creator : public resource_creator {
                public:
                    resource_def *create(string name, const ConfigValue *node) override {
                        const BasicConfigValue *cn = Config::get_value(CONFIG_NODE_RESOURCE_CLASS, node);
                        CHECK_NOT_NULL(cn);
                        POSTCONDITION(cn->get_value() == dummy_resource::DUMMY_RESOURCE_CLASS);

                        dummy_resource *resource = new dummy_resource();
                        resource->configure(node);

                        return resource;
                    }
                };
            }
        }
    }
}

#endif //WATERGATE_DUMMY_RESOURCE_H
