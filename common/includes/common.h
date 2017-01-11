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

#if defined(__unix)
#include <malloc.h>
#include <stdlib.h>
#endif

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>
#include <vector>
#include <assert.h>
#include <limits>
#if defined(__GNUC__)
#include <string.h>     // need both string and string.h for GCC
#include <stdlib.h>
#endif

#define NOT_NULL(v) (nullptr != v)
#define IS_NULL(v) (nullptr == v)
#define IS_EMPTY(v) (v.empty())
#define IS_EMPTY_P(v) (v == nullptr || v->empty())
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


#define __DEFAULT_DATETIME_FORMAT__ "%Y-%m-%d %H:%M"
#define __DEFAULT_TIMESTAMP_FORMAT__ "%Y-%m-%d %X"

#define DEFAULT_DATETIME_BUFFSIZE 32

#define SIZE_UUID 36
#define SIZE_MAX_NAME 64
#define SIZE_MAX_PATH 256
#define SIZE_LOCK_NAME 32

#define DEFAULT_RESOURCE_MODE 0750

typedef uint8_t BYTE;
typedef uint8_t *BYTE_PTR;
typedef char *CHARBUFF;

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
                    static bool equals(const __version_header &v1, const __version_header &v2) {
                        return (compare(v1, v2) == 0);
                    }

                    static bool compatible(const __version_header &v1, const __version_header &v2) {
                        return (v1.major == v2.major);
                    }

                    static string get_version_string(const __version_header &v) {
                        char buff[64];
                        sprintf(buff, "%04d.%04d", v.major, v.minor);
                        return string(buff);
                    }

                    static int compare(const __version_header &v1, const __version_header &v2) {
                        int r = v1.major - v2.major;
                        if (r == 0) {
                            r = v1.minor - v2.minor;
                        }
                        return r;
                    }
                };

                class common_consts {
                public:
                    static const string EMPTY_STRING;
                    static string DEFAULT_DATE_FORMAT;
                    static string DEFAULT_TIMESTAMP_FORMAT;
                };

            }
        }
    }
}

#define TYPE_NAME(X) #X
#define REACTFS_NS namespace com { namespace wookler { namespace reactfs {
#define REACTFS_NS_END }}}
#define REACTFS_NS_COMMON REACTFS_NS namespace common {
#define REACTFS_NS_COMMON_END REACTFS_NS_END }

#define REACTFS_NS_PREFIX com::wookler::reactfs
#define REACTFS_NS_COMMON_PREFIX REACTFS_NS_PREFIX::common

#ifndef ULONG_MAX
#define ULONG_MAX std::numeric_limits<unsigned long>::max()
#endif

#ifndef UCHAR_MAX
#define UCHAR_MAX std::numeric_limits<unsigned char>::max()
#endif

#ifndef USHRT_MAX
#define USHRT_MAX std::numeric_limits<unsigned short>::max()
#endif

#endif
