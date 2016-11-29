//
// Created by Subhabrata Ghosh on 09/11/16.
//

#include "test_node_client.h"

TEST_CASE("Test Node client env setup", "[com::wookler::reactfs::core::node_client_env") {
    string configf = string(CONFIG_FILE);
    node_init_client::create_node_env(configf);
    node_client_env *c_env = node_init_client::get_client_env();
    CHECK_NOT_NULL(c_env);

    for (uint16_t ii = 0; ii < 20; ii++) {
        string s = block_utils::get_block_dir(c_env->get_mount_client(), (ii * M_BYTES));
    }
    node_init_client::shutdown();
}
