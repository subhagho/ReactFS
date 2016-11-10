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
// Created by Subhabrata Ghosh on 06/09/16.
//

#ifndef WATERGATE_APP_H
#define WATERGATE_APP_H

#include <ctime>

#include "common.h"
#include "common_utils.h"

namespace com {
    namespace watergate {
        namespace common {
            /*!
             *
             */
            class __app {
            private:
                string name;
                string id;
                string host_ip;
                string host_name;
                time_t start_time;

            public:
                __app(string name) {
                    this->name = name;
                    this->start_time = time(nullptr);
                    this->id = common_utils::uuid();
                    this->host_ip = common_utils::get_ip_string(AF_INET);
                    this->host_name = common_utils::get_hostname();
                }

                const time_t get_start_time() const {
                    return start_time;
                }

                const string get_name() const {
                    return name;
                }

                const string get_id() const {
                    return id;
                }

                const long get_uptime() const {
                    time_t t = time(nullptr);
                    return (t - start_time);
                }

                const string get_host_ip() const {
                    return host_ip;
                }

                const string get_host_name() const {
                    return host_name;
                }

                string get_app_directory() const {
                    if (!IS_EMPTY(name)) {
                        return common_utils::get_normalized_name(name);
                    }
                    return EMPTY_STRING;
                }
            };
        }
    }
}
#endif //WATERGATE_APP_H
