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

#define BOOL_TRUE "true"
#define BOOL_FALSE "false"

#define EMPTY_STRING ""

#define RETURN_SUCESS 0
#define RETURN_ERROR_UNKNOWN -1

#define DEFAULT_DATETIME_FORMAT "%Y-%m-%d %H:%M"
#define DEFAULT_TIMESTAMP_FORMAT "%Y-%m-%d %X"

#define DEFAULT_DATETIME_BUFFSIZE 32

using namespace std;

#endif