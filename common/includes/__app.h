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
