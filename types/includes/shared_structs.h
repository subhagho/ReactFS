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

                    typedef enum __record_mode__ {
                        RM_READ = 0, RM_WRITE = 1
                    }__record_mode;

                    typedef struct __field_value__ {
                        __type_def_enum type;
                        void *data = nullptr;
                    } __field_value;


                    class record_struct {
                    private:
                        __field_value *buffer = nullptr;
                        uint8_t field_count = 0;
                        __field_value **data_ptr;


                    public:
                        record_struct(uint8_t field_count) {
                            this->field_count = field_count;
                            data_ptr = new __field_value*[this->field_count];
                            CHECK_ALLOC(data_ptr, TYPE_NAME(__field_value *));

                            uint32_t size = sizeof(__field_value) * field_count;
                            buffer = (__field_value *) malloc(size);
                            CHECK_ALLOC(buffer, TYPE_NAME(__field_value__));

                            memset(buffer, 0, size);
                            for (uint8_t ii = 0; ii < field_count; ii++) {
                                data_ptr[ii] = nullptr;
                            }
                        }

                        ~record_struct() {
                            FREE_PTR(buffer);
                            FREE_PTR(data_ptr);
                        }

                        void add_field(uint8_t index, __type_def_enum type, void *data) {
                            PRECONDITION(index < this->field_count);
                            CHECK_NOT_NULL(data);
                            PRECONDITION(IS_NULL(data_ptr[index]));

                            __field_value *ptr = (buffer + index);
                            CHECK_NOT_NULL(ptr);
                            ptr->type = type;
                            ptr->data = data;

                            data_ptr[index] = ptr;
                        }

                        uint8_t get_field_count() const {
                            return this->field_count;
                        }

                        const void *get_field(uint8_t index) const {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr[index];
                            if (NOT_NULL(ptr)) {
                                return ptr->data;
                            }
                            return nullptr;
                        }

                        __type_def_enum get_field_type(uint8_t index) const {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr[index];
                            if (NOT_NULL(ptr)) {
                                return ptr->type;
                            }
                            return __type_def_enum::TYPE_UNKNOWN;
                        }

                        bool is_field_null(uint8_t index) const {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr[index];

                            if (NOT_NULL(ptr)) {
                                return IS_NULL(ptr->data);
                            }
                            return true;
                        }
                    };

                    class mutable_record_struct {
                    private:
                        __field_value *buffer = nullptr;
                        uint8_t field_count = 0;
                        __field_value **data_ptr;

                    public:
                        mutable_record_struct(uint8_t field_count) {
                            this->field_count = field_count;
                            data_ptr = new __field_value*[this->field_count];
                            CHECK_ALLOC(data_ptr, TYPE_NAME(__field_value *));

                            uint32_t size = sizeof(__field_value) * field_count;
                            buffer = (__field_value *) malloc(size);
                            CHECK_ALLOC(buffer, TYPE_NAME(__field_value__));

                            memset(buffer, 0, size);
                            for (uint8_t ii = 0; ii < field_count; ii++) {
                                data_ptr[ii] = nullptr;
                            }
                        }

                        ~mutable_record_struct() {
                            FREE_PTR(buffer);
                            FREE_PTR(data_ptr);
                        }

                        uint8_t get_field_count() const {
                            return this->field_count;
                        }

                        void add_field(uint8_t index, __type_def_enum type, void *data) {
                            PRECONDITION(index < this->field_count);
                            CHECK_NOT_NULL(data);

                            if (IS_NULL(data_ptr[index])) {
                                data_ptr[index] = (buffer + index);
                            }
                            data_ptr[index]->type = type;
                            data_ptr[index]->data = data;
                        }

                        void *get_field(uint8_t index) const {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr[index];
                            if (NOT_NULL(ptr)) {
                                return ptr->data;
                            }
                            return nullptr;
                        }

                        __type_def_enum get_field_type(uint8_t index) const {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr[index];
                            if (NOT_NULL(ptr)) {
                                return ptr->type;
                            }
                            return __type_def_enum::TYPE_UNKNOWN;
                        }

                        bool is_field_null(uint8_t index) const {
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
