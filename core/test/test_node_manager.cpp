//
// Created by Subhabrata Ghosh on 09/11/16.
//

#include "test_node_manager.h"

TEST_CASE("Test Node client env setup", "[com::wookler::reactfs::core::node_client_env") {
    string configf = string(CONFIG_FILE);
    node_init_manager::create_node_env(configf, true);
    node_server_env *env = node_init_manager::get_server_env();
    CHECK_NOT_NULL(env);

    const __env *e = init_utils::get_env();
    Config* config = e->get_config();
    CHECK_NOT_NULL(config);

    config->print();

    node_init_manager::shutdown();
}
