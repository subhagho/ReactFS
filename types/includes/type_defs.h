//
// Created by Subhabrata Ghosh on 03/12/16.
//


/*
 * Copyright [2016] [Subhabrata Ghosh]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef REACTFS_TYPE_DEFS_H
#define REACTFS_TYPE_DEFS_H

#include <type_traits>
#include <unordered_map>

#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/time_utils.h"
#include "common/includes/base_error.h"
#include "core/includes/core.h"

#include "type_errors.h"
#include "types_common.h"
#include "schema_def.h"

using namespace REACTFS_NS_COMMON_PREFIX;

#define COLLECTION_SIZE_FACTOR 4

REACTFS_NS_CORE
                namespace types {

                    class __dt_struct : public __base_datatype_io {
                    private:
                        __complex_type *fields = nullptr;

                        record_struct *get_read_record() {
                            record_struct *ptr = new record_struct(fields);
                            CHECK_ALLOC(ptr, TYPE_NAME(record_struct));
                            return ptr;
                        }

                    public:
                        __dt_struct(__complex_type *fields) {
                            CHECK_NOT_NULL(fields);
                            this->type = __type_def_enum::TYPE_STRUCT;
                            this->fields = fields;
                        }

                        ~__dt_struct() {
                            CHECK_AND_FREE(fields);
                        }

                        void set_fields(__complex_type *fields) {
                            CHECK_NOT_NULL(fields);
                            this->fields = fields;
                        }

                        virtual uint64_t
                        read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            record_struct **T = (record_struct **) t;
                            *T = get_read_record();
                            CHECK_NOT_NULL(*T);
                            __pos pos;
                            pos.offset = offset;
                            pos.size = 0;

                            uint64_t *w_size = nullptr;
                            pos.size += buffer_utils::read<uint64_t>(buffer, &pos.offset, &w_size);
                            CHECK_NOT_NULL(w_size);
                            uint64_t size = *w_size;
                            POSTCONDITION(size <= max_length);

                            unordered_map<uint8_t, __native_type *> types = fields->get_fields();
                            CHECK_NOT_EMPTY(types);
                            unordered_map<uint8_t, __native_type *>::const_iterator iter;
                            while (pos.offset < max_length && size > 0) {
                                uint64_t r = 0;
                                uint8_t *ci = nullptr;
                                r = buffer_utils::read<uint8_t>(buffer, &pos.offset, &ci);
                                pos.size += r;
                                size -= r;
                                iter = types.find(*ci);
                                POSTCONDITION(iter != types.end());
                                __native_type *type = iter->second;
                                CHECK_NOT_NULL(type);
                                __base_datatype_io *handler = type->get_type_handler(__record_mode::RM_READ);
                                CHECK_NOT_NULL(handler);
                                void *value = nullptr;
                                r = handler->read(buffer, &value, pos.offset, max_length);
                                CHECK_NOT_NULL(value);
                                (*T)->add_field(type->get_index(), value);
                                pos.size += r;
                                pos.offset += r;
                                size -= r;
                            }
                            return pos.size;
                        }

                        virtual uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t buffer_size, ...) override {
                            const mutable_record_struct *rec = (const mutable_record_struct *) value;
                            CHECK_NOT_NULL(rec);
                            POSTCONDITION(rec->get_field_count() == fields->get_field_count());
                            long max_length = (long) buffer_size;
                            PRECONDITION(max_length > 0);

                            __pos pos;
                            pos.offset = offset;
                            pos.size = 0;

                            void *ptr = buffer_utils::increment_data_ptr(buffer, offset);
                            uint64_t *w_size = static_cast<uint64_t *>(ptr);
                            *w_size = 0;
                            pos.size += sizeof(uint64_t);
                            pos.offset += sizeof(uint64_t);
                            max_length -= pos.size;

                            unordered_map<uint8_t, __native_type *> types = fields->get_fields();
                            CHECK_NOT_EMPTY(types);
                            unordered_map<uint8_t, __native_type *>::const_iterator iter;
                            for (iter = types.begin(); iter != types.end(); iter++) {
                                PRECONDITION(max_length > 0);
                                __native_type *type = iter->second;
                                CHECK_NOT_NULL(type);
                                __base_datatype_io *handler = type->get_type_handler(__record_mode::RM_WRITE);
                                CHECK_NOT_NULL(handler);
                                const void *d = rec->get_field(type->get_index());
                                if (!type->is_valid_value(d)) {
                                    LOG_DEBUG("Field validation failed. [field=%s]",
                                              type->get_name().c_str());
                                    handler->print(d);
                                    throw TYPE_VALID_ERROR("Field validation failed. [field=%s]",
                                                           type->get_name().c_str());
                                }
                                if (IS_NULL(d)) {
                                    if (!type->is_nullable()) {
                                        throw TYPE_VALID_ERROR("Specified field is not nullable. [field=%s]",
                                                               type->get_canonical_name().c_str());
                                    }
                                    if (NOT_NULL(type->get_default_value())) {
                                        const __default *df = type->get_default_value();
                                        d = df->get_default();
                                        CHECK_NOT_NULL(d);
                                    }
                                }
                                if (NOT_NULL(d)) {
                                    if (NOT_NULL(type->get_constraint())) {
                                        __constraint *constraint = type->get_constraint();
                                        if (!constraint->validate(d)) {
                                            throw TYPE_VALID_ERROR("Constraint validation failed. [field=%s]",
                                                                   type->get_canonical_name().c_str());
                                        }
                                    }
                                    uint8_t ci = type->get_index();
                                    uint64_t r = buffer_utils::write<uint8_t>(buffer, &pos.offset, ci);
                                    pos.size += r;
                                    *w_size += r;
                                    max_length -= r;
                                    PRECONDITION(max_length > 0);
                                    r = handler->write(buffer, d, pos.offset, max_length);
                                    pos.offset += r;
                                    pos.size += r;
                                    *w_size += r;
                                    max_length -= r;
                                }
                            }
                            return pos.size;
                        }

                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            const mutable_record_struct *rec = (const mutable_record_struct *) data;
                            CHECK_NOT_NULL(rec);
                            POSTCONDITION(rec->get_field_count() == fields->get_field_count());

                            uint64_t t_size = sizeof(__version_header) + sizeof(uint64_t);
                            unordered_map<uint8_t, __native_type *> types = fields->get_fields();
                            CHECK_NOT_EMPTY(types);
                            unordered_map<uint8_t, __native_type *>::const_iterator iter;
                            for (iter = types.begin(); iter != types.end(); iter++) {
                                __native_type *type = iter->second;
                                CHECK_NOT_NULL(type);
                                __base_datatype_io *handler = type->get_type_handler(__record_mode::RM_WRITE);
                                CHECK_NOT_NULL(handler);
                                const void *d = rec->get_field(type->get_index());
                                if (!type->is_valid_value(d)) {
                                    throw TYPE_VALID_ERROR("Field validation failed. [field=%s]",
                                                           type->get_name().c_str());
                                }
                                t_size += handler->compute_size(d, -1);
                            }
                            return t_size;
                        }

                        virtual bool
                        compare(const void *source, const void *target, __constraint_operator oper) override {
                            throw BASE_ERROR("Compare only supported for native types.");
                        }

                        /*!
                         * Estimate the storage size of the given type value.
                         *
                         * @return - Return estimated storage size.
                         */
                        virtual uint32_t estimate_size() override {
                            CHECK_NOT_NULL(fields);
                            return fields->estimate_size();
                        }

                        virtual uint16_t
                        set_key_data(void *buffer, uint16_t offset, const void *value, uint8_t size, ...) override {
                            throw BASE_ERROR("Set key data not implemented for structs.");
                        }

                        void print(const void *value) const override {
                            const mutable_record_struct *st = static_cast<const mutable_record_struct *>(value);
                            for (uint8_t ii = 0; ii < st->get_field_count(); ii++) {
                                const __native_type *type = this->fields->get_field(ii);
                                void *fv = st->get_field(ii);
                                if (NOT_NULL(fv)) {
                                    TRACE("Field [%s]:", type->get_canonical_name().c_str());
                                    __base_datatype_io *handler = type->get_type_handler(__record_mode::RM_WRITE);
                                    handler->print(fv);
                                } else {
                                    TRACE("Field [%s] is NULL.", type->get_canonical_name().c_str());
                                }
                            }
                        }
                    };

                    template<typename __T>
                    class __dt_sized_type : public __datatype_io<__T> {
                    public:
                        __dt_sized_type(__type_def_enum type) : __datatype_io<__T>(type) {}

                        virtual uint32_t get_size(const void *value) const = 0;
                    };

                    /*!
                     * List type is a collection of basic types or structs.
                     * Internally implemented as a vector of type pointers.
                     *
                     * @tparam __T - Basic datatype of the array elements.
                     * @tparam __type - Datatype enum of the array element type.
                     */
                    template<typename __T, __type_def_enum __type>
                    class __dt_list : public __dt_sized_type<vector<__T *>> {
                    protected:
                        /// Datatype of the array elements (should be basic types or structs).
                        __type_def_enum inner_type = __type;
                        /// Datatype IO handler for the array type.
                        __base_datatype_io *type_handler = nullptr;

                    public:
                        /*!<constructor
                         * Default constructor.
                         *
                         */
                        __dt_list(__native_type *type) : __dt_sized_type<vector<__T *>>(
                                __type_def_enum::TYPE_LIST) {
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(this->inner_type));
                            if (this->inner_type == __type_def_enum::TYPE_STRUCT) {
                                CHECK_NOT_NULL(type);
                                __complex_type *ct = dynamic_cast<__complex_type *>(type);
                                CHECK_CAST(ct, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));
                                type_handler = new __dt_struct(ct);
                                CHECK_ALLOC(type_handler, TYPE_NAME(__dt_struct));
                            } else
                                type_handler = __type_defs_utils::get_type_handler(this->inner_type);
                            CHECK_NOT_NULL(type_handler);
                        }

                        /*!
                         * Get the element type of this array.
                         *
                         * @return - Element datatype.
                         */
                        __type_def_enum get_inner_type() {
                            return this->inner_type;
                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the typed vector.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        virtual uint64_t
                        read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            CHECK_NOT_NULL(type_handler);

                            __pos pos;
                            pos.offset = offset;
                            pos.size = 0;

                            uint16_t *r_count = nullptr;
                            pos.size = buffer_utils::read<uint16_t>(buffer, &pos.offset, &r_count);
                            CHECK_NOT_NULL(r_count);

                            vector<__T *> **list = (vector<__T *> **) t;

                            if (*r_count > 0) {
                                *list = new vector<__T *>();
                                CHECK_ALLOC(*list, TYPE_NAME(vector));

                                for (uint16_t ii = 0; ii < *r_count; ii++) {
                                    __T *t = nullptr;
                                    uint64_t r = type_handler->read(buffer, &t, pos.offset, max_length);
                                    CHECK_NOT_NULL(t);
                                    (*list)->push_back(t);
                                    pos.offset += r;
                                    pos.size += r;
                                }
                                return pos.size;
                            }
                            return sizeof(uint16_t);
                        }

                        /*!
                        * Write (serialize) data for the list (vector) to the binary output buffer.
                        *
                        * @param buffer - Output data buffer the data is to be copied to.
                        * @param value - Data value pointer to copy from.
                        * @param offset - Offset in the output buffer to start writing from.
                        * @param buffer_size - Max lenght of the output buffer.
                        * @return - Total number of bytes written.
                        */
                        virtual uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t buffer_size, ...) override {
                            CHECK_NOT_NULL(value);
                            CHECK_NOT_NULL(type_handler);

                            long max_length = (long) buffer_size;
                            PRECONDITION(max_length > 0);

                            __pos pos;
                            pos.offset = offset;
                            pos.size = 0;

                            const vector<__T *> *list = static_cast<const vector<__T *> *>( value);

                            uint16_t a_size = list->size();
                            pos.size = buffer_utils::write<uint16_t>(buffer, &pos.offset, a_size);
                            max_length -= pos.size;
                            PRECONDITION(max_length > 0);
                            if (a_size > 0) {
                                for (uint64_t ii = 0; ii < a_size; ii++) {
                                    PRECONDITION(max_length > 0);
                                    uint64_t r = type_handler->write(buffer, (*list)[ii], pos.offset, max_length);
                                    pos.offset += r;
                                    pos.size += r;
                                    max_length -= r;
                                }
                                return pos.size;
                            }
                            return sizeof(uint16_t);
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (data size occupied by the array).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *value, int size = -1) override {
                            if (IS_NULL(value)) {
                                return 0;
                            }
                            const vector<__T *> *data = static_cast< const vector<__T *> *>(value);
                            CHECK_NOT_NULL(data);
                            if (IS_EMPTY_P(data)) {
                                return 0;
                            }
                            __base_datatype_io *type_handler = __type_defs_utils::get_type_handler(this->inner_type);

                            CHECK_NOT_NULL(type_handler);
                            uint64_t t_size = sizeof(uint16_t);
                            for (uint32_t ii = 0; ii < data->size(); ii++) {
                                t_size += type_handler->compute_size((*data)[ii], -1);
                            }
                            return t_size;
                        }

                        /*!
                         * Comparisons not supported for lists. Will throw exception if called.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *source, const void *target, __constraint_operator oper) override {
                            throw BASE_ERROR("Compare only supported for native types.");
                        }

                        /*!
                         * Estimate the storage size of the given type value.
                         *
                         * @return - Return estimated storage size.
                         */
                        virtual uint32_t estimate_size() override {
                            uint32_t r_size = this->type_handler->estimate_size();
                            return (r_size * COLLECTION_SIZE_FACTOR);
                        }

                        virtual uint32_t get_size(const void *value) const override {
                            if (NOT_NULL(value)) {
                                const vector<__T *> *data = static_cast< const vector<__T *> *>(value);
                                CHECK_NOT_NULL(data);
                                if (IS_EMPTY_P(data)) {
                                    return 0;
                                }
                                return data->size();
                            }
                            return 0;
                        }

                        void print(const void *value) const override {

                            cout << "************************" << " VECTOR " << "************************\n";
                            if (inner_type == __type_def_enum::TYPE_STRUCT) {
                                const vector<mutable_record_struct *> *vv = static_cast<const vector<mutable_record_struct *> *>(value);
                                CHECK_CAST(vv, TYPE_NAME(void * ), TYPE_NAME(vector));

                                for (mutable_record_struct *v : *vv) {
                                    type_handler->print(v);
                                }
                            } else {
                                const vector<__T *> *data = static_cast< const vector<__T *> *>(value);
                                for (__T *t : *data) {
                                    type_handler->print(t);
                                }
                            }
                            cout << "************************" << " VECTOR " << "************************\n";
                        }
                    };

                    class __char_list : public __dt_list<char, __type_def_enum::TYPE_CHAR> {
                    public:
                        __char_list() : __dt_list<char, __type_def_enum::TYPE_CHAR>(nullptr) {}
                    };

                    class __short_list : public __dt_list<short, __type_def_enum::TYPE_SHORT> {
                    public:
                        __short_list() : __dt_list<short, __type_def_enum::TYPE_SHORT>(nullptr) {}
                    };

                    class __byte_list : public __dt_list<uint8_t, __type_def_enum::TYPE_BYTE> {
                    public:
                        __byte_list() : __dt_list<uint8_t, __type_def_enum::TYPE_BYTE>(nullptr) {}
                    };

                    class __int_list : public __dt_list<int, __type_def_enum::TYPE_INTEGER> {
                    public:
                        __int_list() : __dt_list<int, __type_def_enum::TYPE_INTEGER>(nullptr) {}
                    };

                    class __long_list : public __dt_list<long, __type_def_enum::TYPE_LONG> {
                    public:
                        __long_list() : __dt_list<long, __type_def_enum::TYPE_LONG>(nullptr) {}
                    };

                    class __float_list : public __dt_list<float, __type_def_enum::TYPE_FLOAT> {
                    public:
                        __float_list() : __dt_list<float, __type_def_enum::TYPE_FLOAT>(nullptr) {}
                    };

                    class __double_list : public __dt_list<double, __type_def_enum::TYPE_DOUBLE> {
                    public:
                        __double_list() : __dt_list<double, __type_def_enum::TYPE_DOUBLE>(nullptr) {}
                    };

                    class __timestamp_list : public __dt_list<uint64_t, __type_def_enum::TYPE_TIMESTAMP> {
                    public:
                        __timestamp_list() : __dt_list<uint64_t, __type_def_enum::TYPE_TIMESTAMP>(nullptr) {}
                    };

                    class __string_list : public __dt_list<char, __type_def_enum::TYPE_STRING> {
                    public:
                        __string_list() : __dt_list<char, __type_def_enum::TYPE_STRING>(nullptr) {}
                    };

                    class __text_list : public __dt_list<char, __type_def_enum::TYPE_TEXT> {
                    public:
                        __text_list() : __dt_list<char, __type_def_enum::TYPE_TEXT>(nullptr) {}
                    };

                    /*!
                     * Map datatype of key/value pairs.
                     * Keys are expected to be basic datatypes.
                     * Values can be any embedded datatype. (Cannot be array or list types).
                     * Internally implemented as an unordered_map.
                     *
                     * @tparam __K - Key datatype.
                     * @tparam __key_type - Key datatype enum.
                     * @tparam __V - Value datatype.
                     * @tparam __value_type - Value datatype enum.
                     */
                    template<typename __K, __type_def_enum __key_type, typename __V, __type_def_enum __value_type>
                    class __dt_map : public __dt_sized_type<unordered_map<__K, __V *>> {
                    protected:
                        /// Datatype enum of the map key.
                        __type_def_enum key_type = __key_type;
                        /// Datatype enum of the map value.
                        __type_def_enum value_type = __value_type;
                        /// Key type IO handler.
                        __base_datatype_io *kt_handler = nullptr;
                        /// Value type IO handler.
                        __base_datatype_io *vt_handler = nullptr;

                    public:
                        /*!<constructor
                         * Default empty constructor.
                         */
                        __dt_map(__native_type *v_type) : __dt_sized_type<unordered_map<__K, __V *>>(
                                __type_def_enum::TYPE_MAP) {
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(value_type));
                            PRECONDITION(__type_enum_helper::is_native(key_type));
                            kt_handler = __type_defs_utils::get_type_handler(this->key_type);
                            CHECK_NOT_NULL(kt_handler);
                            if (this->value_type == __type_def_enum::TYPE_STRUCT) {
                                CHECK_NOT_NULL(v_type);
                                __complex_type *ct = dynamic_cast<__complex_type *>(v_type);
                                CHECK_CAST(ct, TYPE_NAME(__native_type), TYPE_NAME(__complex_type));
                                vt_handler = new __dt_struct(ct);
                                CHECK_ALLOC(vt_handler, TYPE_NAME(__dt_struct));
                            } else
                                vt_handler = __type_defs_utils::get_type_handler(this->value_type);
                            CHECK_NOT_NULL(vt_handler);
                        }

                        /*!
                         * Get the key type enum.
                         *
                         * @return - Key type enum
                         */
                        __type_def_enum get_key_type() {
                            return this->key_type;
                        }

                        /*!
                         * Get the value type enum.
                         *
                         * @return - Value type enum.
                         */
                        __type_def_enum get_value_type() {
                            return this->value_type;
                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the typed unordered_map.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        virtual uint64_t
                        read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            CHECK_NOT_NULL(kt_handler);
                            CHECK_NOT_NULL(vt_handler);

                            __pos pos;
                            pos.offset = offset;
                            pos.size = 0;

                            uint16_t *r_count = nullptr;
                            pos.size = buffer_utils::read<uint16_t>(buffer, &pos.offset, &r_count);
                            CHECK_NOT_NULL(r_count);

                            if (r_count > 0) {
                                unordered_map<__K, __V *> **T = (unordered_map<__K, __V *> **) t;
                                *T = new unordered_map<__K, __V *>();

                                for (uint16_t ii = 0; ii < *r_count; ii++) {
                                    __K *key = nullptr;
                                    __V *value = nullptr;

                                    uint64_t r = kt_handler->read(buffer, &key, pos.offset, max_length);
                                    CHECK_NOT_NULL(key);
                                    pos.offset += r;
                                    pos.size += r;
                                    r = vt_handler->read(buffer, &value, pos.offset, max_length);
                                    CHECK_NOT_NULL(value);
                                    pos.offset += r;
                                    pos.size += r;

                                    (*T)->insert({*key, value});
                                }

                                return pos.size;
                            }
                            return sizeof(uint64_t);
                        }

                        /*!
                        * Write (serialize) data for the map (unordered_map) to the binary output buffer.
                        *
                        * @param buffer - Output data buffer the data is to be copied to.
                        * @param value - Data value pointer to copy from.
                        * @param offset - Offset in the output buffer to start writing from.
                        * @param buffer_size - Max lenght of the output buffer.
                        * @return - Total number of bytes written.
                        */
                        virtual uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t buffer_size, ...) override {
                            CHECK_NOT_NULL(kt_handler);
                            CHECK_NOT_NULL(vt_handler);
                            CHECK_NOT_NULL(value);
                            long max_length = (long) buffer_size;
                            PRECONDITION(max_length > 0);

                            __pos pos;
                            pos.offset = offset;
                            pos.size = 0;

                            const unordered_map<__K, __V *> *map = (const unordered_map<__K, __V *> *) value;
                            CHECK_NOT_NULL(map);

                            CHECK_NOT_NULL(kt_handler);
                            CHECK_NOT_NULL(vt_handler);

                            uint16_t m_size = map->size();
                            pos.size = buffer_utils::write<uint16_t>(buffer, &pos.offset, m_size);
                            max_length -= pos.size;

                            if (!map->empty()) {
                                for (auto iter = map->begin(); iter != map->end(); iter++) {
                                    PRECONDITION(max_length > 0);
                                    uint64_t r = kt_handler->write(buffer, &(iter->first), pos.offset, max_length);
                                    pos.offset += r;
                                    pos.size += r;
                                    max_length -= r;
                                    PRECONDITION(max_length > 0);
                                    r = vt_handler->write(buffer, iter->second, pos.offset, max_length);
                                    pos.offset += r;
                                    pos.size += r;
                                    max_length -= r;
                                }
                                return pos.size;
                            }
                            return sizeof(uint16_t);
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (data size occupied by the map).
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *value, int size = -1) override {
                            if (IS_NULL(value)) {
                                return 0;
                            }
                            const unordered_map<__K, __V *> *data = static_cast<const unordered_map<__K, __V *> *>(value);
                            CHECK_NOT_NULL(data);
                            if (IS_EMPTY_P(data)) {
                                return 0;
                            }
                            uint64_t t_size = sizeof(uint16_t);

                            for (auto iter = data->begin(); iter != data->end(); iter++) {
                                t_size += kt_handler->compute_size(&(iter->first), -1);
                                t_size += vt_handler->compute_size(iter->second, -1);
                            }
                            return t_size;
                        }

                        /*!
                         * Comparisons not supported for maps. Will throw exception if called.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *source, const void *target, __constraint_operator oper) override {
                            throw BASE_ERROR("Compare only supported for native types.");
                        }

                        /*!
                         * Estimate the storage size of the given type value.
                         *
                         * @return - Return estimated storage size.
                         */
                        virtual uint32_t estimate_size() override {
                            uint32_t k_size = kt_handler->estimate_size();
                            uint32_t v_size = vt_handler->estimate_size();
                            return ((k_size * COLLECTION_SIZE_FACTOR) + (v_size * COLLECTION_SIZE_FACTOR));
                        }


                        virtual uint32_t get_size(const void *value) const override {
                            if (IS_NULL(value)) {
                                return 0;
                            }
                            const unordered_map<__K, __V *> *data = static_cast<const unordered_map<__K, __V *> *>(value);
                            CHECK_NOT_NULL(data);
                            if (IS_EMPTY_P(data)) {
                                return 0;
                            }
                            return data->size();
                        }

                        void print(const void *value) const override {
                            cout << "************************" << " MAP " << "************************\n";
                            if (value_type == __type_def_enum::TYPE_STRUCT) {
                                const unordered_map<__K, mutable_record_struct *> *data = static_cast<const unordered_map<__K, mutable_record_struct *> *>(value);
                                for (auto iter = data->begin(); iter != data->end(); iter++) {
                                    const __K key = iter->first;
                                    const mutable_record_struct *value = iter->second;
                                    kt_handler->print(&key);
                                    vt_handler->print(value);
                                }
                            } else {
                                const unordered_map<__K, __V *> *data = static_cast<const unordered_map<__K, __V *> *>(value);
                                for (auto iter = data->begin(); iter != data->end(); iter++) {
                                    const __K key = iter->first;
                                    const __V *value = iter->second;

                                    kt_handler->print(&key);
                                    vt_handler->print(value);
                                }
                            }
                            cout << "************************" << " MAP " << "************************\n";
                        }
                    };

                    template<typename __V, __type_def_enum __value_type>
                    class string_key_map : public __dt_map<string, __type_def_enum::TYPE_STRING, __V, __value_type> {
                    public:
                        /*!<constructor
                         * Default empty constructor.
                         */
                        string_key_map(__native_type *v_type)
                                : __dt_map<string, __type_def_enum::TYPE_STRING, __V, __value_type>(
                                v_type) {

                        }

                        /*!
                        * Write (serialize) data for the map (unordered_map) to the binary output buffer.
                        *
                        * @param buffer - Output data buffer the data is to be copied to.
                        * @param value - Data value pointer to copy from.
                        * @param offset - Offset in the output buffer to start writing from.
                        * @param buffer_size - Max lenght of the output buffer.
                        * @return - Total number of bytes written.
                        */
                        virtual uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t buffer_size, ...) override {
                            CHECK_NOT_NULL(this->kt_handler);
                            CHECK_NOT_NULL(this->vt_handler);
                            CHECK_NOT_NULL(value);
                            long max_length = (long) buffer_size;
                            PRECONDITION(max_length > 0);

                            const unordered_map<string, __V *> *map = (const unordered_map<string, __V *> *) value;
                            CHECK_NOT_NULL(map);

                            __pos pos;
                            pos.offset = offset;
                            pos.size = 0;

                            CHECK_NOT_NULL(this->kt_handler);
                            CHECK_NOT_NULL(this->vt_handler);

                            uint16_t m_size = map->size();
                            pos.size = buffer_utils::write<uint16_t>(buffer, &pos.offset, m_size);
                            max_length -= pos.size;

                            if (!map->empty()) {
                                for (auto iter = map->begin(); iter != map->end(); iter++) {
                                    PRECONDITION(max_length > 0);
                                    string key = (iter->first);
                                    uint64_t r = this->kt_handler->write(buffer, key.c_str(), pos.offset, max_length);
                                    pos.offset += r;
                                    pos.size += r;
                                    max_length -= r;
                                    PRECONDITION(max_length > 0);
                                    r = this->vt_handler->write(buffer, iter->second, pos.offset, max_length);
                                    pos.offset += r;
                                    pos.size += r;
                                }
                                return pos.size;
                            }
                            return sizeof(uint16_t);
                        }

                        /*!
                        * Read (de-serialize) data from the binary format for the typed unordered_map.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @return - Total bytes consumed by this read.
                        */
                        virtual uint64_t
                        read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            CHECK_NOT_NULL(this->kt_handler);
                            CHECK_NOT_NULL(this->vt_handler);

                            __pos pos;
                            pos.offset = offset;
                            pos.size = 0;

                            uint16_t *r_count = nullptr;
                            pos.size = buffer_utils::read<uint16_t>(buffer, &pos.offset, &r_count);
                            CHECK_NOT_NULL(r_count);

                            if (r_count > 0) {
                                unordered_map<string, __V *> **T = (unordered_map<string, __V *> **) t;
                                *T = new unordered_map<string, __V *>();

                                for (uint16_t ii = 0; ii < *r_count; ii++) {
                                    CHARBUFF key = nullptr;
                                    __V *value = nullptr;

                                    uint64_t r = this->kt_handler->read(buffer, &key, pos.offset, max_length);
                                    CHECK_NOT_NULL(key);
                                    pos.offset += r;
                                    pos.size += r;
                                    r = this->vt_handler->read(buffer, &value, pos.offset, max_length);
                                    CHECK_NOT_NULL(value);
                                    pos.offset += r;
                                    pos.size += r;
                                    string ss = string(key);
                                    (*T)->insert({ss, value});
                                }

                                return pos.size;
                            }
                            return sizeof(uint64_t);
                        }

                    };

                    class ___read_struct_list : public __dt_list<record_struct, __type_def_enum::TYPE_STRUCT> {
                    public:
                        ___read_struct_list(__native_type *type)
                                : __dt_list<record_struct, __type_def_enum::TYPE_STRUCT>(type) {

                        }
                    };

                    class ___write_struct_list : public __dt_list<mutable_record_struct, __type_def_enum::TYPE_STRUCT> {
                    public:
                        ___write_struct_list(__native_type *type)
                                : __dt_list<mutable_record_struct, __type_def_enum::TYPE_STRUCT>(type) {

                        }
                    };

                }

REACTFS_NS_CORE_END

#endif //REACTFS_TYPE_DEFS_H
