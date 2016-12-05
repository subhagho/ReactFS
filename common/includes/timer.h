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

#ifndef WATERGATE_TIMER_H_H
#define WATERGATE_TIMER_H_H

#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>

#include "common.h"
#include "common_utils.h"
#include "time_utils.h"

using namespace std::chrono;

REACTFS_NS_COMMON
                class timer {
                protected:
                    time_point<system_clock> _t = system_clock::now().min();
                    uint64_t elapsed = 0;

                public:
                    void start() {
                        _t = system_clock::now();
                        elapsed = 0;
                    }

                    virtual void pause() {
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
                        _t = system_clock::now().min();
                    }

                    void restart() {
                        _t = system_clock::now();
                    }

                    virtual void stop() {
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
                        _t = system_clock::now().min();
                    }

                    virtual uint64_t get_current_elapsed() {
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

                class nano_timer : public timer {
                public:
                    virtual void pause() override {
                        if (_t > system_clock::now().min()) {
                            nanoseconds nt = duration_cast<nanoseconds>(
                                    system_clock::now().time_since_epoch()
                            );
                            nanoseconds ot = duration_cast<nanoseconds>(
                                    _t.time_since_epoch()
                            );
                            nanoseconds d = nt - ot;
                            elapsed += d.count();
                        }
                        _t = system_clock::now().min();
                    }

                    virtual void stop() override {
                        if (_t > system_clock::now().min()) {
                            nanoseconds nt = duration_cast<nanoseconds>(
                                    system_clock::now().time_since_epoch()
                            );
                            nanoseconds ot = duration_cast<nanoseconds>(
                                    _t.time_since_epoch()
                            );
                            nanoseconds d = nt - ot;
                            elapsed += d.count();
                        }
                        _t = system_clock::now().min();
                    }

                    virtual uint64_t get_current_elapsed() override {
                        if (_t > system_clock::now().min()) {
                            nanoseconds nt = duration_cast<nanoseconds>(
                                    system_clock::now().time_since_epoch()
                            );
                            nanoseconds ot = duration_cast<nanoseconds>(
                                    _t.time_since_epoch()
                            );
                            nanoseconds d = nt - ot;
                            uint64_t e = elapsed + d.count();

                            return e;
                        }
                        return elapsed;
                    }

                };
REACTFS_NS_COMMON_END
#endif //WATERGATE_TIMER_H_H
