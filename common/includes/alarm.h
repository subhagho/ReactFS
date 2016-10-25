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
// Created by Subhabrata Ghosh on 20/09/16.
//

#ifndef WATERGATE_ALARM_H
#define WATERGATE_ALARM_H

#include <unistd.h>
#include <csignal>

#include "common_utils.h"
#include "__callback.h"
#include "base_error.h"

#define START_ALARM(t) do {\
    com::watergate::common::alarm a(t); \
    bool r = a.start(); \
    _assert(r); \
} while(0);

#define START_ALARM_WITH_CALLBACK(t, c) do {\
    com::watergate::common::alarm a(t, c); \
    bool r = a.start(); \
    _assert(r); \
} while(0);

namespace com {
    namespace watergate {
        namespace common {
            class alarm {
            private:
                __callback *_c = nullptr;
                long _d;

                static void signal_handler(int signal) {
                    throw BASE_ERROR("Alarm interrupted...");
                }

            public:
                alarm(long delta) {
                    this->_d = delta;
                }

                alarm(long delta, __callback *c) {
                    this->_d = delta;
                    this->_c = c;
                }

                alarm(string duration) {
                    assert(!IS_EMPTY(duration));
                    this->_d = common_utils::parse_duration(duration);
                    assert((this->_d > 0));
                }

                alarm(string duration, __callback *c) {
                    assert(!IS_EMPTY(duration));
                    this->_d = common_utils::parse_duration(duration);
                    assert((this->_d > 0));

                    this->_c = c;
                }

                bool start() {
                    assert(_d > 0);
                    signal(SIGINT, &signal_handler);

                    long s_time = _d * 1000;
                    usleep(s_time);

                    if (NOT_NULL(_c)) {
                        _c->set_state(__callback_state_enum::SUCCESS);
                        _c->callback();
                    }
                    return true;
                }
            };
        }
    }
}
#endif //WATERGATE_ALARM_H
