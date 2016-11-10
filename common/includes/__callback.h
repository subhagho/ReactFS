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

                class __callback {
                protected:
                    string uuid;
                    void *context;
                    __callback_state state;

                public:
                    virtual __callback() {
                        uuid = common_utils::uuid();
                    }

                    virtual ~__callback() {
                        state.dispose();
                    }

                    const string get_uuid() const {
                        return uuid;
                    }

                    bool operator==(const __callback *c) const {
                        if (NOT_NULL(c)) {
                            return (uuid == c->uuid);
                        }
                        return false;
                    }

                    bool operator==(const __callback c) const {
                        return (uuid == c.uuid);
                    }

                    const __callback_state get_state() const {
                        return state;
                    }

                    template<typename T>
                    T *get_context(T *t) const {
                        if (NOT_NULL(context)) {
                            T *tt = static_cast<T *>(context);
                            return tt;
                        }
                        return t;
                    }

                    void set_context(void *context) {
                        this->context = context;
                    }

                    void set_state(__callback_state_enum state) {
                        this->state.set_state(state);
                    }

                    void set_error(exception *error) {
                        this->state.set_error(error);
                    }

                    virtual void callback() = 0;

                    virtual void error() = 0;
                };
            }
        }
    }
}
#endif //WATERGATE_CALLBACK_H
