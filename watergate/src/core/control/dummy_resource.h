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

#include "watergate/includes/resource_def.h"
#include "common/includes/common_utils.h"

#define CONST_DR_CONFIG_PARAM_SIZE "max_concurrency"

namespace com {
    namespace wookler {
        namespace watergate {
            namespace core {
                class dummy_resource : public resource_def {
                private:
                    int size = 2048;
                    string *name;

                public:
                    dummy_resource() : resource_def(resource_type_enum::IO) {
                        this->name = new string(common_utils::uuid());
                    }

                    ~dummy_resource() override {
                        if (NOT_NULL(name)) {
                            delete (name);
                        }
                    }

                    void init(const ConfigValue *config) override;

                    int get_control_size() override;

                    const string *get_resource_name() override;

                    bool accept(const string name) override {
                        return false;
                    }
                };
            }
        }
    }
}

#endif //WATERGATE_DUMMY_RESOURCE_H
