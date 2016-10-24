//
// Created by Subhabrata Ghosh on 19/09/16.
//

#ifndef WATERGATE_TIMER_H_H
#define WATERGATE_TIMER_H_H

#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>

#include "common.h"
#include "common_utils.h"

using namespace std::chrono;

namespace com {
    namespace watergate {
        namespace common {
            class time_utils {
            public:
                static uint64_t now() {
                    milliseconds nt = duration_cast<milliseconds>(
                            system_clock::now().time_since_epoch()
                    );
                    return nt.count();
                }

                static long get_delta_time(long start) {
                    milliseconds nt = duration_cast<milliseconds>(
                            system_clock::now().time_since_epoch()
                    );
                    return (nt.count() - start);
                }

                static string get_time_string(long ts, string fmt) {
                    if (ts >= 0) {
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
                    return EMPTY_STRING;
                }

                static string get_time_string(uint64_t ts) {
                    return get_time_string(ts, DEFAULT_TIMESTAMP_FORMAT);
                }
            };

            class timer {
            private:
                time_point<system_clock> _t = system_clock::now().min();
                uint64_t elapsed = 0;

            public:
                void start() {
                    _t = system_clock::now();
                    elapsed = 0;
                }

                void pause() {
                    if (_t > system_clock::now().min()) {
                        milliseconds nt = duration_cast<milliseconds>(
                                system_clock::now().time_since_epoch()
                        );
                        milliseconds ot = duration_cast<milliseconds>(
                                _t.time_since_epoch()
                        );
                        milliseconds d = nt - ot;
                        elapsed += d.count();
                    }
                    _t = system_clock::now();
                }

                void restart() {
                    _t = system_clock::now();
                }

                void stop() {
                    if (_t > system_clock::now().min()) {
                        milliseconds nt = duration_cast<milliseconds>(
                                system_clock::now().time_since_epoch()
                        );
                        milliseconds ot = duration_cast<milliseconds>(
                                _t.time_since_epoch()
                        );
                        milliseconds d = nt - ot;
                        elapsed += d.count();
                    }
                    _t = system_clock::now();
                }

                uint64_t get_current_elapsed() {
                    if (_t > system_clock::now().min()) {
                        milliseconds nt = duration_cast<milliseconds>(
                                system_clock::now().time_since_epoch()
                        );
                        milliseconds ot = duration_cast<milliseconds>(
                                _t.time_since_epoch()
                        );
                        milliseconds d = nt - ot;
                        uint64_t e = elapsed + d.count();

                        return e;
                    }
                    return elapsed;
                }

                uint64_t get_elapsed() {
                    return elapsed;
                }
            };
        }
    }
}
#endif //WATERGATE_TIMER_H_H
