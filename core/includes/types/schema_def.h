
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
//
// Created by Subhabrata Ghosh on 11/12/16.
//

#ifndef REACTFS_SCHEMA_DEF_H
#define REACTFS_SCHEMA_DEF_H

#include "common/includes/common.h"
#include "core/includes/core.h"

#include "types_common.h"
#include "__constraints.h"

REACTFS_NS_CORE
                namespace types {

                    /*!
                     * Enum definition for the field types.
                     */
                    typedef enum __field_type__ {
                        /// Field type is a native type.
                                NATIVE = 0,
                        /// Field type is a sized type.
                                SIZED = 1,
                        /// Field type is a complex type.
                                COMPLEX = 2,
                        /// Field type is an array.
                                ARRAY = 3,
                        /// Field type is a list
                                LIST = 4,
                        /// Field type is a map
                                MAP = 5
                    } __field_type;

                    /*!
                     * Helper class for field type definitions.
                     */
                    class __field_type_helper {
                    public:
                        /*!
                         * Get the numeric (uint8_t) value of the field type.
                         *
                         * @param type - Field type.
                         * @return - Numeric value.
                         */
                        static uint8_t get_type_number(__field_type type) {
                            return (uint8_t) type;
                        }

                        /*!
                         * Get the type enum for the specified numeric value.
                         *
                         * @param ii - Type enum numeric value.
                         * @return - Type enum.
                         */
                        static __field_type get_type(uint8_t ii) {
                            if (ii == 0) {
                                return __field_type::NATIVE;
                            } else if (ii == 1) {
                                return __field_type::SIZED;
                            } else if (ii == 2) {
                                return __field_type::COMPLEX;
                            } else if (ii == 3) {
                                return __field_type::ARRAY;
                            } else if (ii == 4) {
                                return __field_type::LIST;
                            } else if (ii == 5) {
                                return __field_type::MAP;
                            }
                            throw BASE_ERROR("Not field type defined for value. [value=%d]", ii);
                        }
                    };

                    /*!
                     * Field definition for a native data type. (including string).
                     */
                    class __native_type {
                    protected:
                        /// Field name
                        string name;
                        /// Field index (in the complex type definition)
                        uint8_t index;
                        /// Type of this field definition
                        __field_type type;
                        /// Datatype of this field.
                        __type_def_enum datatype;
                        /// Constraints (if any) defined for this field.
                        __constraint *constraint = nullptr;
                        /// Default value (if any) defined for this field.
                        __default *default_value = nullptr;
                        /// Field datatype IO handler.
                        __base_datatype_io *type_handler = nullptr;
                        /// Is this field nullable.
                        bool nullable = false;
                        /// Parent type of which this field is a part.
                        const __native_type *parent = nullptr;
                    public:
                        /*!
                         * Default empty constructor, to be instantiated
                         * when reading the definition from buffer.
                         */
                        __native_type(const __native_type *parent) {
                            this->parent = parent;
                        }

                        /*!
                         * Constructor for creating a new field definition.
                         *
                         * @param index - Index of this field in the complex type.
                         * @param name - Name of this field.
                         * @param datatype - Data type of this field.
                         */
                        __native_type(const __native_type *parent, const uint8_t index, const string &name,
                                      const __type_def_enum datatype) {
                            this->parent = parent;
                            this->index = index;
                            this->name = string(name);
                            this->datatype = datatype;
                            this->type = __field_type::NATIVE;

                            type_handler = __type_defs_utils::get_type_handler(this->datatype);
                            CHECK_NOT_NULL(type_handler);
                        }

                        /*!
                         * Default virtual destuctor.
                         */
                        virtual ~__native_type() {
                            CHECK_AND_FREE(constraint);
                            CHECK_AND_FREE(default_value);
                        }

                        /*!
                         * Get the index of this field.
                         *
                         * @return - Field index.
                         */
                        const uint8_t get_index() const {
                            return this->index;
                        }

                        /*!
                         * Get the name of this field.
                         *
                         * @return - Field name.
                         */
                        const string get_name() const {
                            return this->name;
                        }

                        /*!
                         * Get the datatype of this field.
                         *
                         * @return - Field datatype.
                         */
                        const __type_def_enum get_datatype() const {
                            return this->datatype;
                        }

                        /*!
                         * Set the constraint definition for this field.
                         *
                         * @param constraint - Field data constraint.
                         */
                        virtual void set_constraint(__constraint *constraint) {
                            this->constraint = constraint;
                        }

                        /*!
                         * Get the constraint definition for this field.
                         *
                         * @return - Field data constraint.
                         */
                        __constraint *get_constraint() const {
                            return this->constraint;
                        }

                        /*!
                         * Set the default value for this field.
                         *
                         * @param default_value - Field default value.
                         */
                        virtual void set_default_value(__default *default_value) {
                            this->default_value = default_value;
                        }

                        /*!
                         * Get the defualt value for this field.
                         *
                         * @return - Field default value.
                         */
                        const __default *get_default_value() const {
                            return this->default_value;
                        }

                        /*!
                         * Set if this field is nullable.
                         *
                         * @param nullable - Is nullable?
                         */
                        void set_nullable(bool nullable) {
                            this->nullable = nullable;
                        }

                        /*!
                         * Get if this field is nullable.
                         *
                         * @return - Is nullable?
                         */
                        bool is_nullable() const {
                            return this->nullable;
                        }

                        /*!
                         * Check if the specified field value is a valid value.
                         *
                         * @param value - Field data value.
                         * @return - Is valid?
                         */
                        bool is_valid_value(void *value) {
                            if (!nullable && IS_NULL(value)) {
                                return false;
                            }
                            if (NOT_NULL(constraint)) {
                                return constraint->validate(value);
                            }
                            return true;
                        }

                        /*!
                         * Write (serialize) this field definition to the output buffer.
                         *
                         * @param buffer - Output write buffer.
                         * @param offset - Offset to start writing from.
                         * @return - Number of bytes written.
                         */
                        virtual uint32_t write(void *buffer, uint64_t offset) {
                            CHECK_NOT_NULL(buffer);
                            __base_datatype_io *string_handler = __type_defs_utils::get_type_handler(
                                    __type_def_enum::TYPE_STRING);
                            CHECK_NOT_NULL(string_handler);

                            // Write the field type.
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t ft = __field_type_helper::get_type_number(this->type);
                            memcpy(ptr, &ft, sizeof(uint8_t));
                            uint32_t w_size = sizeof(uint8_t);

                            // Write the field index
                            ptr = common_utils::increment_data_ptr(ptr, w_size);
                            memcpy(ptr, &index, sizeof(uint8_t));
                            w_size += sizeof(uint8_t);

                            // Write the field name.
                            w_size += string_handler->write(buffer, &name, (offset + w_size), ULONG_MAX);
                            uint8_t bits = 0;
                            if (NOT_NULL(constraint)) {
                                bits = bitset_utils::set_uint8_bit(bits, BIT_TYPE_CONSTRAINT);
                            }
                            // Write the field datatype
                            uint8_t dt = (uint8_t) this->datatype;
                            ptr = common_utils::increment_data_ptr(buffer, (offset + w_size));
                            memcpy(ptr, &dt, sizeof(uint8_t));
                            w_size += sizeof(uint8_t);

                            // Write is nullable
                            ptr = common_utils::increment_data_ptr(buffer, (offset + w_size));
                            memcpy(ptr, &this->nullable, sizeof(bool));
                            w_size += sizeof(bool);

                            // Set constraint bit
                            if (NOT_NULL(constraint)) {
                                bits = bitset_utils::set_uint8_bit(bits, BIT_TYPE_CONSTRAINT);
                            }

                            // Set default value bit
                            if (NOT_NULL(default_value)) {
                                bits = bitset_utils::set_uint8_bit(bits, BIT_TYPE_DEFAULT_VALUE);
                            }
                            // Write the extended data bits.
                            ptr = common_utils::increment_data_ptr(ptr, w_size);
                            memcpy(ptr, &bits, sizeof(uint8_t));
                            w_size += sizeof(uint8_t);

                            // Write the constraint (if any)
                            if (NOT_NULL(constraint)) {
                                w_size += this->constraint->write(buffer, (offset + w_size));
                            }
                            // Write the default value (if any)
                            if (NOT_NULL(default_value)) {
                                w_size += this->default_value->write(buffer, (offset + w_size));
                            }
                            return w_size;
                        }

                        /*!
                         * Read (de-serialize) the field definition from the input buffer.
                         *
                         * @param buffer - Input data buffer.
                         * @param offset - Offset to start reading from.
                         * @return - Number of bytes read.
                         */
                        virtual uint32_t read(void *buffer, uint64_t offset) {
                            CHECK_NOT_NULL(buffer);
                            __base_datatype_io *string_handler = __type_defs_utils::get_type_handler(
                                    __type_def_enum::TYPE_STRING);
                            CHECK_NOT_NULL(string_handler);

                            // Read the field type.
                            void *ptr = common_utils::increment_data_ptr(buffer, offset);
                            uint8_t *ft = static_cast<uint8_t *>(ptr);
                            POSTCONDITION(*ft == __field_type_helper::get_type_number(this->type));
                            uint32_t r_size = sizeof(uint8_t);

                            // Read the field index.
                            ptr = common_utils::increment_data_ptr(ptr, r_size);
                            uint16_t *index = static_cast<uint16_t *>(ptr);
                            this->index = *index;
                            r_size += sizeof(uint16_t);

                            // Read the field name.
                            string *sp = nullptr;
                            r_size += string_handler->read(buffer, &sp, (offset + r_size), ULONG_MAX);
                            CHECK_NOT_NULL(sp);
                            this->name = string(*sp);
                            CHECK_AND_FREE(sp);

                            // Read the field datatype
                            ptr = common_utils::increment_data_ptr(buffer, (offset + r_size));
                            uint8_t *dt = static_cast<uint8_t *>(ptr);
                            this->datatype = __type_enum_helper::parse_type(*dt);
                            r_size += sizeof(uint16_t);
                            type_handler = __type_defs_utils::get_type_handler(this->datatype);
                            CHECK_NOT_NULL(type_handler);

                            // Read is nullable
                            ptr = common_utils::increment_data_ptr(buffer, (offset + r_size));
                            bool *nn = static_cast<bool *>(ptr);
                            this->nullable = *nn;
                            r_size += sizeof(bool);

                            // Read the extended data bitmap
                            ptr = common_utils::increment_data_ptr(buffer, (offset + r_size));
                            uint8_t *bits = static_cast<uint8_t *>(ptr);
                            r_size += sizeof(uint8_t);

                            // If constriant bit is set, read the constraint definition.
                            if (bitset_utils::check_uint8_bit(*bits, BIT_TYPE_CONSTRAINT)) {
                                uint64_t size = 0;
                                this->constraint = __constraint_loader::read(buffer, (offset + r_size), this->datatype,
                                                                             &size);
                                CHECK_NOT_NULL(this->constraint);
                                POSTCONDITION(size > 0);
                                r_size += size;
                            }
                            // If the default value bit is set, read the default value definition
                            if (bitset_utils::check_uint8_bit(*bits, BIT_TYPE_DEFAULT_VALUE)) {
                                uint64_t size = 0;
                                this->default_value = __defaults_loader::read(buffer, (offset + r_size), this->datatype,
                                                                              &size);
                                CHECK_NOT_NULL(this->default_value);
                                POSTCONDITION(size > 0);
                                r_size += size;
                            }
                            return r_size;
                        }

                        const __native_type *get_parent() const {
                            return this->parent;
                        }

                        string get_canonical_name() const {
                            if (NOT_NULL(parent)) {
                                string sn = parent->get_canonical_name();
                                return common_utils::format("%s.%s", sn, this->name);
                            }
                            return this->name;
                        }
                    };

                    /*!
                     * Instance of a field that has a size definition along with the basic
                     * definitions used by the native fields.
                     */
                    class __sized_type : public __native_type {
                    private:
                        /// Max size defined for this field.
                        uint32_t max_size = 0;

                    public:
                        /*!
                         * Default empty constructor, to be instantiated
                         * when reading the definition from buffer.
                         */
                        __sized_type(const __native_type *parent) : __native_type(parent) {

                        }

                        /*!
                         * Constructor for creating a new field definition.
                         *
                         * @param index - Index of this field in the complex type.
                         * @param name - Name of this field.
                         * @param datatype - Data type of this field.
                         * @param max_size - Maximum size for this field type.
                         */
                        __sized_type(const __native_type *parent, const uint8_t index, const string &name,
                                     const __type_def_enum datatype,
                                     const uint32_t max_size)
                                : __native_type(parent, index, name, datatype) {
                            PRECONDITION(max_size > 0);
                            this->max_size = max_size;
                            this->type = __field_type::SIZED;
                        }

                        const uint32_t get_max_size() const {
                            return this->max_size;
                        }

                        /*!
                         * Write (serialize) this field definition to the output buffer.
                         *
                         * @param buffer - Output write buffer.
                         * @param offset - Offset to start writing from.
                         * @return - Number of bytes written.
                         */
                        virtual uint32_t write(void *buffer, uint64_t offset) override {
                            uint32_t w_size = __native_type::write(buffer, offset);
                            // Write the max_size value.
                            void *ptr = common_utils::increment_data_ptr(buffer, (offset + w_size));
                            memcpy(ptr, &max_size, sizeof(uint32_t));
                            w_size += sizeof(uint32_t);

                            return w_size;
                        }

                        /*!
                         * Read (de-serialize) the field definition from the input buffer.
                         *
                         * @param buffer - Input data buffer.
                         * @param offset - Offset to start reading from.
                         * @return - Number of bytes read.
                         */
                        virtual uint32_t read(void *buffer, uint64_t offset) override {
                            uint32_t r_size = __native_type::read(buffer, offset);
                            // Read the max_size value.
                            void *ptr = common_utils::increment_data_ptr(buffer, (offset + r_size));
                            uint32_t *s = static_cast<uint32_t *>(ptr);
                            this->max_size = *s;
                            r_size += sizeof(uint32_t);

                            return r_size;
                        }
                    };

                    class __array_type : public __sized_type {
                    private:
                        __type_def_enum inner_type;
                        __base_datatype_io *inner_type_handler = nullptr;
                    public:
                        __array_type(const __native_type *parent) : __sized_type(parent) {

                        }

                        __array_type(const __native_type *parent, const uint8_t index, const string &name,
                                     const __type_def_enum inner_type,
                                     const uint32_t max_size) : __sized_type(parent, index, name,
                                                                             __type_def_enum::TYPE_ARRAY, max_size) {
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(inner_type));
                            this->inner_type = inner_type;
                            inner_type_handler = __type_defs_utils::get_type_handler(this->inner_type);
                            CHECK_NOT_NULL(inner_type_handler);
                        }

                        /*!
                         * Set the constraint definition for this field.
                         * Note:: Constraints cannot be defined for arrays. Call will throw exception.
                         *
                         * @param constraint - Field data constraint.
                         */
                        virtual void set_constraint(__constraint *constraint) {
                            throw BASE_ERROR("Constraints can only be defined for basic types.");
                        }

                        /*!
                         * Set the default value for this field.
                         * Note:: Default value cannot be defined for arrays. Call will throw exception.
                         *
                         * @param default_value - Field default value.
                         */
                        virtual void set_default_value(__default *default_value) {
                            throw BASE_ERROR("Default value can only be defined for basic types.");
                        }

                        /*!
                         * Write (serialize) this field definition to the output buffer.
                         *
                         * @param buffer - Output write buffer.
                         * @param offset - Offset to start writing from.
                         * @return - Number of bytes written.
                         */
                        virtual uint32_t write(void *buffer, uint64_t offset) override {

                        }
                    };

                    class __complex_type : public __native_type {
                    private:
                        unordered_map<string, __native_type *> fields;
                    public:
                        __complex_type(const uint8_t index, const string &name) : __native_type(index, name,
                                                                                                __type_def_enum::TYPE_STRUCT) {
                            this->type = __field_type::COMPLEX;
                        }

                        ~__complex_type() {
                            unordered_map<string, __native_type *>::iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                CHECK_AND_FREE(iter->second);
                            }
                            fields.clear();
                        }

                        void add_field(string &name, __native_type *type) {
                            CHECK_NOT_EMPTY(name);
                            CHECK_NOT_NULL(type);
                            fields.insert({name, type});
                        }

                        __native_type *get_type(string &name) {
                            unordered_map<string, __native_type *>::iterator iter = fields.find(name);
                            if (iter != fields.end()) {
                                return iter->second;
                            }
                            return nullptr;
                        }
                    };

                }
REACTFS_NS_CORE_END
#endif //REACTFS_SCHEMA_DEF_H
