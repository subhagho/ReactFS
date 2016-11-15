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
#ifndef _CORE_H_
#define _CORE_H_

extern "C"
{
#ifdef WIN32
#include <Rpc.h>
#else
#include <uuid/uuid.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
}

#include <stdarg.h>  // For va_start, etc.
#include <memory>
#include <time.h>
#include <unistd.h>
#include <regex>

#include "common.h"

#define DEFAULT_UUID_SZIE 37
#define DEFAULT_HOSTNAME_SIZE 256

#define DUMMY_LOCAL_IP "127.0.0.1"
#define DUMMY_LOCAL_HOST "localhost"
#define DUMMY_IPV6_IP "::1"

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {

                class common_utils {
                public:
                    static string get_normalized_name(const string name) {
                        if (!IS_EMPTY(name)) {
                            char buff[name.length() + 1];
                            for (uint32_t ii = 0; ii < name.length(); ii++) {
                                char c = name[ii];
                                c = tolower(c);
                                if (isalnum(c)) {
                                    buff[ii] = c;
                                } else {
                                    buff[ii] = '_';
                                }
                            }
                            buff[name.length()] = 0;
                            return string(buff);
                        }
                        return EMPTY_STRING;
                    }

                    static long parse_duration(string input) {
                        regex r("([[:digit:]]+)\\s*([[:alpha:]]*)");
                        smatch m;
                        string s(input);
                        if (regex_search(s, m, r)) {
                            string num = m[1];
                            double d = atof(num.c_str());
                            if (d > 0) {
                                if (m.size() > 2) {
                                    string p = m[2];
                                    char c = p[0];
                                    c = toupper(c);
                                    if (c == 'S') {
                                        d *= 1000;
                                    } else if (c == 'M') {
                                        d *= 1000 * 60;
                                    } else if (c == 'H') {
                                        d *= 1000;
                                        d *= 60 * 60;
                                    } else if (c == 'D') {
                                        d *= 1000;
                                        d *= 60 * 60 * 24;
                                    }
                                }
                            }
                            return d;
                        }
                        return -1;
                    }

                    static double parse_size(string input) {
                        regex r("([[:digit:]]+)\\s*([[:alpha:]]*)");
                        smatch m;
                        string s(input);
                        if (regex_search(s, m, r)) {
                            string num = m[1];
                            double d = atof(num.c_str());
                            if (d > 0) {
                                if (m.size() > 2) {
                                    string p = m[2];
                                    char c = p[0];
                                    c = toupper(c);
                                    if (c == 'K') {
                                        d *= 1024;
                                    } else if (c == 'M') {
                                        d *= 1024 * 1024;
                                    } else if (c == 'G') {
                                        d *= 1024;
                                        d *= 1024 * 1024;
                                    } else if (c == 'T') {
                                        d *= 1024 * 1024;
                                        d *= 1024 * 1024;
                                    }
                                }
                            }
                            return d;
                        }
                        return -1;
                    }

                    static inline std::string &ltrim(std::string &s) {
                        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                                        std::not1(std::ptr_fun<int, int>(std::isspace))));
                        return s;
                    }

