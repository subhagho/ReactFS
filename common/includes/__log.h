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
// Created by Subhabrata Ghosh on 19/09/16.
//

#ifndef WATERGATE_LOG_H
#define WATERGATE_LOG_H

#include "spdlog/spdlog.h"

#include "common.h"
#include "config.h"
#include "file_utils.h"

#define DEFAULT_LOG_PATTERN "[%Y-%m-%d %H:%M:%S.%e]\t[%l]\t%v"
#define DEFAULT_LOGGER_NAME "watergate"
#define CONST_CONFIG_PATH_LOGGING "logging"
#define CONST_CONFIG_PARAM_LOGGING_FILE "filename"
#define CONST_CONFIG_PARAM_LOGGING_SIZE "size"
#define CONST_CONFIG_PARAM_LOGGING_MAX_FILES "max-files"
#define CONST_CONFIG_PARAM_LOGGING_CONSOLE "console"
#define CONST_CONFIG_PARAM_LOGGING_MODE "level"
#define CONST_CONFIG_PARAM_LOGGING_FORMAT "format"

namespace spd = spdlog;

REACTFS_NS_COMMON
                class __log {
                private:

                    shared_ptr<spd::logger> logger;
                    shared_ptr<spd::logger> console;
                    spd::level::level_enum log_level;

                public:
                    bool console_enabled;

                    ~__log() {
                        spd::drop_all();
                    }

                    bool check_lock_level(spd::level::level_enum level) {
                        bool r = false;
                        if (log_level == spd::level::trace) {
                            return true;
                        }
                        if (log_level == spd::level::debug) {
                            if (level != spd::level::trace)
                                return true;
                        }
                        if (log_level == spd::level::info) {
                            if (level != spd::level::trace && level != spd::level::debug)
                                return true;
                        }
                        if (log_level == spd::level::warn) {
                            if (level != spd::level::trace && level != spd::level::debug && level != spd::level::info)
                                return true;
                        }
                        if (log_level == spd::level::err) {
                            if (level == spd::level::err || level == spd::level::critical)
                                return true;
                        }
                        if (log_level == spd::level::critical) {
                            if (level == spd::level::critical)
                                return true;
                        }
                        return r;
                    }

                    shared_ptr<spd::logger> get_console() {
                        return console;
                    }

                    shared_ptr<spd::logger> get_logger() {
                        return logger;
                    }

                    void init(const ConfigValue *parent, const Path *workdir, const string appname) {
                        CHECK_NOT_NULL(parent);
                        CHECK_NOT_NULL(workdir);
                        CHECK_NOT_EMPTY(appname);

                        Path *file = nullptr;
                        size_t size = 1 * 1024 * 1024;
                        int max_files = 3;
                        log_level = spd::level::level_enum::warn;
                        string format = DEFAULT_LOG_PATTERN;
                        console_enabled = false;

                        const ConfigValue *config = parent->find(CONST_CONFIG_PATH_LOGGING);
                        if (NOT_NULL(config)) {
                            const BasicConfigValue *dv = Config::get_value(CONST_CONFIG_PARAM_LOGGING_FILE, config);
                            if (NOT_NULL(dv)) {
                                const string fs = dv->get_value();
                                if (!IS_EMPTY(fs)) {
                                    file = new Path(fs);
                                    CHECK_ALLOC(file, TYPE_NAME(Path));
                                    const string pp = file->get_parent_dir();
                                    if (!IS_EMPTY(pp)) {
                                        Path pd = Path(pp);
                                        if (!pd.exists()) {
                                            pd.create(0755);
                                        }
                                    }

                                    if (!file->exists()) {
                                        file->create(0755);
                                    }
                                }
                            }
                            if (IS_NULL(file)) {
                                file = new Path(workdir->get_path());
                                CHECK_ALLOC(file, TYPE_NAME(Path));
                                file->append("log");
                                if (!file->exists()) {
                                    file->create(0755);
                                }
                                string f_name = string(appname);
                                f_name.append(".log");
                                file->append(f_name);
                            }
                            const BasicConfigValue *sv = Config::get_value(CONST_CONFIG_PARAM_LOGGING_SIZE, config);
                            if (NOT_NULL(sv)) {
                                size_t st = common_utils::parse_size(sv->get_value());
                                if (st > 0)
                                    size = st;
                            }
                            const BasicConfigValue *mv = Config::get_value(CONST_CONFIG_PARAM_LOGGING_MAX_FILES,
                                                                           config);
                            if (NOT_NULL(mv)) {
                                string s = mv->get_value();
                                if (!IS_EMPTY(s)) {
                                    int m = atoi(s.c_str());
                                    if (m >= 0) {
                                        max_files = m;
                                    }
                                }
                            }
                            const BasicConfigValue *cv = Config::get_value(CONST_CONFIG_PARAM_LOGGING_CONSOLE, config);
                            if (NOT_NULL(cv)) {
                                console_enabled = cv->get_bool_value(false);
                            }

                            string mode = "warn";
                            const BasicConfigValue *mov = Config::get_value(CONST_CONFIG_PARAM_LOGGING_MODE, config);
                            if (NOT_NULL(mov)) {
                                string mm = mov->get_value();
                                if (!IS_EMPTY(mm)) {
                                    mode = mm;
                                }
                            }
                            log_level = get_level(mode);

                            const BasicConfigValue *fov = Config::get_value(CONST_CONFIG_PARAM_LOGGING_FORMAT, config);
                            if (NOT_NULL(fov)) {
                                string fm = mov->get_value();
                                if (!IS_EMPTY(fm)) {
                                    format = fm;
                                }
                            }

                        }
                        string lf = file->get_path();
                        init_c(lf, size, max_files, log_level, format);

                        if (NOT_NULL(file)) {
                            delete (file);
                        }
                    }

                    static spd::level::level_enum get_level(string level) {
                        if (level == "critical") {
                            return spd::level::level_enum::critical;
                        } else if (level == "error") {
                            return spd::level::level_enum::err;
                        } else if (level == "warn") {
                            return spd::level::level_enum::warn;
                        } else if (level == "info") {
                            return spd::level::level_enum::info;
                        } else if (level == "debug") {
                            return spd::level::level_enum::debug;
                        } else if (level == "trace") {
                            return spd::level::level_enum::trace;
                        }
                        return spd::level::level_enum::err;
                    }

                    static string get_level_string(spd::level::level_enum level) {
                        switch (level) {
                            case spd::level::level_enum::critical:
                                return "critical";
                            case spd::level::level_enum::err:
                                return "error";
                            case spd::level::level_enum::warn:
                                return "warn";
                            case spd::level::level_enum::info:
                                return "info";
                            case spd::level::level_enum::debug:
                                return "debug";
                            case spd::level::level_enum::trace:
                                return "trace";
                            default:
                                return common_consts::EMPTY_STRING;
                        }
                        return common_consts::EMPTY_STRING;
                    }

                    void
                    init_c(const string filename, const size_t size, const int max_files,
                           const spd::level::level_enum level,
                           const string format) {
                        cout << "Initializing logger...\n";
                        cout << "Logging level = " << get_level_string(level) << "\n";
                        cout << "Console Logging enabeled? " << (console_enabled ? "true" : "false") << "\n";

                        spd::set_pattern(format);

                        console = console = spd::stdout_logger_mt("console");
                        if (NOT_NULL(console))
                            console->set_level(level);
                        if (!IS_EMPTY(filename)) {
                            logger = spd::rotating_logger_mt(DEFAULT_LOGGER_NAME, filename, size, max_files);
                            if (NOT_NULL(logger)) {
                                logger->set_level(level);
                                cout << "Logging to file. [file=" << filename << "]\n";
                            } else {
                                cout << "Error initializing file logger. [log file=" << filename << "]\n";
                            }
                        }
                    }

                    void init() {
                        spd::level::level_enum level = spd::level::level_enum::warn;
                        string format = DEFAULT_LOG_PATTERN;
                        console_enabled = true;

                        cout << "Initializing logger to default...\n";
                        cout << "Logging level = " << get_level_string(level) << "\n";
                        cout << "Console Logging enabeled? " << (console_enabled ? "true" : "false") << "\n";

                        spd::set_pattern(format);

                        console = spd::get("console");
                        if (NOT_NULL(console))
                            console->set_level(level);
                    }
                };
REACTFS_NS_COMMON_END
#endif //WATERGATE_LOG_H
