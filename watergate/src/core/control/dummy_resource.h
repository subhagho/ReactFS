//
// Created by Subhabrata Ghosh on 15/09/16.
//

#ifndef WATERGATE_DUMMY_RESOURCE_H
#define WATERGATE_DUMMY_RESOURCE_H

#include "includes/core/resource_def.h"
#include "includes/common/common_utils.h"

#define CONST_DR_CONFIG_PARAM_SIZE "max_concurrency"

namespace com {
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

#endif //WATERGATE_DUMMY_RESOURCE_H
