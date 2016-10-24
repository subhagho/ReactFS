//
// Created by Subhabrata Ghosh on 13/10/16.
//

#ifndef WATERGATE_INIT_UTILS_H
#define WATERGATE_INIT_UTILS_H

#include "includes/common/__env.h"
#include "includes/core/control_def.h"
#include "includes/core/control_manager.h"

using namespace com::watergate::core;
using namespace com::watergate::common;

namespace com {
    namespace watergate {
        namespace core {
            class init_utils {
            private:
                static control_client *client;
                static __env *env;

            public:
                static control_manager *init_control_manager(const __env *env, const string path) {
                    CHECK_NOT_NULL(env);
                    CHECK_NOT_EMPTY(path);

                    const Config *config = env->get_config();
                    CHECK_NOT_NULL(config);

                    const ConfigValue *m_config = config->find(path);
                    CHECK_NOT_NULL(m_config);

                    control_manager *manager = new control_manager();
                    manager->init(env->get_app(), m_config);

                    return manager;
                }

                static const control_client *init_control_client(const __env *env, const string path) {
                    CHECK_NOT_NULL(env);
                    CHECK_NOT_EMPTY(path);

                    const Config *config = env->get_config();
                    CHECK_NOT_NULL(config);

                    const ConfigValue *c_config = config->find(path);
                    CHECK_NOT_NULL(c_config);

                    control_client *control = new control_client();
                    control->init(env->get_app(), c_config);

                    set_client(control);

                    return control;
                }

                static const control_client *get_client() {
                    CHECK_NOT_NULL(client);
                    CHECK_STATE_AVAILABLE(client->get_state());
                    return client;
                }

                static void set_client(control_client *client_p) {
                    CHECK_NOT_NULL(client_p);
                    CHECK_STATE_AVAILABLE(client_p->get_state());

                    client = client_p;
                }

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
                    LOG_DEBUG("[pid=%d] Releasing control client...", getpid());
                    CHECK_AND_FREE(client);
                    LOG_DEBUG("[pid=%d] Releasing environment handle...", getpid());
                    CHECK_AND_FREE(env);

                    client = nullptr;
                    env = nullptr;
                }
            };
        }
    }
}
#endif //WATERGATE_INIT_UTILS_H
