//
// Created by Subhabrata Ghosh on 10/10/16.
//

#ifndef WATERGATE_FILESYSTEM_DRIVER_H
#define WATERGATE_FILESYSTEM_DRIVER_H

#include <stdlib.h>
#include <unordered_map>

#ifdef __APPLE__

#include <sys/syslimits.h>

#elif __linux__
#include <linux/limits.h>
#endif

#include "includes/core/resource_def.h"
#include "includes/common/file_utils.h"
#include "includes/common/log_utils.h"

using namespace com::watergate::core;

namespace com {
    namespace watergate {
        namespace core {
            namespace io {
                class fs_driver_constants {
                public:
                    static const string CONFIG_PARAM_ROOT_PATH;
                    static const string CONFIG_PARAM_QUOTA_BYTES;
                    static const string CONFIG_PARAM_QUOTA_LEASE_TIME;
                    static const string CONFIG_PARAM_MAX_CONCURRENCY;
                };

                class filesystem_driver : public resource_def {
                private:
                    Path *root_path;
                    int concurrency;

                public:
                    filesystem_driver() : resource_def(FS) {}

                    ~filesystem_driver() {
                        CHECK_AND_FREE(root_path);
                    }

                    void init(const ConfigValue *config) override;

                    int get_control_size() override {
                        return concurrency;
                    }

                    const string *get_resource_name() override;

                    bool accept(const string name) override {
                        CHECK_NOT_EMPTY(name);
                        char path[PATH_MAX];
                        memset(path, 0, PATH_MAX);

                        if (NOT_NULL(realpath(name.c_str(), path))) {
                            LOG_DEBUG("Requested file path [%s]", path);

                            string str(path);
                            if (!IS_EMPTY(str)) {
                                size_t pos = str.find(root_path->get_path());
                                if (pos == 0) {
                                    return true;
                                }
                            }
                        }
                        return false;
                    }
                };
            }
        }
    }
}

#endif //WATERGATE_FILESYSTEM_DRIVER_H
