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

#ifndef WATERGATE_CALLBACK_H
#define WATERGATE_CALLBACK_H

#include "common.h"
#include "common_utils.h"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                /*!
                 * Enumeration for defining the call-stack state prior to invoking the callback(s)
                 */
                enum __callback_state_enum {
                    /// State is unknown
                            UNKNOWN,
                    /// State is success
                            SUCCESS,
                    /// State is ignored.
                            IGNORED,
                    /// Error occured prior to invoking this callback
                            ERROR
                };

                /*!
                 * Encapsulating class to propagate the execution state to a callback.
                 */
                class __callback_state {
                private:
                    /// Execution state
                    __callback_state_enum state = __callback_state_enum::UNKNOWN;
                    /// Exception handle, if any error occurred.
                    exception *error = nullptr;

                public:
                    /*!< destructor
                     * Dispose this callback instance.
                     */
                    ~__callback_state() {
                        dispose();
                    }

                    /*!
                     * Set the execution state prior to the callback being invoked.
                     *
                     * @param state - Execution state.
                     */
                    void set_state(__callback_state_enum state) {
                        this->state = state;
                    }

                    /*!
                     * Set the execution state to error and record the generated exception.
                     *
                     * @param error - Exception generated.
                     */
                    void set_error(exception *error) {
                        set_state(ERROR);
                        this->error = error;
                    }

                    /*!
                     * Get the execution state.
                     *
                     * @return - Execution state.
                     */
                    const __callback_state_enum get_state() const {
                        return state;
                    }

                    /*!
                     * Get the execption, if any.
                     *
                     * @return - Exception handle.
                     */
                    const exception *get_error() const {
                        return this->error;
                    }

                    void dispose() {
                        CHECK_AND_FREE(error);
                    }
                };

                /*!
                 * Base class for defining callback handlers. Callback handlers can be registered with alarms, signal
                 * handlers to invoke context specific functions in reaction to certain events.
                 *
                 * Note: The responsibility of freeing the handler(s) lies with the calling code. Pointers will not be
                 * automatically deleted.
                 */
                class __callback {
                protected:
                    /// Unique ID of this callback instance.
                    string uuid;
                    /// Context that needs to be passed to the callback while being invoked.
                    void *context = nullptr;
                    /// Execution state prior to invoking the callback.
                    __callback_state state;

                public:
                    /*!< constructor
                     * Default constructor of the callback.
                     *
                     * @return - Constructor
                     */
                    __callback() {
                        uuid = common_utils::uuid();
                    }

                    /*!< destructor
                     * Virtual base destructor.
                     */
                    virtual ~__callback() {
                        state.dispose();
                    }

                    /*!
                     * Get the UUID for this callback instance.
                     *
                     * @return - UUID of this instance.
                     */
                    const string get_uuid() const {
                        return uuid;
                    }

                    /*!
                     * Get the execution state this callback handler was invoked with.
                     *
                     * @return - Execution state.
                     */
                    const __callback_state get_state() const {
                        return state;
                    }

                    /*!
                     * Template method, which returns the context by typecasting to the required type.
                     *
                     * @return - Typecasted context value, if not null.
                     */
                    template<typename T>
                    T *get_context(T *t) const {
                        if (NOT_NULL(context)) {
                            T *tt = static_cast<T *>(context);
                            return tt;
                        }
                        return t;
                    }

                    /*!
                     * Set the context value, the context is used to pass data from the execution context
                     * to the callback. Data to be required for storing local context should be stored as
                     * member variables and not as the context.
                     *
                     * @param context - Context object.
                     */
                    void set_context(void *context) {
                        this->context = context;
                    }

                    /*!
                     * Set the execution state prior to invoking the callback.
                     *
                     * @param state - Execution state.
                     */
                    void set_state(__callback_state_enum state) {
                        this->state.set_state(state);
                    }

                    /*!
                     * Set the execution error raised prior to invoking the callback.
                     *
                     * @param error - Exception handle.
                     */
                    void set_error(exception *error) {
                        this->state.set_error(error);
                    }

                    /*!
                     * Virtual method to be implemented by sub-classes.
                     * This will be invoked in case of normal execution.
                     */
                    virtual void callback() = 0;

                    /*!
                     * Virtual method to be implemented by sub-classes.
                     * This will be invoked in case of error conditions.
                     */
                    virtual void error() = 0;

                    /*!
                     * Virtual method to be implemented by sub-classes.
                     * This will be invoked in case of error conditions.
                     *
                     * @param err - Exception handle of the error.
                     */
                    virtual void error(exception *err) = 0;
                };
            }
        }
    }
}

#endif //WATERGATE_CALLBACK_H
