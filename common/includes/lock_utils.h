//
// Created by Subhabrata Ghosh on 10/11/16.
//

#ifndef REACTFS_LOCK_UTILS_H
#define REACTFS_LOCK_UTILS_H

#include "config.h"
#include "read_write_lock.h"

#define SHARED_LOCK_MAX_COUNT "lock.shared.count.max"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                class lock_utils {
                private:
                    static lock_env *_lock_env = nullptr;

                public:
                    static void create(uint32_t max_count) {
                        PRECONDITION(max_count > 0);
                        PRECONDITION(IS_NULL(_lock_env));

                        _lock_env = new lock_env();
                        _lock_env->create();
                        CHECK_NOT_NULL(_lock_env);
                    }

                    static void create(const ConfigValue *config) {
                        PRECONDITION(NOT_NULL(config));
                        const ParamConfigValue *params = Config::get_params(config);
                        int max_count = MAX_SHARED_LOCKS;
                        if (NOT_NULL(params)) {
                            max_count = params->get_int(SHARED_LOCK_MAX_COUNT, MAX_SHARED_LOCKS);
                        }
                        create(max_count);
                    }

                    static lock_env *get_lock_env() {
                        PRECONDITION(NOT_NULL(_lock_env));

                        return _lock_env;
                    }
                };
            }
        }
    }
}
#endif //REACTFS_LOCK_UTILS_H
