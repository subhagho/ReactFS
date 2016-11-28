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
#ifndef _COMMON_H_
#define _COMMON_H_

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>
#include <vector>
#include <assert.h>

#define NOT_NULL(v) (nullptr != v)
#define IS_NULL(v) (nullptr == v)
#define IS_EMPTY(v) (v.empty())
#define IS_EMPTY_P(v) (v->empty())
#define NOT_EMPTY_P(v) (nullptr != v && !v->empty())

#define CHECK_AND_FREE(p) do { \
    if (NOT_NULL(p)) { \
        delete (p); \
        p = nullptr; \
    } \
} while(0);

#define FREE_PTR(p) do {\
    if (NOT_NULL(p)) { \
        free(p); \
        p = nullptr; \
    } \
} while(0);

#define BOOL_TRUE "true"
#define BOOL_FALSE "false"

#define EMPTY_STRING ""

#define RETURN_SUCESS 0
#define RETURN_ERROR_UNKNOWN -1

#define DEFAULT_DATETIME_FORMAT "%Y-%m-%d %H:%M"
#define DEFAULT_TIMESTAMP_FORMAT "%Y-%m-%d %X"

#define DEFAULT_DATETIME_BUFFSIZE 32

#define SIZE_UUID 36
#define SIZE_USER_NAME 64
#define SIZE_MAX_PATH 256
#define SIZE_LOCK_NAME 32

#define DEFAULT_RESOURCE_MODE 0750

typedef uint8_t BYTE;
typedef uint8_t *BYTE_PTR;

#define PADCHAR(i, s) char __padchar_##i[s]
#define PADINT(i) uint32_t __padint_##i


using namespace std;

namespace com {
    namespace wookler {
        namespace reactfs {
            namespace common {
                struct __version_header__ {
                    uint16_t major = 0;
                    uint16_t minor = 0;
                };

                typedef __version_header__ __version_header;

                class version_utils {
                public:
                    static bool equals(__version_header v1, __version_header v2) {
                        return ((v1.major == v2.major) && (v1.minor == v2.minor));
                    }

                    static bool compatible(__version_header v1, __version_header v2) {
                        return (v1.major == v2.major);
                    }

                    static string get_version_string(__version_header v) {
                        char buff[64];
                        sprintf(buff, "%04d.%04d", v.major, v.minor);
                        return string(buff);
                    }
                };

            }
        }
    }
}

#define TYPE_NAME(X) #X

#endif