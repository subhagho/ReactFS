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
                    /*!
                     * Base class for defining auto-generated datatypes derived from
                     * schema definitions.
                     */
                    class __base_type {
                    protected:
                        /// Parsed schema definition for this type.
                        const __complex_type *record_type = nullptr;
                        /// Source record, if type has been deserialized.
                        const record_struct *__data = nullptr;

                        const __native_type *get_field_type(const string &name) {
                            CHECK_NOT_NULL(record_type);
                            return record_type->get_field(name);
                        }

                    public:
                        /*!
                         * Base destructor.
                         */
                        virtual ~__base_type() {
                        }

                        /*!
                         * Get the parsed schema definition for this type.
                         *
                         * @return - Parsed schema definition.
                         */
                        const __complex_type *get_record_type() const {
                            return this->record_type;
                        }

                        /*!
                         * Base virtual method for deserialising this type instance
                         * from a record.
                         *
                         * @param __data - Serialized data record.
                         */
                        virtual void deserialize(const record_struct *__data) = 0;
                    };

                    class __mutable_base_type {
                    protected:
                        /// Parsed schema definition for this type.
                        const __complex_type *record_type = nullptr;

                        const __native_type *get_field_type(const string &name) {
                            CHECK_NOT_NULL(record_type);
                            return record_type->get_field(name);
                        }

                    public:
                        /*!
                         * Base destructor.
                         */
                        virtual ~__mutable_base_type() {
                        }

                        /*!
                         * Get the parsed schema definition for this type.
                         *
                         * @return - Parsed schema definition.
                         */
                        const __complex_type *get_record_type() const {
                            return this->record_type;
                        }

                        /*!
                         * Serialize this class instance.
                         *
                         * @return - Serialized data record.
                         */
                        virtual mutable_record_struct *serialize() = 0;
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_BASE_TYPE_H
