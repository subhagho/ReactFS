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
                        void *data = nullptr;
                    } __field_value;


                    class record_struct {
                    private:
                        uint8_t field_count = 0;
                        __field_value *data_ptr = nullptr;

                    public:
                        record_struct(const uint8_t field_count) {
                            PRECONDITION(field_count > 0);
                            this->field_count = field_count;
                            __field_value = (__field_value *) malloc(field_count * sizeof(__field_value));
                            CHECK_ALLOC(data_ptr, TYPE_NAME(__field_value));
                            void **ptr = data_ptr;
                            for (uint8_t ii = 0; ii < field_count; ii++) {
                                *ptr = nullptr;
                                ptr++;
                            }
                        }

                        virtual ~record_struct() {
                            __field_value *ptr = this->data_ptr;
                            for(uint8_t ii=0; ii < field_count; ii++) {
                                if (NOT_NULL(ptr)) {
                                    
                                    FREE_PTR(ptr);
                                }
                            }
                            FREE_PTR(data_ptr);
                        }

                        void add_field(uint8_t index, __type_def_enum type, void *data) {
                            PRECONDITION(index < this->field_count);
                            CHECK_NOT_NULL(data);

                            __field_value *ptr = this->data_ptr + index;
                            if (NOT_NULL(ptr)) {
                                free_field_data(ptr);
                                ptr->data = nullptr;
                            } else {
                                ptr = (__field_value *) malloc(sizeof(__field_value));
                                CHECK_ALLOC(ptr, TYPE_NAME(__field_value));
                            }
                            ptr->type = type;
                            ptr->data = data;
                        }

                        uint8_t get_field_count() {
                            return this->field_count;
                        }

                        virtual const void *get_field(uint8_t index) {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr + index;
                            if (NOT_NULL(ptr)) {
                                return ptr->data;
                            }
                            return nullptr;
                        }

                        __type_def_enum get_field_type(uint8_t index) {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr + index;
                            if (NOT_NULL(ptr)) {
                                return ptr->type;
                            }
                            return __type_def_enum::TYPE_UNKNOWN;
                        }

                        bool is_field_null(uint8_t index) {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr + index;

                            if (NOT_NULL(ptr)) {
                                return IS_NULL(ptr->data);
                            }
                            return true;
                        }
                    };
                }
REACTFS_NS_CORE_END
#endif //REACTFS_SHARED_STRUCTS_H
