/* 
 * Copyright [2016] [Subhabrata Ghosh] 
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); 
 * you may not use this file except in compliance with the License. 
 * You may obtain a copy of the License at 
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * See the License for the specific language governing permissions and 
 *       limitations under the License. 
 * 
 */
//
// Created by Subhabrata Ghosh on 16/09/16.
//

#ifndef WATERGATE_LOG_UTILS_H
#define WATERGATE_LOG_UTILS_H

#include "__log.h"

#define LOG_DEBUG(fmt, ...) do { \
    if (com::wookler::reactfs::common::log_utils::check_log_level(spdlog::level::debug)) { \
        string s = com::wookler::reactfs::common::common_utils::format(fmt, ##__VA_ARGS__); \
        com::wookler::reactfs::common::log_utils::debug(s); \
    } \
} while(0)
#define LOG_INFO(fmt, ...) do { \
    if (com::wookler::reactfs::common::log_utils::check_log_level(spdlog::level::info)) { \
        string s = com::wookler::reactfs::common::common_utils::format(fmt, ##__VA_ARGS__); \
        com::wookler::reactfs::common::log_utils::info(s); \
    } \
} while(0)
#define LOG_WARN(fmt, ...) do { \
    if (com::wookler::reactfs::common::log_utils::check_log_level(spdlog::level::warn)) { \
        string s = com::wookler::reactfs::common::common_utils::format(fmt, ##__VA_ARGS__); \
        com::wookler::reactfs::common::log_utils::warn(s); \
    } \
} while(0)
#define LOG_ERROR(fmt, ...) do { \
    if (com::wookler::reactfs::common::log_utils::check_log_level(spdlog::level::err)) { \
        string s = com::wookler::reactfs::common::common_utils::format(fmt, ##__VA_ARGS__); \
        com::wookler::reactfs::common::log_utils::error(s); \
    } \
} while(0)
#define LOG_CRITICAL(fmt, ...) do { \
    if (com::wookler::reactfs::common::log_utils::check_log_level(spdlog::level::critical)) { \
        string s = com::wookler::reactfs::common::common_utils::format(fmt, ##__VA_ARGS__); \
        com::wookler::reactfs::common::log_utils::critical(s); \
    } \
} while(0)
#define TRACE(fmt, ...) do { \
    if (com::wookler::reactfs::common::log_utils::check_log_level(spdlog::level::trace)) { \
        string s = com::wookler::reactfs::common::common_utils::format(fmt, ##__VA_ARGS__); \
        com::wookler::reactfs::common::log_utils::trace(s); \
    } \
} while(0)

namespace spd = spdlog;

extern com::wookler::reactfs::common::__log *LOG;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                class log_utils {
                public:
                    static bool check_log_level(spd::level::level_enum level) {
                        if (NOT_NULL(LOG)) {
                            return LOG->check_lock_level(level);
                        }
                        return false;
                    }

                    static void critical(string mesg) {
                        if (NOT_NULL(LOG)) {
                            bool console_enabled = LOG->console_enabled;
                            shared_ptr<spd::logger> console = LOG->get_console();
                            shared_ptr<spd::logger> logger = LOG->get_logger();

                            if (console_enabled || IS_NULL(logger)) {
                                if (NOT_NULL(console))
                                    console->critical(mesg);
                            }
                            if (NOT_NULL(logger)) {
                                logger->critical(mesg);
                            }
                        } else {
                            cerr << mesg << "\n";
                        }
                    }

                    static void error(string mesg) {
                        if (NOT_NULL(LOG)) {
                            bool console_enabled = LOG->console_enabled;
                            shared_ptr<spd::logger> console = LOG->get_console();
                            shared_ptr<spd::logger> logger = LOG->get_logger();

                            if (console_enabled || IS_NULL(logger)) {
                                if (NOT_NULL(console))
                                    console->error(mesg);
                            }
                            if (NOT_NULL(logger)) {
                                logger->error(mesg);
                            }
                        } else {
                            cerr << mesg << "\n";
                        }
                    }

                    static void warn(string mesg) {
                        if (NOT_NULL(LOG)) {
                            bool console_enabled = LOG->console_enabled;
                            shared_ptr<spd::logger> console = LOG->get_console();
                            shared_ptr<spd::logger> logger = LOG->get_logger();

                            if (console_enabled || IS_NULL(logger)) {
                                if (NOT_NULL(console))
                                    console->warn(mesg);
                            }
                            if (NOT_NULL(logger)) {
                                logger->warn(mesg);
                            }
                        } else {
                            cout << mesg << "\n";
                        }
                    }

                    static void info(string mesg) {
                        if (NOT_NULL(LOG)) {
                            bool console_enabled = LOG->console_enabled;
                            shared_ptr<spd::logger> console = LOG->get_console();
                            shared_ptr<spd::logger> logger = LOG->get_logger();

                            if (console_enabled || IS_NULL(logger)) {
                                if (NOT_NULL(console))
                                    console->info(mesg);
                            }
                            if (NOT_NULL(logger)) {
                                logger->info(mesg);
                            }
                        } else {
                            cout << mesg << "\n";
                        }
                    }

                    static void debug(string mesg) {
                        if (NOT_NULL(LOG)) {
                            bool console_enabled = LOG->console_enabled;
                            shared_ptr<spd::logger> console = LOG->get_console();
                            shared_ptr<spd::logger> logger = LOG->get_logger();

                            if (console_enabled || IS_NULL(logger)) {
                                if (NOT_NULL(console))
                                    console->debug(mesg);
                            }
                            if (NOT_NULL(logger)) {
                                logger->debug(mesg);
                            }
                        } else {
                            cout << mesg << "\n";
                        }
                    }

                    static void trace(string mesg) {
                        if (NOT_NULL(LOG)) {
                            bool console_enabled = LOG->console_enabled;
                            shared_ptr<spd::logger> console = LOG->get_console();
                            shared_ptr<spd::logger> logger = LOG->get_logger();

                            if (console_enabled || IS_NULL(logger)) {
                                if (NOT_NULL(console))
                                    console->trace(mesg);
                            }
                            if (NOT_NULL(logger)) {
                                logger->trace(mesg);
                            }
                        } else {
                            cout << mesg << "\n";
                        }
                    }
                };
            }
        }
    }
}
#endif //WATERGATE_LOG_UTILS_H
