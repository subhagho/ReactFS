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
    namespace wookler {
        namespace reactfs {
            namespace common {
                /*!
                 * Class defines the application context.
                 */
                class __app {
                private:
                    /// Application name as specified in the configuration.
                    string name;
                    /// Application instance UUID.
                    string id;
                    /// Host IP address of the node where this application is running.
                    string host_ip;
                    /// Host name of the node where this application is running.
                    string host_name;
                    /// Start time when this application instance was started.
                    time_t start_time;

                public:
                    /*!<constructor
                     * Create a new application instance.
                     *
                     * @param name - Application name.
                     * @return
                     */
                    __app(string name) {
                        this->name = name;
                        this->start_time = time(nullptr);
                        this->id = common_utils::uuid();
                        this->host_ip = common_utils::get_ip_string(AF_INET);
                        this->host_name = common_utils::get_hostname();
                    }

                    /*!
                     * Get the start time of this application instanace.
                     *
                     * @return - Instance start time.
                     */
                    const time_t get_start_time() const {
                        return start_time;
                    }

                    /*!
                     * Get the name of this application instance.
                     *
                     * @return - Application name.
                     */
                    const string get_name() const {
                        return name;
                    }

                    /*!
                     * Get the unique ID of this application instance.
                     *
                     * @return - Unique instance ID.
                     */
                    const string get_id() const {
                        return id;
                    }

                    /*!
                     * Get the uptime for this application instance.
                     *
                     * @return - Instance uptime.
                     */
                    const long get_uptime() const {
                        time_t t = time(nullptr);
                        return (t - start_time);
                    }

                    /*!
                     * Get this host IP address of the node.
                     *
                     * @return - Host IP address.
                     */
                    const string get_host_ip() const {
                        return host_ip;
                    }

                    /*!
                     * Get this host name of the node.
                     *
                     * @return - Host name.
                     */
                    const string get_host_name() const {
                        return host_name;
                    }

                    /*!
                     * Get the default application directory name.
                     *
                     * @return - Application directory name.
                     */
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
}
#endif //WATERGATE_APP_H
