//
// Created by Subhabrata Ghosh on 15/09/16.
//

#ifndef WATERGATE_RESOURCE_FACTORY_H
#define WATERGATE_RESOURCE_FACTORY_H

#include "watergate/src/core/control/dummy_resource.h"
#include "filesystem_driver.h"

using namespace com::watergate::core::io;

namespace com {
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

#endif //WATERGATE_RESOURCE_FACTORY_H
