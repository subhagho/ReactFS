//
// Created by Subhabrata Ghosh on 13/10/16.
//

#ifndef WATERGATE_ENV_UTILS_H
#define WATERGATE_ENV_UTILS_H

#include "__env.h"

using namespace com::watergate::common;

namespace com {
    namespace watergate {
        namespace common {
            class env_utils {
            private:
                static __env *env;

            public:

                static void create_env(const string configfile) {
                    env = new __env();
                    env->create(configfile);
                    CHECK_ENV_STATE(env);
                }

                static void create_env(const string configfile, const string appname) {
                    env = new __env();
                    env->create(configfile, appname);
                    CHECK_ENV_STATE(env);
                }

                static const __env *get_env() {
                    CHECK_NOT_NULL(env);
                    CHECK_ENV_STATE(env);

                    return env;
                }

                static const Config *get_config() {
                    const __env *e = get_env();

                    return e->get_config();
                }

                static void dispose() {
                    LOG_DEBUG("[pid=%d] Releasing environment handle...", getpid());
                    CHECK_AND_FREE(env);

                    env = nullptr;
                }
            };
        }
    }
}
#endif //WATERGATE_ENV_UTILS_H
