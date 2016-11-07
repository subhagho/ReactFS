//
// Created by Subhabrata Ghosh on 07/11/16.
//

#ifndef REACTFS_PROCESS_UTILS_H
#define REACTFS_PROCESS_UTILS_H

#include "common.h"

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
            }
        }
    }
}
#endif //REACTFS_PROCESS_UTILS_H
