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
    namespace watergate {
        namespace common {
            enum __callback_state_enum {
                UNKNOWN, SUCCESS, IGNORED, ERROR
            };

            class __callback_state {
            private:
                __callback_state_enum state = __callback_state_enum::UNKNOWN;
                exception *error = nullptr;

            public:
                ~__callback_state() {
                    dispose();
                }

                void set_state(__callback_state_enum state) {
                    this->state = state;
                }

                void set_error(exception *error) {
                    set_state(ERROR);
                    this->error = error;
                }

                const __callback_state_enum get_state() const {
                    return state;
                }

                const exception *get_error() const {
                    return this->error;
                }

                void dispose() {
                    CHECK_AND_FREE(error);
                }
            };

            class __callback {
            protected:
                void *context;
                __callback_state state;

            public:
                virtual ~__callback() {
                    state.dispose();
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
            };
        }
    }
}
#endif //WATERGATE_CALLBACK_H
