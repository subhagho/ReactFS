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
// Created by Subhabrata Ghosh on 27/11/16.
//

#ifndef REACTFS_TIME_UTILS_H
#define REACTFS_TIME_UTILS_H

#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>

#include "common.h"
#include "common_utils.h"

using namespace std::chrono;

REACTFS_NS_COMMON
                class time_utils {
                public:
                    static uint64_t now() {
                        milliseconds nt = duration_cast<milliseconds>(
                                system_clock::now().time_since_epoch()
                        );
                        return nt.count();
                    }

                    static long get_delta_time(uint64_t start) {
                        milliseconds nt = duration_cast<milliseconds>(
                                system_clock::now().time_since_epoch()
                        );
                        return (nt.count() - start);
                    }

                    static string get_time_string(uint64_t ts, string fmt) {
                        long t = ts / 1000;
                        auto tm = *std::localtime(&t);

                        std::ostringstream oss;
                        oss << std::put_time(&tm, fmt.c_str());
                        string str = string(oss.str());

                        long ms = ts % 1000;
                        str.append(".");
                        str.append(to_string(ms));

                        return str;
                    }

                    static string get_time_string(uint64_t ts) {
                        return get_time_string(ts, common_consts::DEFAULT_TIMESTAMP_FORMAT);
                    }

                    static uint64_t parse_time(const string &str, const string &format) {
                        if (!IS_EMPTY(str) && !IS_EMPTY(format)) {
                            std::tm tm = {};
                            strptime(str.c_str(), format.c_str(), &tm);
                            auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
                            return tp.time_since_epoch().count();
                        }
                        return 0;
                    }

                    static uint64_t parse_time(const string &str) {
                        string format(common_consts::DEFAULT_TIMESTAMP_FORMAT);
                        return parse_time(str, format);
                    }

                    static string get_hour_dir() {
                        time_t t = time(NULL);
                        tm *time_ptr = localtime(&t);

                        string dir = common_utils::format("%d/%02d/%02d/%02d", time_ptr->tm_year, time_ptr->tm_mon,
                                                          time_ptr->tm_mday, time_ptr->tm_hour);

                        return dir;
                    }

                    static string get_day_dir() {
                        time_t t = time(NULL);
                        tm *time_ptr = localtime(&t);

                        string dir = common_utils::format("%d/%02d/%02d", time_ptr->tm_year, time_ptr->tm_mon,
                                                          time_ptr->tm_mday);

                        return dir;
                    }

                    static string get_month_dir() {
                        time_t t = time(NULL);
                        tm *time_ptr = localtime(&t);

                        string dir = common_utils::format("%d/%02d/", time_ptr->tm_year, time_ptr->tm_mon);

                        return dir;
                    }

                    static uint16_t get_year(time_t *tt) {
                        time_t t = time(tt);
                        tm *time_ptr = localtime(&t);

                        return (uint16_t) (time_ptr->tm_year);
                    }

                    static uint16_t get_month(time_t *tt) {
                        time_t t = time(tt);
                        tm *time_ptr = localtime(&t);

                        return (uint16_t) (time_ptr->tm_mon);
                    }

                    static uint16_t get_day(time_t *tt) {
                        time_t t = time(tt);
                        tm *time_ptr = localtime(&t);

                        return (uint16_t) (time_ptr->tm_mday);
                    }

                    static uint16_t get_hour(time_t *tt) {
                        time_t t = time(tt);
                        tm *time_ptr = localtime(&t);

                        return (uint16_t) (time_ptr->tm_hour);
                    }

                    static bool check_hour(long ti) {
                        long tt = now();
                        if (tt != ti) {
                            uint16_t hi = get_hour(&ti);
                            uint16_t ht = get_hour(&tt);
                            return (hi == ht);
                        }
                        return true;
                    }

                    static bool check_day(long ti) {
                        long tt = now();
                        if (tt != ti) {
                            uint16_t hi = get_day(&ti);
                            uint16_t ht = get_day(&tt);
                            return (hi == ht);
                        }
                        return true;
                    }

                    static bool check_month(long ti) {
                        long tt = now();
                        if (tt != ti) {
                            uint16_t hi = get_month(&ti);
                            uint16_t ht = get_month(&tt);
                            return (hi == ht);
                        }
                        return true;
                    }

                    static uint32_t get_hour_diff(long ti) {
                        long tt = now();
                        uint64_t delta = (tt - ti);

                        uint32_t hours = (delta / (1000 * 60 * 60));
                        if (hours == 0) {
                            uint16_t hi = get_hour(&ti);
                            uint16_t ht = get_hour(&tt);
                            return (ht - hi);
                        }
                        return hours;
                    }

                    static uint32_t get_day_diff(long ti) {
                        long tt = now();
                        uint64_t delta = (tt - ti);

                        uint32_t hours = (delta / (1000 * 60 * 60 * 24));
                        if (hours == 0) {
                            uint16_t hi = get_day(&ti);
                            uint16_t ht = get_day(&tt);
                            return (ht - hi);
                        }
                        return hours;
                    }
                };
REACTFS_NS_COMMON_END
#endif //REACTFS_TIME_UTILS_H