// trim from end
                    static inline std::string &rtrim(std::string &s) {
                        s.erase(std::find_if(s.rbegin(), s.rend(),
                                             std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
                        return s;
                    }

// trim from both ends
                    static inline std::string &trim(std::string &s) {
                        return ltrim(rtrim(s));
                    }

                    static void split(const string &s, char delim, vector<string> &elems) {
                        stringstream ss;
                        ss.str(s);
                        string item;
                        while (getline(ss, item, delim)) {
                            elems.push_back(item);
                        }
                    }

                    static vector<string> split(const string &s, char delim) {
                        vector<string> elems;
                        split(s, delim, elems);
                        return elems;
                    }

                    static std::string uuid() {
#ifdef WIN32
                        UUID uuid;
        UuidCreate ( &uuid );

        unsigned char * str;
        UuidToStringA ( &uuid, &str );

        std::string s( ( char* ) str );

        RpcStringFreeA ( &str );
#else
                        uuid_t uuid;
                        uuid_generate_random(uuid);
                        char s[DEFAULT_UUID_SZIE];
                        uuid_unparse(uuid, s);
#endif
                        return s;
                    }

                    static string *format_time(const time_t time, string fmt) {
                        if (IS_EMPTY(fmt)) {
                            fmt = DEFAULT_DATETIME_FORMAT;
                        }

                        char buff[DEFAULT_DATETIME_BUFFSIZE];
                        memset(buff, 0, DEFAULT_DATETIME_BUFFSIZE);
                        strftime(buff, DEFAULT_DATETIME_BUFFSIZE, fmt.c_str(), localtime(&time));

                        return new string(buff);
                    }

                    static string *format_time(const time_t time) {
                        return format_time(time, DEFAULT_DATETIME_FORMAT);
                    }

                    static time_t get_time(const string time, string fmt) {
                        assert(!IS_EMPTY(time));
                        if (IS_EMPTY(fmt)) {
                            fmt = DEFAULT_DATETIME_FORMAT;
                        }
                        struct tm t;
                        memset(&t, 0, sizeof(struct tm));

                        const char *ts = time.c_str();
                        const char *fs = fmt.c_str();

                        if (strptime(ts, fs, &t)) {
                            return timelocal(&t);
                        }

                        return -1;
                    }

                    static time_t get_time(const string time) {
                        return get_time(time, DEFAULT_DATETIME_FORMAT);
                    }

                    static std::string format(const string fmt_str, ...) {
                        int final_n, n =
                                ((int) fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */

                        std::string str;
                        std::unique_ptr<char[]> formatted;
                        va_list ap;
                        while (1) {
                            formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */

                            strcpy(&formatted[0], fmt_str.c_str());
                            va_start(ap, fmt_str);
                            final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
                            va_end(ap);
                            if (final_n < 0 || final_n >= n)
                                n += abs(final_n - n + 1);
                            else
                                break;
                        }
                        string ss = std::string(formatted.get());

                        return ss;
                    }

                    static string get_hostname() {
#ifdef WIN32
                        // No implemented yet
#else
                        // On *nix platforms
                        char hostname[DEFAULT_HOSTNAME_SIZE];
                        gethostname(hostname, DEFAULT_HOSTNAME_SIZE);

                        return string(hostname);
#endif
                        return DUMMY_LOCAL_HOST;
                    }

                    static string get_ip_string(short type) {
#ifdef WIN32
                        // No implemented yet
#else
                        // On *nix platforms
                        struct ifaddrs *ifa_ptr = nullptr;
                        struct ifaddrs *ifa = nullptr;
                        void *t_ptr = nullptr;

                        getifaddrs(&ifa_ptr);

                        for (ifa = ifa_ptr; NOT_NULL(ifa); ifa = ifa->ifa_next) {
                            if (!ifa->ifa_addr) {
                                continue;
                            }
                            if ((type <= 0 || type == AF_INET) && ifa->ifa_addr->sa_family == AF_INET) {
                                char mask[INET_ADDRSTRLEN];
                                void *mask_ptr = &((struct sockaddr_in *) ifa->ifa_netmask)->sin_addr;
                                if (IS_NULL(mask_ptr)) {
                                    continue;
                                }
                                inet_ntop(AF_INET, mask_ptr, mask, INET_ADDRSTRLEN);
                                if (strcmp(mask, "255.0.0.0") != 0) {
                                    // is a valid IP4 Address
                                    t_ptr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
                                    char buff[INET_ADDRSTRLEN];
                                    inet_ntop(AF_INET, t_ptr, buff, INET_ADDRSTRLEN);

                                    return string(buff);
                                }
                            } else if ((type <= 0 || type == AF_INET6) &&
                                       ifa->ifa_addr->sa_family == AF_INET6) { // check it is IP6
                                // is a valid IP6 Address
                                t_ptr = &((struct sockaddr_in6 *) ifa->ifa_addr)->sin6_addr;
                                char buff[INET6_ADDRSTRLEN];
                                inet_ntop(AF_INET6, t_ptr, buff, INET6_ADDRSTRLEN);

                                if (strncmp(buff, DUMMY_IPV6_IP, strlen(DUMMY_IPV6_IP)) == 0) {
                                    continue;
                                }

                                return string(buff);
                            }
                        }
                        if (NOT_NULL(ifa_ptr)) freeifaddrs(ifa_ptr);
#endif

                        return DUMMY_LOCAL_IP;
                    }
                };

                class temp_buffer {
                private:
                    void *buffer;
                    uint32_t size;
                    uint32_t used_size;

                public:

                    ~temp_buffer() {
                        FREE_PTR(buffer);
                        size = 0;
                    }

                    uint32_t get_size() {
                        return this->size;
                    }

                    void *allocate(uint32_t size) {
                        if (NOT_NULL(buffer)) {
                            if (size <= this->size) {
                                memset(buffer, 0, this->size);

                                return buffer;
                            } else {
                                free(buffer);
                                buffer = nullptr;
                            }
                        }
                        this->size = size;
                        buffer = (void *) malloc(size * sizeof(BYTE));
                        if (NOT_NULL(buffer)) {
                            memset(buffer, 0, size);
                        }
                        return buffer;
                    }

                    void *get_ptr() {
                        return buffer;
                    }

                    void set_used_size(uint32_t size) {
                        this->used_size = size;
                    }

                    uint32_t get_used_size() {
                        return this->used_size;
                    }

                    uint32_t copy(const void *source, uint32_t size) {
                        allocate(size);
                        if (NOT_NULL(buffer)) {
                            memcpy(buffer, source, (size_t) size);
                            this->used_size = size;
                        }
                        return 0;
                    }

                    uint32_t read(void *dest, uint32_t size) {
                        if (NOT_NULL(buffer)) {
                            uint32_t c_size = (size > this->size ? this->size : size);
                        }
                        return 0;
                    }
                };
            }
        }
    }
}

#endif // _CORE_H_