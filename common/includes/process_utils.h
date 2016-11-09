//
// Created by Subhabrata Ghosh on 07/11/16.
//

#ifndef REACTFS_PROCESS_UTILS_H
#define REACTFS_PROCESS_UTILS_H

#include "common.h"

#define SIZE_THREAD_ID 64

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                class thread_utils {
                public:
                    static std::string get_current_thread() {
                        std::stringstream ss;
                        ss << std::this_thread::get_id();
                        return std::string(ss.str());
                    }


                };

                class process_utils {
                public:
                    static bool check_process(pid_t pid) {
                        if (0 == kill(pid, 0)) {
                            return true;
                        }
                        return false;
                    }
                };
            }
        }
    }
}
#endif //REACTFS_PROCESS_UTILS_H
