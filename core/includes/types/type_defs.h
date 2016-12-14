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
#include "core/includes/types/type_errors.h"

#include "types_common.h"
#include "__constraints.h"
#include "schema_def.h"

using namespace REACTFS_NS_COMMON_PREFIX;

#define SIZE_MAX_TYPE_STRING 256

REACTFS_NS_CORE
                namespace types {

                    class __type_instance {
                    protected:
                        const __type_instance *parent = nullptr;
                        __native_type *type;
                        __base_datatype_io *handler;
                        uint8_t index;
                    public:
                        __type_instance(__native_type *type, const __type_instance *parent = nullptr) {
                            CHECK_NOT_NULL(type);
                            PRECONDITION(__type_enum_helper::is_native(type->get_datatype()));
                            this->parent = parent;
                            this->handler = __type_defs_utils::get_type_handler(type->get_datatype());
                            this->type = type;
                            this->index = type->get_index();
                        }

                        __type_instance(__native_type *type, __base_datatype_io *handler,
                                        __type_instance *parent = nullptr) {
                            CHECK_NOT_NULL(type);
                            CHECK_NOT_NULL(handler);
                            this->parent = parent;
                            this->handler = handler;
                            this->type = type;
                            this->index = type->get_index();
                        }

                        ~__type_instance() {
                            CHECK_AND_FREE(type);
                        }

                        uint8_t get_index() const {
                            return this->index;
                        }

                        __native_type *get_type() const {
                            return this->type;
                        }

                        __base_datatype_io *get_handler() const {
                            return this->handler;
                        }

                        const __type_instance *get_parent() const {
                            return this->parent;
                        }


                        string get_canonical_name() const {
                            if (IS_NULL(parent)) {
                                return type->get_name();
                            } else {
                                string p_name = parent->get_canonical_name();
                                CHECK_NOT_EMPTY(p_name);
                                return common_utils::format("%s.%s", p_name.c_str(), type->get_name().c_str());
                            }
                        }

                    };

                    typedef unordered_map<uint8_t, void *> __struct_datatype__;

                    class __dt_struct : public __base_datatype_io {
                    private:
                        __version_header *version;
                        vector<__type_instance *> fields;

                        void *get_field_value(const unordered_map<uint8_t, void *> *map, __type_instance *type) {
                            CHECK_NOT_NULL(type);
                            unordered_map<uint8_t, void *>::const_iterator iter = map->find(type->get_index());
                            if (iter == map->end())
                                return nullptr;
                            return iter->second;
                        }

                    public:
                        __dt_struct() {
                            this->type = __type_def_enum::TYPE_STRUCT;
                        }

                        __dt_struct(__version_header version) {
                            this->version = (__version_header *) malloc(sizeof(__version_header));
                            CHECK_ALLOC(this->version, TYPE_NAME(__version_header));
                            this->version->major = version.major;
                            this->version->minor = version.minor;
                            this->type = __type_def_enum::TYPE_STRUCT;
                        }

                        ~__dt_struct() {
                            if (!IS_EMPTY(fields)) {
                                vector<__type_instance *>::iterator iter;
                                for (iter = fields.begin(); iter != fields.end(); iter++) {
                                    CHECK_AND_FREE(*iter);
                                }
                                fields.clear();
                            }
                            FREE_PTR(this->version);
                        }

                        void add_field(uint16_t index, __type_instance *type) {
                            CHECK_NOT_NULL(type);
                            if (index == fields.size()) {
                                fields.push_back(type);
                            } else if (index > fields.size()) {
                                for (uint16_t ii = fields.size(); ii <= index; ii++) {
                                    fields.push_back(nullptr);
                                }
                                fields[index] = type;
                            } else {
                                PRECONDITION(IS_NULL(fields[index]));
                                fields[index] = type;
                            }
                        }

                        bool remove_field(string name) {
                            if (!IS_EMPTY(fields)) {
                                vector<__type_instance *>::iterator iter;
                                for (iter = fields.begin(); iter != fields.end(); iter++) {
                                    __type_instance *t = *iter;
                                    if (t->get_canonical_name() == name) {
                                        break;
                                    }
                                }
                                if (iter != fields.end()) {
                                    fields.erase(iter);
                                    return true;
                                }
                            }
                            return false;
                        }

                        virtual uint64_t
                        read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            __version_header *v = static_cast<__version_header *>(ptr);
                            CHECK_NOT_NULL(v);

                            POSTCONDITION(version_utils::compatible(*(this->version), *v));
                            unordered_map<uint8_t, void *> **T = (unordered_map<uint8_t, void *> **) t;
                            *T = new unordered_map<uint8_t, void *>();

                            vector<__type_instance *>::iterator iter;
                            uint64_t r_offset = offset + sizeof(__version_header);
                            uint64_t t_size = sizeof(__version_header);
                            while (r_offset < max_length) {
                                ptr = common_utils::increment_data_ptr(buffer, r_offset);
                                uint8_t *ci = (uint8_t *) ptr;
                                POSTCONDITION(*ci < fields.size());
                                __type_instance *type = fields[*ci];
                                CHECK_NOT_NULL(type);
                                __base_datatype_io *handler = type->get_handler();
                                CHECK_NOT_NULL(handler);
                                r_offset += sizeof(uint8_t);
                                t_size += sizeof(uint8_t);
                                uint64_t r = 0;
                                void *value = nullptr;
                                if (type->get_type()->get_datatype() == __type_def_enum::TYPE_ARRAY) {
                                    __sized_type *st = static_cast<__sized_type *>(type->get_type());
                                    const uint32_t a_size = st->get_max_size();
                                    r = handler->read(buffer, &value, r_offset, max_length, a_size);
                                } else {
                                    r = handler->read(buffer, &value, r_offset, max_length);
                                }
                                (*T)->insert({type->get_index(), value});
                                t_size += r;
                                r_offset += r;
                            }
                            return t_size;
                        }

                        virtual uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            const unordered_map<uint8_t, void *> *map = (const unordered_map<uint8_t, void *> *) value;
                            CHECK_NOT_EMPTY_P(map);
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            memcpy(ptr, this->version, sizeof(__version_header));
                            uint64_t r_offset = offset + sizeof(__version_header);
                            uint64_t t_size = sizeof(__version_header);

                            vector<__type_instance *>::iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                __base_datatype_io *handler = (*iter)->get_handler();
                                CHECK_NOT_NULL(handler);
                                void *d = get_field_value(map, *iter);
                                if (!(*iter)->get_type()->is_valid_value(d)) {
                                    throw TYPE_VALID_ERROR("Field validation failed. [field=%s]",
                                                           (*iter)->get_type()->get_name().c_str());
                                }
                                __native_type *ntype = (*iter)->get_type();
                                CHECK_NOT_NULL(ntype);
                                if (IS_NULL(d)) {
                                    if (!ntype->is_nullable()) {
                                        throw TYPE_VALID_ERROR("Specified field is not nullable. [field=%s]",
                                                               (*iter)->get_canonical_name().c_str());
                                    }
                                    if (NOT_NULL(ntype->get_default_value())) {
                                        const __default *df = ntype->get_default_value();
                                        df->set_default(d);
                                    }
                                }
                                if (NOT_NULL(d)) {
                                    if (NOT_NULL(ntype->get_constraint())) {
                                        __constraint *constraint = ntype->get_constraint();
                                        if (!constraint->validate(d)) {
                                            throw TYPE_VALID_ERROR("Constraint validation failed. [field=%s]",
                                                                   (*iter)->get_canonical_name().c_str());
                                        }
                                    }
                                    uint64_t r = handler->write(buffer, d, r_offset, max_length);
                                    r_offset += r;
                                    t_size += r;
                                }
                            }
                            return t_size;
                        }

                        virtual uint64_t compute_size(const void *data, int size = 0) override {
                            if (IS_NULL(data)) {
                                return 0;
                            }
                            const unordered_map<uint8_t, void *> *map = (const unordered_map<uint8_t, void *> *) data;
                            CHECK_NOT_EMPTY_P(map);
                            uint64_t t_size = sizeof(__version_header);
                            vector<__type_instance *>::const_iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                __base_datatype_io *handler = (*iter)->get_handler();
                                CHECK_NOT_NULL(handler);
                                void *d = get_field_value(map, *iter);
                                if (!(*iter)->get_type()->is_valid_value(d)) {
                                    throw TYPE_VALID_ERROR("Field validation failed. [field=%s]",
                                                           (*iter)->get_type()->get_name().c_str());
                                }
                                t_size += handler->compute_size(d, -1);
                            }
                            return t_size;
                        }

                        virtual bool
                        compare(const void *source, void *target, __constraint_operator oper) override {
                            throw BASE_ERROR("Compare only supported for native types.");
                        }
                    };


                    /*!
                     * Array type is a size bound collection of basic types or structs.
                     *
                     * Should generally be initialized
                     * as an array rather than a double pointer.
                     *
                     * @tparam __T - Basic datatype of the array elements.
                     * @tparam __type - Datatype enum of the array element type.
                     */
                    template<typename __T, __type_def_enum __type>
                    class __dt_array : public __datatype_io<__T *> {
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
                        __dt_array() : __datatype_io<__T *>(
                                __type_def_enum::TYPE_ARRAY) {
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(this->inner_type));
                            if (this->inner_type == __type_def_enum::TYPE_STRUCT) {
                                type_handler = new __dt_struct();
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
                        * Read (de-serialize) data from the binary format for the typed array.
                        *
                        * @param buffer - Source data buffer (binary data)
                        * @param t - Pointer to map the output data to.
                        * @param offset - Start offset where the buffer is to be read from.
                        * @param max_length - Max length of the data in the buffer.
                        * @param size - Size of the input array (number of records).
                        * @return - Total bytes consumed by this read.
                        */
                        virtual uint64_t
                        read(void *buffer, void *t, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(t);
                            CHECK_NOT_NULL(type_handler);

                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint64_t r_count = *((uint64_t *) ptr);
                            __T **d_ptr = (__T **) t;
                            va_list vl;
                            va_start(vl, max_length);
                            uint64_t a_size = va_arg(vl, uint64_t);
                            va_end(vl);

                            PRECONDITION(a_size >= r_count);
                            if (a_size > 0) {
                                uint64_t r_offset = offset + sizeof(uint64_t);
                                uint64_t t_size = sizeof(uint64_t);
                                for (uint64_t ii = 0; ii < r_count; ii++) {
                                    uint64_t r = type_handler->read(buffer, d_ptr[ii], r_offset, max_length);
                                    r_offset += r;
                                    t_size += r;
                                }
                                return t_size;
                            }
                            return sizeof(uint64_t);
                        }

                        /*!
                        * Write (serialize) data for the array to the binary output buffer.
                        *
                        * @param buffer - Output data buffer the data is to be copied to.
                        * @param value - Data value pointer to copy from.
                        * @param offset - Offset in the output buffer to start writing from.
                        * @param max_length - Max lenght of the output buffer.
                        * @param size - Size of the input array (number of records).
                        * @return - Total number of bytes written.
                        */
                        virtual uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(type_handler);
                            CHECK_NOT_NULL(value);
                            va_list vl;
                            va_start(vl, max_length);
                            uint64_t a_size = va_arg(vl, uint64_t);
                            va_end(vl);

                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            memcpy(ptr, &a_size, sizeof(uint64_t));

                            if (a_size > 0) {
                                __T * const *d_ptr = static_cast<__T * const *>(value);
                                uint64_t r_offset = offset + sizeof(uint64_t);
                                uint64_t t_size = sizeof(uint64_t);
                                for (uint64_t ii = 0; ii < a_size; ii++) {
                                    uint64_t r = type_handler->write(buffer, d_ptr[ii], r_offset, max_length);
                                    r_offset += r;
                                    t_size += r;
                                }
                                return t_size;
                            }
                            return sizeof(uint64_t);
                        }

                        /*!
                         * Compute the storage size of the given type value.
                         *
                         * @param data - Value of the type (data size occupied by the array).
                         * @param size - Must be specified if value is a double pointer. If it's an array
                         *              size can be computed.
                         * @return - Return storage size, if null returns 0.
                         */
                        virtual uint64_t compute_size(const void *value, int size = -1) override {
                            if (IS_NULL(value)) {
                                return 0;
                            }

                            __T * const *data = static_cast<__T * const *>(value);
                            CHECK_NOT_NULL(data);
                            __base_datatype_io *type_handler = __type_defs_utils::get_type_handler(this->inner_type);
                            if (size < 0) {
                                size = (sizeof(data) / sizeof(__T *));
                            }
                            CHECK_NOT_NULL(type_handler);
                            uint64_t t_size = sizeof(uint64_t);
                            for (int ii = 0; ii < size; ii++) {
                                t_size += type_handler->compute_size(data[ii], -1);
                            }
                            return t_size;
                        }

                        /*!
                         * Comparisons not supported for arrays. Will throw exception if called.
                         *
                         * @param source - Source value pointer
                         * @param target - Target value pointer
                         * @return - Is comparision true?
                         */
                        virtual bool
                        compare(const void *source, void *target, __constraint_operator oper) override {
                            throw BASE_ERROR("Compare only supported for native types.");
                        }
                    };

                    typedef __dt_array<char, __type_def_enum::TYPE_CHAR> __char_array;
                    typedef __dt_array<short, __type_def_enum::TYPE_SHORT> __short_array;
                    typedef __dt_array<uint8_t, __type_def_enum::TYPE_BYTE> __byte_array;
                    typedef __dt_array<int, __type_def_enum::TYPE_INTEGER> __int_array;
                    typedef __dt_array<long, __type_def_enum::TYPE_LONG> __long_array;
                    typedef __dt_array<float, __type_def_enum::TYPE_FLOAT> __float_array;
                    typedef __dt_array<double, __type_def_enum::TYPE_DOUBLE> __double_array;
                    typedef __dt_array<uint64_t, __type_def_enum::TYPE_TIMESTAMP> __timestamp_array;
                    typedef __dt_array<string, __type_def_enum::TYPE_STRING> __string_array;
                    typedef __dt_array<string, __type_def_enum::TYPE_TEXT> __text_array;

                    /*!
                     * List type is a collection of basic types or structs.
                     * Internally implemented as a vector of type pointers.
                     *
                     * @tparam __T - Basic datatype of the array elements.
                     * @tparam __type - Datatype enum of the array element type.
                     */
                    template<typename __T, __type_def_enum __type>
                    class __dt_list : public __datatype_io<vector<__T *>> {
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
                        __dt_list() : __datatype_io<vector<__T *>>(
                                __type_def_enum::TYPE_LIST) {
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(this->inner_type));
                            if (this->inner_type == __type_def_enum::TYPE_STRUCT) {
                                type_handler = new __dt_struct();
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

                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint64_t r_count = *((uint64_t *) ptr);
                            vector<__T *> **list = (vector<__T *> **) t;

                            if (r_count > 0) {
                                *list = new vector<__T *>();
                                CHECK_ALLOC(*list, TYPE_NAME(vector));

                                uint64_t r_offset = offset + sizeof(uint64_t);
                                uint64_t t_size = sizeof(uint64_t);
                                for (uint64_t ii = 0; ii < r_count; ii++) {
                                    __T *t = nullptr;
                                    uint64_t r = type_handler->read(buffer, &t, r_offset, max_length);
                                    CHECK_NOT_NULL(t);
                                    (*list)->push_back(t);
                                    r_offset += r;
                                    t_size += r;
                                }
                                return t_size;
                            }
                            return sizeof(uint64_t);
                        }

                        /*!
                        * Write (serialize) data for the list (vector) to the binary output buffer.
                        *
                        * @param buffer - Output data buffer the data is to be copied to.
                        * @param value - Data value pointer to copy from.
                        * @param offset - Offset in the output buffer to start writing from.
                        * @param max_length - Max lenght of the output buffer.
                        * @return - Total number of bytes written.
                        */
                        virtual uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(value);
                            CHECK_NOT_NULL(type_handler);

                            const vector<__T *> *list = static_cast<const vector<__T *> *>( value);

                            uint64_t a_size = list->size();
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            memcpy(ptr, &a_size, sizeof(uint64_t));

                            if (a_size > 0) {
                                __T **d_ptr = (__T **) value;
                                uint64_t r_offset = offset + sizeof(uint64_t);
                                uint64_t t_size = sizeof(uint64_t);
                                for (uint64_t ii = 0; ii < a_size; ii++) {
                                    uint64_t r = type_handler->write(buffer, (*list)[ii], r_offset, max_length);
                                    r_offset += r;
                                    t_size += r;
                                }
                                return t_size;
                            }
                            return sizeof(uint64_t);
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
                            uint64_t t_size = sizeof(uint64_t);
                            for (int ii = 0; ii < data->size(); ii++) {
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
                        compare(const void *source, void *target, __constraint_operator oper) override {
                            throw BASE_ERROR("Compare only supported for native types.");
                        }
                    };

                    typedef __dt_list<char, __type_def_enum::TYPE_CHAR> __char_list;
                    typedef __dt_list<short, __type_def_enum::TYPE_SHORT> __short_list;
                    typedef __dt_list<uint8_t, __type_def_enum::TYPE_BYTE> __byte_list;
                    typedef __dt_list<int, __type_def_enum::TYPE_INTEGER> __int_list;
                    typedef __dt_list<long, __type_def_enum::TYPE_LONG> __long_list;
                    typedef __dt_list<float, __type_def_enum::TYPE_FLOAT> __float_list;
                    typedef __dt_list<double, __type_def_enum::TYPE_DOUBLE> __double_list;
                    typedef __dt_list<uint64_t, __type_def_enum::TYPE_TIMESTAMP> __timestamp_list;
                    typedef __dt_list<string, __type_def_enum::TYPE_STRING> __string_list;
                    typedef __dt_list<string, __type_def_enum::TYPE_TEXT> __text_list;

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
                    class __dt_map : public __datatype_io<unordered_map<__K, __V *>> {
                    private:
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
                        __dt_map() : __datatype_io<unordered_map<__K, __V *>>(
                                __type_def_enum::TYPE_MAP) {
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(value_type));
                            PRECONDITION(__type_enum_helper::is_native(key_type));
                            kt_handler = __type_defs_utils::get_type_handler(this->key_type);
                            CHECK_NOT_NULL(kt_handler);
                            if (this->value_type == __type_def_enum::TYPE_STRUCT) {
                                vt_handler = new __dt_struct();
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

                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint64_t r_count = *((uint64_t *) ptr);
                            if (r_count > 0) {
                                unordered_map<__K, __V *> **T = (unordered_map<__K, __V *> **) t;
                                *T = new unordered_map<__K, __V *>();

                                uint64_t r_offset = offset + sizeof(uint64_t);
                                uint64_t t_size = sizeof(uint64_t);

                                for (uint64_t ii = 0; ii < r_count; ii++) {
                                    __K *key = nullptr;
                                    __V *value = nullptr;

                                    uint64_t r = kt_handler->read(buffer, key, r_offset, max_length);
                                    CHECK_NOT_NULL(key);
                                    r_offset += r;
                                    t_size += r;
                                    r = vt_handler->read(buffer, value, r_offset, max_length);
                                    CHECK_NOT_NULL(value);
                                    r_offset += r;
                                    t_size += r;

                                    (*T)->insert({*key, value});
                                }

                                return t_size;
                            }
                            return sizeof(uint64_t);
                        }

                        /*!
                        * Write (serialize) data for the map (unordered_map) to the binary output buffer.
                        *
                        * @param buffer - Output data buffer the data is to be copied to.
                        * @param value - Data value pointer to copy from.
                        * @param offset - Offset in the output buffer to start writing from.
                        * @param max_length - Max lenght of the output buffer.
                        * @return - Total number of bytes written.
                        */
                        virtual uint64_t
                        write(void *buffer, const void *value, uint64_t offset, uint64_t max_length, ...) override {
                            CHECK_NOT_NULL(kt_handler);
                            CHECK_NOT_NULL(vt_handler);
                            CHECK_NOT_NULL(value);

                            const unordered_map<__K, __V *> *map = (const unordered_map<__K, __V *> *) value;
                            CHECK_NOT_NULL(map);

                            CHECK_NOT_NULL(kt_handler);
                            CHECK_NOT_NULL(vt_handler);

                            uint64_t m_size = map->size();
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            memcpy(ptr, &m_size, sizeof(uint64_t));

                            if (!map->empty()) {

                                uint64_t r_offset = offset + sizeof(uint64_t);
                                uint64_t t_size = sizeof(uint64_t);

                                for (auto iter = map->begin(); iter != map->end(); iter++) {
                                    uint64_t r = kt_handler->write(buffer, &(iter->first), r_offset, max_length);
                                    r_offset += r;
                                    t_size += r;
                                    r = vt_handler->write(buffer, iter->second, r_offset, max_length);
                                    r_offset += r;
                                    t_size += r;
                                }
                                return t_size;
                            }
                            return sizeof(uint64_t);
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
                            uint64_t t_size = sizeof(uint64_t);
                            __base_datatype_io *kt_handler = __type_defs_utils::get_type_handler(this->key_type);
                            CHECK_NOT_NULL(kt_handler);
                            __base_datatype_io *vt_handler = __type_defs_utils::get_type_handler(this->value_type);
                            CHECK_NOT_NULL(vt_handler);

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
                        compare(const void *source, void *target, __constraint_operator oper) override {
                            throw BASE_ERROR("Compare only supported for native types.");
                        }
                    };


                    typedef __dt_array<__struct_datatype__, __type_def_enum::TYPE_STRUCT> __struct_array;
                    typedef __dt_list<__struct_datatype__, __type_def_enum::TYPE_STRUCT> __struct_list;

                    class __array_init_utils {
                    private:
                        static unordered_map<string, __base_datatype_io *> array_handlers;

                    public:
                        static __base_datatype_io *get_handler(__type_def_enum inner_type) {
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(inner_type));
                            string type_n = __type_enum_helper::get_type_string(inner_type);
                            __base_datatype_io *handler = nullptr;
                            unordered_map<string, __base_datatype_io *>::iterator iter = array_handlers.find(type_n);
                            if (iter != array_handlers.end()) {
                                return iter->second;
                            }
                            handler = create_handler(inner_type);
                            CHECK_NOT_NULL(handler);
                            array_handlers.insert({type_n, handler});
                            return handler;
                        }

                        static __base_datatype_io *create_handler(__type_def_enum inner_type) {
                            __base_datatype_io *handler = nullptr;
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(inner_type));
                            switch (inner_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __byte_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__byte_array));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __char_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__char_array));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __short_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__short_array));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __int_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__int_array));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __long_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__long_array));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __float_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__float_array));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __double_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__double_array));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __timestamp_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__timestamp_array));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __string_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__string_array));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __text_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__text_array));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __struct_array();
                                    CHECK_ALLOC(handler, TYPE_NAME(__struct_array));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", inner_type);
                            }
                            return handler;
                        }
                    };

                    class __list_init_utils {
                    private:
                        static unordered_map<string, __base_datatype_io *> list_handlers;

                    public:
                        static __base_datatype_io *get_handler(__type_def_enum inner_type) {
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(inner_type));
                            string type_n = __type_enum_helper::get_type_string(inner_type);
                            __base_datatype_io *handler = nullptr;
                            unordered_map<string, __base_datatype_io *>::iterator iter = list_handlers.find(type_n);
                            if (iter != list_handlers.end()) {
                                return iter->second;
                            }
                            handler = create_handler(inner_type);
                            CHECK_NOT_NULL(handler);
                            list_handlers.insert({type_n, handler});
                            return handler;
                        }

                        static __base_datatype_io *create_handler(__type_def_enum inner_type) {
                            __base_datatype_io *handler = nullptr;
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(inner_type));
                            switch (inner_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __byte_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__byte_list));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __char_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__char_list));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __short_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__short_list));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __int_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__int_list));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __long_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__long_list));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __float_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__float_list));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __double_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__double_list));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __timestamp_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__timestamp_list));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __string_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__string_list));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __text_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__text_list));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __struct_list();
                                    CHECK_ALLOC(handler, TYPE_NAME(__struct_list));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", inner_type);
                            }
                            return handler;
                        }
                    };

                    class __map_init_utils {
                    private:
                        static unordered_map<string, __base_datatype_io *> map_handlers;

                        static string get_key(__type_def_enum key_type, __type_def_enum value_type) {
                            string key_n = __type_enum_helper::get_type_string(key_type);
                            string value_n = __type_enum_helper::get_type_string(value_type);
                            return common_utils::format("%s::%s", key_n.c_str(), value_n.c_str());
                        }

                        static __base_datatype_io *create_char_map(__type_def_enum value_type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<char, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_byte_map(__type_def_enum value_type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<uint8_t, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_short_map(__type_def_enum value_type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<short, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_int_map(__type_def_enum value_type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<int, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_long_map(__type_def_enum value_type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<long, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_float_map(__type_def_enum value_type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<float, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_double_map(__type_def_enum value_type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<double, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_timestamp_map(__type_def_enum value_type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<uint64_t, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                        static __base_datatype_io *create_string_map(__type_def_enum value_type) {
                            __base_datatype_io *handler = nullptr;
                            switch (value_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, uint8_t, __type_def_enum::TYPE_BYTE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, char, __type_def_enum::TYPE_CHAR>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, short, __type_def_enum::TYPE_SHORT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, int, __type_def_enum::TYPE_INTEGER>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, long, __type_def_enum::TYPE_LONG>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, float, __type_def_enum::TYPE_FLOAT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, double, __type_def_enum::TYPE_DOUBLE>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, uint64_t, __type_def_enum::TYPE_TIMESTAMP>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_STRING>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, string, __type_def_enum::TYPE_TEXT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    handler = new __dt_map<string, __type_def_enum::TYPE_CHAR, __struct_datatype__, __type_def_enum::TYPE_STRUCT>();
                                    CHECK_ALLOC(handler, TYPE_NAME(__dt_map));
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", value_type);
                            }
                            return handler;
                        }

                    public:
                        static __base_datatype_io *get_handler(__type_def_enum key_type, __type_def_enum value_type) {
                            PRECONDITION(__type_enum_helper::is_native(key_type));
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(value_type));
                            string type_n = get_key(key_type, value_type);

                            __base_datatype_io *handler = nullptr;
                            unordered_map<string, __base_datatype_io *>::iterator iter = map_handlers.find(type_n);
                            if (iter != map_handlers.end()) {
                                return iter->second;
                            }
                            handler = create_handler(key_type, value_type);
                            CHECK_NOT_NULL(handler);
                            map_handlers.insert({type_n, handler});
                            return handler;
                        }

                        static __base_datatype_io *
                        create_handler(__type_def_enum key_type, __type_def_enum value_type) {
                            __base_datatype_io *handler = nullptr;
                            PRECONDITION(__type_enum_helper::is_native(key_type));
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(value_type));
                            switch (key_type) {
                                case __type_def_enum::TYPE_BYTE:
                                    handler = create_byte_map(value_type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    handler = create_char_map(value_type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    handler = create_short_map(value_type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    handler = create_int_map(value_type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    handler = create_long_map(value_type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    handler = create_float_map(value_type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    handler = create_double_map(value_type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                case __type_def_enum::TYPE_DATETIME:
                                    handler = create_timestamp_map(value_type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    handler = create_string_map(value_type);
                                    CHECK_NOT_NULL(handler);
                                    break;
                                default:
                                    throw BASE_ERROR("Specified type not supported. [type=%d]", key_type);
                            }
                            return handler;
                        }
                    };
                }
REACTFS_NS_CORE_END

#endif //REACTFS_TYPE_DEFS_H
