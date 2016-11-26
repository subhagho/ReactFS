//
// Created by Subhabrata Ghosh on 24/11/16.
//

#include "core/includes/block_utils.h"
#include "core/includes/server/node_init_manager.h"
#include "core/includes/clients/node_client_env.h"

using namespace com::wookler::reactfs::core;
using namespace com::wookler::reactfs::core::server;
using namespace com::wookler::reactfs::core::client;

int main(int argc, char **argv) {
    try {
        PRECONDITION(argc > 1);
        char *cf = argv[1];
        CHECK_NOT_NULL(cf);

        bool reset = false;
        string configf(cf);
        PRECONDITION(!IS_EMPTY(configf));

        if (argc > 2) {
            char *rs = argv[2];
            if (NOT_NULL(rs)) {
                if (strncmp(rs, BOOL_TRUE, strlen(rs)) == 0) {
                    reset = true;
                }
            }
        }
        node_init_manager::create_node_env(configf, reset);
        node_server_env *env = node_init_manager::get_server_env();
        CHECK_NOT_NULL(env);

        node_init_client::create_node_env(configf);
        node_client_env *c_env = node_init_client::get_client_env();
        CHECK_NOT_NULL(c_env);

        for (uint16_t ii = 0; ii < 20; ii++) {
            string s = block_utils::get_block_dir(c_env->get_mount_client());
        }
        node_init_client::shutdown();
        node_init_manager::shutdown();

    } catch (const exception &e) {
        LOG_ERROR(e.what());
        exit(-1);
    } catch (...) {
        LOG_ERROR("Unhandled exception occurred.");
        exit(-2);
    }
}