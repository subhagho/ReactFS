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

#include "watergate/src/core/control/dummy_resource.h"
#include "filesystem_driver.h"

using namespace com::wookler::watergate::core::io;

namespace com {
    namespace wookler {
        namespace watergate {
            namespace core {
                class resource_factory {
                public:
                    static const string DUMMY_RESOURCE_CLASS;
                    static const string FS_RESOURCE_CLASS;

                    static resource_def *get_resource(string classname, const ConfigValue *config) {
                        assert(!IS_EMPTY(classname));

                        if (classname == DUMMY_RESOURCE_CLASS) {
                            dummy_resource *r = new dummy_resource();
                            r->init(config);

                            return r;
                        } else if (classname == FS_RESOURCE_CLASS) {
                            filesystem_driver *r = new filesystem_driver();
                            r->init(config);

                            return r;
                        }
                        throw BASE_ERROR("No registered resource class found. [class=%s]", classname.c_str());
                    }
                };
            }
        }
    }
}

#endif //WATERGATE_RESOURCE_FACTORY_H
