//
// Created by Subhabrata Ghosh on 25/10/16.
//

#ifndef REACTFS_FS_NODE_H
#define REACTFS_FS_NODE_H

#include "common/includes/common.h"
#include "common/includes/__app.h"
#include "common/includes/__env.h"
#include "common/includes/init_utils.h"

#include "fs_error_base.h"

#define CONFIG_NODE_FS_PATH "/configuration/filesystem/node"

using namespace com::watergate::common;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace core {
                enum fs_node_state_enum {
                    Unkown = 0, Initialized, Available, Exception, Recovering, Starting, Stopped
                };

                class fs_node_state {
                private:
                    fs_node_state_enum state = fs_node_state_enum::Available;
                    const exception *error = nullptr;

                public:
                    const fs_node_state_enum get_state() const {
                        return state;
                    }

                    void set_state(fs_node_state_enum state) {
                        this->state = state;
                    }

                    void set_error(const exception *e) {
                        error = e;
                        state = fs_node_state_enum::Exception;
                    }

                    const exception *get_error() const {
                        if (state == fs_node_state_enum::Exception) {
                            return error;
                        }
                        return NULL;
                    }

                    bool has_error() const {
                        return (state == fs_node_state_enum::Exception);
                    }

                    bool is_available() const {
                        return (state == fs_node_state_enum::Available);
                    }

                    static string get_state_string(fs_node_state_enum state) {
                        switch (state) {
                            case fs_node_state_enum::Unkown:
                                return "UNKNOWN";
                            case fs_node_state_enum::Available:
                                return "AVAILABLE";
                            case fs_node_state_enum::Initialized:
                                return "INITIALIZED";
                            case fs_node_state_enum::Exception:
                                return "EXCEPTION";
                            case fs_node_state_enum::Recovering:
                                return "RECOVERING";
                            case fs_node_state_enum::Starting:
                                return "STARTING";
                            case fs_node_state_enum::Stopped:
                                return "STOPPED";
                            default:
                                return "UNKNOWN";
                        }
                    }
                };

                class fs_node {
                private:
                    uint32_t node_id;
                    fs_node_state state;

                public:
                    fs_node *start(string config_file);

                    fs_node *stop();

                    const fs_node_state_enum get_state() const {
                        return state.get_state();
                    }

                    const exception *get_error() const {
                        return state.get_error();
                    }

                    const uint32_t get_node_id() const {
                        return node_id;
                    }

                    const string get_node_uid() const {
                        const __env *env = env_utils::get_env();
                        CHECK_NOT_NULL(env);

                        return env->get_app()->get_id();
                    }

                    const __app *get_instance_info() const {
                        const __env *env = env_utils::get_env();
                        CHECK_NOT_NULL(env);

                        return env->get_app();
                    }

                    bool is_available() const {
                        return state.is_available();
                    }
                };
            }
        }
    }
}

#endif //REACTFS_FS_NODE_H
