//
// Created by Subhabrata Ghosh on 07/11/16.
//

#ifndef REACTFS_PROCESS_UTILS_H
#define REACTFS_PROCESS_UTILS_H

#include <sys/types.h>
#include <signal.h>

#include "common.h"

#define SIZE_THREAD_ID 64

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                /*!
                 * Utility class for defining thread releated methods.
                 */
                class thread_utils {
                public:
                    /*!
                     * Get the current thread id as string.
                     *
                     * @return - String value of thread id.
                     */
                    static std::string get_current_thread() {
                        std::stringstream ss;
                        ss << std::this_thread::get_id();
                        return std::string(ss.str());
                    }
                };

                /*!
                 * Utility class to define processe related methods.
                 */
                class process_utils {
                public:
                    /*!
                     * Check if a process with the specified PID exists.
                     *
                     * @param pid - PID to check.
                     * @return - Process exists.
                     */
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
