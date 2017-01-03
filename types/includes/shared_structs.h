//
// Created by subho on 2/1/17.
//

#ifndef REACTFS_SHARED_STRUCTS_H
#define REACTFS_SHARED_STRUCTS_H

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"

#include "types_common.h"

REACTFS_NS_CORE
                namespace types {

                    typedef struct __field_value__ {
                        __type_def_enum type;
                        const void *data = nullptr;
                    } __field_value;


                    template<uint8_t __SIZE>
                    class record_struct {
                    protected:
                        uint8_t field_count = __SIZE;
                        __field_value *data_ptr[__SIZE] = nullptr;

                        void free_value_ptr(uint8_t index) {
                            __field_value *ptr = data_ptr[index];
                            if (NOT_NULL(ptr)) {
                                if (NOT_NULL(ptr->data)) {
                                    if (!__type_enum_helper::is_native(ptr->type)) {
                                        CHECK_AND_FREE(ptr->data);
                                    } else if (ptr->type == __type_def_enum::TYPE_STRING ||
                                               ptr->type == __type_def_enum::TYPE_TEXT) {
                                        FREE_PTR(ptr->data);
                                    }
                                }
                                FREE_PTR(ptr);
                                data_ptr[index] = nullptr;
                            }
                        }

                    public:
                        record_struct() {
                            for (uint8_t ii = 0; ii < field_count; ii++) {
                                data_ptr[ii] = nullptr;
                            }
                        }

                        virtual ~record_struct() {
                            for (uint8_t ii = 0; ii < field_count; ii++) {
                                free_value_ptr(ii);
                            }
                        }

                        void add_field(uint8_t index, __type_def_enum type, const void *data) {
                            PRECONDITION(index < this->field_count);
                            CHECK_NOT_NULL(data);
                            PRECONDITION(data_ptr[index] == nullptr);

                            __field_value *ptr = (__field_value *) malloc(sizeof(__field_value));
                            CHECK_ALLOC(ptr, TYPE_NAME(__field_value));
                            ptr->type = type;
                            ptr->data = data;

                            data_ptr[index] = ptr;
                        }

                        uint8_t get_field_count() {
                            return this->field_count;
                        }

                        virtual const void *get_field(uint8_t index) {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr[index];
                            if (NOT_NULL(ptr)) {
                                return ptr->data;
                            }
                            return nullptr;
                        }

                        __type_def_enum get_field_type(uint8_t index) {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr[index];
                            if (NOT_NULL(ptr)) {
                                return ptr->type;
                            }
                            return __type_def_enum::TYPE_UNKNOWN;
                        }

                        bool is_field_null(uint8_t index) {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr[index];

                            if (NOT_NULL(ptr)) {
                                return IS_NULL(ptr->data);
                            }
                            return true;
                        }
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_SHARED_STRUCTS_H
