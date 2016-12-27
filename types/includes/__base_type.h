//
// Created by Subhabrata Ghosh on 23/12/16.
//

#ifndef REACTFS_BASE_TYPE_H
#define REACTFS_BASE_TYPE_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"

#include "type_defs.h"

#define FREE_NATIVE_LIST(list) do { \
    if (NOT_EMPTY_P(list)) { \
        for( auto ll : *(list)) { \
            FREE_PTR(ll); \
        } \
        list->clear(); \
    } \
    CHECK_AND_FREE(list); \
} while(0);

#define FREE_TYPE_LIST(list) do { \
    if (NOT_EMPTY_P(list)) { \
        for( auto ll : *(list)) { \
            CHECK_AND_FREE(ll); \
        } \
        list->clear(); \
    } \
    CHECK_AND_FREE(list); \
} while(0);

#define FREE_NATIVE_MAP(map) do { \
    if (NOT_EMPTY_P(map)) { \
        for (auto iter = map->begin(); iter != map->end(); iter++) { \
            FREE_PTR(iter->second); \
        } \
        map->clear(); \
    } \
    CHECK_AND_FREE(map); \
} while(0);

#define FREE_TYPE_MAP(map) do { \
    if (NOT_EMPTY_P(map)) { \
        for (auto iter = map->begin(); iter != map->end(); iter++) { \
            CHECK_AND_FREE(iter->second); \
        } \
        map->clear(); \
    } \
    CHECK_AND_FREE(map); \
} while(0);

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace types {
                    class __base_type {
                    protected:
                        bool __is_allocated = false;
                        __struct_datatype__ *__data = nullptr;

                        void free_data_ptr() {
                            free_data_ptr(this->__data, this->__is_allocated);
                        }

                    public:
                        virtual ~__base_type() {
                            this->__data = nullptr;
                        }

                        virtual void free_data_ptr(__struct_datatype__ *data, bool allocated) = 0;

                        virtual void deserialize(__struct_datatype__ *__data) = 0;

                        virtual __struct_datatype__ *serialize() = 0;
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_BASE_TYPE_H
