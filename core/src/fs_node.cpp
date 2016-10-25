//
// Created by Subhabrata Ghosh on 25/10/16.
//

#include "core/includes/fs_node.h"
#include "common/includes/log_utils.h"

com::wookler::reactfs::core::fs_node *com::wookler::reactfs::core::fs_node::start(string config_file) {
    try {
        env_utils::create_env(config_file);

        const __env *env = env_utils::get_env();
        CHECK_ENV_STATE(env);

        const Config *config = env->get_config();
        const ConfigValue *node = config->find(CONFIG_NODE_FS_PATH);
        CHECK_NOT_NULL(node);

        
    } catch (const exception &e) {
        fs_error_base be = FS_BASE_ERROR_E(e);
        state.set_error(&be);
        LOG_ERROR(be.what());
        throw be;
    } catch (...) {
        fs_error_base be = FS_BASE_ERROR_E("Unknown error occurred.");
        state.set_error(&be);
        LOG_ERROR(be.what());
        throw be;
    }
}