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
#include "log_utils.h"
#include "__signals.h"

/*!
 * Macro defined for running an alarm for the specified interval.
 *
 * @param t - Interval to set the alarm for.
 */
#define NEW_ALARM(t, i) \
    com::wookler::reactfs::common::__alarm_signal_handler _h_##i(__FILE__, __LINE__); \
    com::wookler::reactfs::common::__alarm _a_##i(t, &_h_##i); \


/*!
 * Macro defined for running an alarm for the specified alaram instance.
 *
 * @param a - Alarm object instance.
 */
#define WAIT_ALARM(a) do {\
    bool r = a.start(); \
    POSTCONDITION(r); \
} while(0);

/*!
 * Macro defined for running an alarm for the specified alaram instance.
 *
 * @param a - Alarm object pointer.
 */
#define WAIT_ALARM_P(a) do {\
    bool r = a->start(); \
    POSTCONDITION(r); \
} while(0);

#define START_ALARM(i) WAIT_ALARM(_a_##i)

/*!
 * Macro defined for running an alarm for the specified interval
 * with a callback to be invoked once the alarm is raised.
 *
 * @param t - Interval to set the alarm for.
 * @param c - Callback to invoke
 */
#define NEW_ALARM_WITH_CALLBACK(t, c, i) \
    com::wookler::reactfs::common::__alarm _a_##i(t, c);

REACTFS_NS_COMMON
                /*!
                 * Default interrupt signal handler class for alarms.
                 */
                class __alarm_signal_handler : public __callback {
                private:
                    /// Filename of the source where the alarm was called.
                    char const *filename;
                    /// Line no. of the source where the alarm was called.
                    uint32_t lineno;

                public:
                    /*!< constructor
                     * Default constructor with the source filename and line no.
                     *
                     * @param filename - Source filename of the caller.
                     * @param lineno  - Source line no. of the caller
                     * @return - Constructor
                     */
                    __alarm_signal_handler(char const *filename, uint32_t lineno) {
                        this->filename = filename;
                        this->lineno = lineno;
                    }

                    /*!
                     * Empty method. Doesn't do anything as only the error method is expected to be called.
                     */
                    void callback() override {

                    }

                    /*!
                     * Log the interrupt call.
                     */
                    void error() override {
                        LOG_ERROR("[file=%s][line=%lu] Alarm interrupted...", filename, lineno);
                    }

                    void error(exception *err) override {
                        LOG_ERROR("[file=%s][line=%lu][error=%s] Alarm interrupted...", filename, lineno, err->what());
                    }
                };

                /*!
                 * Class represents a timer based alarm. The invoking thread will go into a sleep mode
                 * once the alarm is started. If a callback is specified if will be invoked once the thread resumes,
                 * else control will be returned to the caller.
                 *
                 */
                class __alarm {
                private:
                    /// Callback handler to be invoked once the alaram is reaised.
                    __callback *_c = nullptr;
                    /// Duration the thread should sleep (in millisecods)
                    long _d;

                    /// Generic signal handler to catch sleep interrupts.
                    static void signal_handler(int signal) {
                        throw BASE_ERROR("Alarm interrupted...");
                    }

                public:

                    /*!< constructor
                     * Create an alarm instance with the specified timeout duration.
                     *
                     * @param delta - Duration to sleep (in milliseconds)
                     * @return - Constructor
                     */
                    __alarm(long delta) {
                        this->_d = delta;
                    }

                    /*!< constructor
                     * Create an alarm instance with the specified timeout duration and a callback handler
                     * to be invoked once the alarm is completed.
                     *
                     * @param delta - Duration to sleep (in milliseconds)
                     * @param c - Callback handler to be invoked
                     * @return - Constructor
                     */
                    __alarm(long delta, __callback *c) {
                        this->_d = delta;
                        this->_c = c;
                    }

                    /*!< constructor
                     * Create an alarm instance with the specified timeout duration.
                     *
                     * @param duration - String representation of duration [duration parser]: @ref common_utils::parse_duration()
                     * @return - Constructor
                     */
                    __alarm(string duration) {
                        PRECONDITION(!IS_EMPTY(duration));
                        this->_d = common_utils::parse_duration(duration);
                        POSTCONDITION((this->_d > 0));
                    }

                    /*!< constructor
                     * Create an alarm instance with the specified timeout duration and a callback handler
                     * to be invoked once the alarm is completed.
                     *
                     * @param duration - String representation of duration [duration parser]: @ref common_utils::parse_duration()
                     * @param c - Callback handler to be invoked
                     * @return - Constructor
                     */
                    __alarm(string duration, __callback *c) {
                        PRECONDITION(!IS_EMPTY(duration));
                        this->_d = common_utils::parse_duration(duration);
                        POSTCONDITION((this->_d > 0));

                        this->_c = c;
                    }

                    /*!
                     * Start the alarm. This call will cause the current thread
                     * to sleep for the duration specified in the constructor. If callback handler is set, then the handler
                     * will be invoked once the thread wakes up, else control will return directly to the caller.
                     *
                     * @return - True, if everything went fine.
                     */
                    bool start() {
                        if (NOT_NULL(_c)) {
                            // If callback is not null, use the callback to register to the signal handler.
                            __signals::add_handler(SIGINT, _c);
                        } else {
                            // Else register a generic signal handler.
                            signal(SIGINT, &signal_handler);
                        }

                        long s_time = _d * 1000;
                        usleep(s_time);

                        if (NOT_NULL(_c)) {
                            _c->set_state(__callback_state_enum::SUCCESS);
                            _c->callback();
                            // Un-register the callback from the signal handlers.
                            __signals::remove_handler(SIGINT, _c);
                        }
                        return true;
                    }
                };
REACTFS_NS_COMMON_END
#endif //WATERGATE_ALARM_H
