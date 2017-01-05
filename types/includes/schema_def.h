
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

#include <unordered_map>

#include "common/includes/common.h"
#include "common/includes/log_utils.h"
#include "common/includes/buffer_utils.h"
#include "core/includes/core.h"

#include "types_common.h"
#include "schema_common.h"
#include "__constraints.h"
#include "schema_helpers.h"

#define MAP_TYPE_KEY_NAME "key"
#define MAP_TYPE_VALUE_NAME "value"

REACTFS_NS_CORE
                namespace types {

                    /*!
                     * Enum definition for the field types.
                     */
                    typedef enum __field_type__ {
                        /// Field type is a native type.
                                NATIVE = 1,
                        /// Field type is a sized type.
                                SIZED = 2,
                        /// Field type is a complex type.
                                COMPLEX = 3,
                        /// Field type is an array.
                                ARRAY = 4,
                        /// Field type is a list
                                LIST = 5,
                        /// Field type is a map
                                MAP = 6
                    } __field_type;

                    typedef enum __record_mode__ {
                        RM_READ = 0, RM_WRITE = 1
                    } __record_mode;

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
                            if (ii == 1) {
                                return __field_type::NATIVE;
                            } else if (ii == 2) {
                                return __field_type::SIZED;
                            } else if (ii == 3) {
                                return __field_type::COMPLEX;
                            } else if (ii == 4) {
                                return __field_type::ARRAY;
                            } else if (ii == 5) {
                                return __field_type::LIST;
                            } else if (ii == 6) {
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
                        __native_type *parent = nullptr;
                    public:
                        /*!
                         * Default empty constructor, to be instantiated
                         * when reading the definition from buffer.
                         *
                         * @param parent - Parent type node.
                         */
                        __native_type(__native_type *parent) {
                            this->parent = parent;
                            this->type = __field_type::NATIVE;
                        }

                        /*!
                         * Constructor for creating a new field definition.
                         *
                         * @param parent - Parent type node.
                         * @param index - Index of this field in the complex type.
                         * @param name - Name of this field.
                         * @param datatype - Data type of this field.
                         * @param mode - Type of this node (Read/Write)
                         */
                        __native_type(__native_type *parent, const uint8_t index, const string &name,
                                      const __type_def_enum datatype) {
                            this->parent = parent;
                            this->index = index;
                            this->name = string(name);
                            this->datatype = datatype;
                            this->type = __field_type::NATIVE;

                            if (__type_enum_helper::is_native(this->datatype) ||
                                this->datatype == __type_def_enum::TYPE_TEXT) {
                                type_handler = __type_defs_utils::get_type_handler(this->datatype);
                                CHECK_NOT_NULL(type_handler);
                            }
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
                         * Get the field type of this instance.
                         *
                         * @return - Field type enum.
                         */
                        __field_type get_type() const {
                            return this->type;
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
                         * Get the type IO handler.
                         *
                         * @param mode - Handler for read or write?
                         * @return - Type IO handler.
                         */
                        virtual __base_datatype_io *get_type_handler(__record_mode mode) {
                            return this->type_handler;
                        }

                        /*!
                         * Check if the specified field value is a valid value.
                         *
                         * @param value - Field data value.
                         * @return - Is valid?
                         */
                        bool is_valid_value(const void *value) {
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
                            uint64_t w_offset = offset;

                            // Write the field type.
                            uint8_t ft = __field_type_helper::get_type_number(this->type);
                            TRACE("[write][type=%d][name=%s] [field type] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), w_offset);
                            uint32_t w_size = buffer_utils::write<uint8_t>(buffer, &w_offset, ft);

                            // Write the field datatype
                            uint8_t dt = (uint8_t) this->datatype;
                            TRACE("[write][type=%d][name=%s] [datatype] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), w_offset);
                            w_size += buffer_utils::write<uint8_t>(buffer, &w_offset, dt);

                            // Write the field index
                            TRACE("[write][type=%d][name=%s] [index] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), w_offset);
                            w_size += buffer_utils::write<uint8_t>(buffer, &w_offset, index);

                            // Write the field name.
                            TRACE("[write][type=%d][name=%s] [name] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), w_offset);
                            w_size += buffer_utils::write_8str(buffer, &w_offset, this->name.c_str(),
                                                               this->name.length());

                            // Write is nullable
                            TRACE("[write][type=%d][name=%s] [nullable] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), w_offset);
                            w_size += buffer_utils::write<bool>(buffer, &w_offset, this->nullable);

                            uint8_t bits = 0;
                            // Set constraint bit
                            if (NOT_NULL(constraint)) {
                                bits = bitset_utils::set_uint8_bit(bits, BIT_TYPE_CONSTRAINT);
                            }

                            // Set default value bit
                            if (NOT_NULL(default_value)) {
                                bits = bitset_utils::set_uint8_bit(bits, BIT_TYPE_DEFAULT_VALUE);
                            }
                            // Write the extended data bits.
                            TRACE("[write][type=%d][name=%s] [bits] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), w_offset);
                            w_size += buffer_utils::write<uint8_t>(buffer, &w_offset, bits);

                            // Write the constraint (if any)
                            if (NOT_NULL(constraint)) {
                                TRACE("[write][type=%d][name=%s] [constraint] Offset = %d", this->type,
                                      this->get_canonical_name().c_str(), w_offset);
                                w_size += this->constraint->write(buffer, (offset + w_size));
                            }
                            // Write the default value (if any)
                            if (NOT_NULL(default_value)) {
                                TRACE("[write][type=%d][name=%s] [default value] Offset = %d", this->type,
                                      this->get_canonical_name().c_str(), w_offset);
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
                            uint64_t r_offset = offset;

                            // Read the field type.
                            uint8_t *ft = nullptr;
                            TRACE("[read][type=%d][name=%s] [field type] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), r_offset);
                            uint32_t r_size = buffer_utils::read<uint8_t>(buffer, &r_offset, &ft);
                            CHECK_NOT_NULL(ft);
                            POSTCONDITION(*ft == __field_type_helper::get_type_number(this->type));


                            // Read the field datatype
                            uint8_t *dt = nullptr;
                            TRACE("[read][type=%d][name=%s] [datatype] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), r_offset);
                            r_size += buffer_utils::read<uint8_t>(buffer, &r_offset, &dt);
                            CHECK_NOT_NULL(dt);
                            this->datatype = __type_enum_helper::parse_type(*dt);
                            POSTCONDITION(this->datatype != __type_def_enum::TYPE_UNKNOWN);
                            if (__type_enum_helper::is_native(this->datatype) ||
                                this->datatype == __type_def_enum::TYPE_TEXT) {
                                type_handler = __type_defs_utils::get_type_handler(this->datatype);
                                CHECK_NOT_NULL(type_handler);
                            }

                            // Read the field index.
                            uint8_t *index = nullptr;
                            TRACE("[read][type=%d][name=%s] [index] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), r_offset);
                            r_size += buffer_utils::read<uint8_t>(buffer, &r_offset, &index);
                            CHECK_NOT_NULL(index);
                            this->index = *index;

                            // Read the field name.
                            string *sp = new string();
                            CHECK_ALLOC(sp, TYPE_NAME(string));
                            TRACE("[read][type=%d][name=%s] [name] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), r_offset);
                            r_size += buffer_utils::read_8str(buffer, &r_offset, sp);
                            CHECK_NOT_NULL(sp);
                            this->name = string(*sp);
                            CHECK_AND_FREE(sp);

                            // Read is nullable
                            bool *nn = nullptr;
                            TRACE("[read][type=%d][name=%s] [nullable] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), r_offset);
                            r_size += buffer_utils::read<bool>(buffer, &r_offset, &nn);
                            this->nullable = *nn;

                            // Read the extended data bitmap
                            uint8_t *bits = nullptr;
                            TRACE("[read][type=%d][name=%s] [bits] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), r_offset);
                            r_size += buffer_utils::read<uint8_t>(buffer, &r_offset, &bits);

                            // If constriant bit is set, read the constraint definition.
                            if (bitset_utils::check_uint8_bit(*bits, BIT_TYPE_CONSTRAINT)) {
                                TRACE("[read][type=%d][name=%s] [constraint] Offset = %d", this->type,
                                      this->get_canonical_name().c_str(), (offset + r_size));
                                uint64_t size = 0;
                                this->constraint = __constraint_loader::read(buffer, (offset + r_size), this->datatype,
                                                                             &size);
                                CHECK_NOT_NULL(this->constraint);
                                POSTCONDITION(size > 0);
                                r_size += size;
                            }
                            // If the default value bit is set, read the default value definition
                            if (bitset_utils::check_uint8_bit(*bits, BIT_TYPE_DEFAULT_VALUE)) {
                                TRACE("[read][type=%d][name=%s] [default value] Offset = %d", this->type,
                                      this->get_canonical_name().c_str(), (offset + r_size));
                                uint64_t size = 0;
                                this->default_value = __defaults_loader::read(buffer, (offset + r_size), this->datatype,
                                                                              &size);
                                CHECK_NOT_NULL(this->default_value);
                                POSTCONDITION(size > 0);
                                r_size += size;
                            }
                            return r_size;
                        }

                        /*!
                         * Get the parent type of this type instance.
                         *
                         * @return - Parent type instance.
                         */
                        const __native_type *get_parent() const {
                            return this->parent;
                        }

                        /*!
                         * Set the parent instance of this type.
                         *
                         * @param parent - Parent type instance.
                         */
                        void set_parent(__native_type *parent) {
                            CHECK_NOT_NULL(parent);
                            this->parent = parent;
                        }

                        /*!
                         * Get the complete (canonical) name of this type node.
                         *
                         * Canonical name is the complete name path (using dot notation).
                         *
                         * @return - Canonical name.
                         */
                        virtual string get_canonical_name() const {
                            if (NOT_NULL(parent)) {
                                string sn = parent->get_canonical_name();
                                return common_utils::format("%s.%s", sn.c_str(), this->name.c_str());
                            }
                            return this->name;
                        }

                        virtual string get_type_name() const {
                            return __type_enum_helper::get_datatype(this->datatype);
                        }

                        virtual string get_type_ptr() const {
                            string ptr = get_type_name();
                            ptr = common_utils::format("%s *", ptr.c_str());
                            return ptr;
                        }

                        /*!
                         * Utility function to print a readable format of the type structure.
                         */
                        virtual void print() const {
                            string t = __type_enum_helper::get_type_string(this->datatype);
                            string n = this->get_canonical_name();
                            LOG_DEBUG("[index=%d] [name=%s] %s", this->index, n.c_str(), t.c_str());
                            if (NOT_NULL(constraint)) {
                                constraint->print();
                            }
                            if (NOT_NULL(default_value)) {
                                default_value->print();
                            }
                        }

                        virtual void get_field_index(unordered_map<string, __native_type *> *map) {
                            CHECK_NOT_NULL(map);
                            string key = get_canonical_name();
                            map->insert({key, this});
                        }

                        virtual uint32_t estimate_size() {
                            CHECK_NOT_NULL(this->type_handler);
                            return this->type_handler->estimate_size();
                        }
                    };

                    typedef struct __field_value__ {
                        const __native_type *type = nullptr;
                        void *data = nullptr;
                    } __field_value;


                    /*!
                     * Interface to define the datatype loaders for a complex type.
                     */
                    class __complex_type_helper {
                    protected:
                        /// Static singleton instance of the type helper.
                        static __complex_type_helper *type_loader;

                    public:
                        virtual ~__complex_type_helper() {

                        }

                        /*!
                         * Load the field type definition for fields of a complex type.
                         *
                         * @param buffer - Input data buffer.
                         * @param offset - Offset to start reading from.
                         * @param count - Number of fields expected to be loaded.
                         * @param fields - Vector to return the loaded fields.
                         * @param size - Size increment pointer.
                         * @return - Loaded field definition.
                         */
                        virtual void read(__native_type *parent, void *buffer, uint64_t offset, uint8_t count,
                                          vector<__native_type *> *fields,
                                          uint32_t *size) = 0;

                        /*!
                         * Get the IO type handler for a complex datatype.
                         *
                         * @param type - Type definition.
                         * @return - IO Handler.
                         */
                        virtual __base_datatype_io *get_complex_type_handler(__native_type *type) = 0;

                        /*!
                         * Get the IO type handler for a list/array datatype.
                         *
                         * @param inner_type_enum - Type inner of the list element.
                         * @param inner_type - Type definition of the list element.
                         * @param mode - Hanlder in read or write mode?
                         * @return - List IO type handler.
                         */
                        virtual __base_datatype_io *
                        get_array_type_handler(__type_def_enum inner_type_enum,
                                               __native_type *inner_type, __record_mode mode) = 0;

                        /*!
                         * Get the IO type handler for a map datatype.
                         *
                         * @param key_type_enum - Type of the map key.
                         * @param value_type_enum - Type of the map value.
                         * @param value_type - Value type.
                         * @param mode - Hanlder in read or write mode?
                         * @return - Map IO type handler.
                         */
                        virtual __base_datatype_io *
                        get_map_type_handler(__type_def_enum key_type_enum, __type_def_enum value_type_enum,
                                             __native_type *value_type, __record_mode mode) = 0;

                        /*!
                         * Free an instance of a record struct.
                         *
                         * @param type - Struct type definition. (__complex_type)
                         * @param node - Struct value pointer.
                         * @param mode - Struct create mode (Read/Write)
                         */
                        virtual void free_type_node(const __native_type *type, void *node, __record_mode mode) = 0;

                        /*!
                         * Get the singleton instance of the type helper.
                         *
                         * @return - Type helper instance.
                         */
                        static __complex_type_helper *get_type_loader() {
                            CHECK_NOT_NULL(type_loader);
                            return type_loader;
                        }

                        /*!
                         * Dispose this helper singleton instance.
                         */
                        static void dispose() {
                            CHECK_AND_FREE(type_loader);
                        }
                    };


                    /*!
                     * A complex (composite) type instance.
                     * Complex types can contain any other type instances.
                     */
                    class __complex_type : public __native_type {
                    private:
                        /// Map of column index/types.
                        unordered_map<uint8_t, __native_type *> fields;
                        /// Index map of fields by name.
                        unordered_map<string, uint8_t> field_index;
                        /// Index of canonical field names.
                        unordered_map<string, __native_type *> name_index;
                        /// Type loader helper instance.
                        __complex_type_helper *loader = nullptr;
                        /// Type name of this complex type.
                        string type_name;

                        /*!
                         * Add a new field definition.
                         *
                         * @param index - Column index of this field.
                         * @param name - Field name.
                         * @param type - Datatype of the field.
                         */
                        void add_field(const uint8_t index, const string &name, __native_type *type) {
                            CHECK_NOT_EMPTY(name);
                            CHECK_NOT_NULL(type);
                            PRECONDITION(fields.size() < UCHAR_MAX);

                            fields.insert({index, type});
                            field_index.insert({name, index});
                        }

                    public:
                        /*!
                         * Constructor to be used when this instance will be setup by reading from the
                         * serialized form.
                         *
                         * @param parent - Parent type of this instance.
                         */
                        __complex_type(__native_type *parent) : __native_type(parent) {
                            this->type = __field_type::COMPLEX;
                            this->datatype = __type_def_enum::TYPE_STRUCT;
                            loader = __complex_type_helper::get_type_loader();
                            CHECK_NOT_NULL(loader);
                            this->type_handler = loader->get_complex_type_handler(this);
                            CHECK_NOT_NULL(this->type_handler);
                        }

                        /*!
                         * Constructor to be used when this is the root node of the schema definition.
                         *
                         * @param parent - Parent type of this instance.
                         * @param name - Schema name
                         * @param type_name - Name of the referenced type.
                         */
                        __complex_type(__native_type *parent, const string &name, const string &type_name)
                                : __native_type(parent) {
                            CHECK_NOT_EMPTY(name);
                            CHECK_NOT_EMPTY(type_name);
                            this->type = __field_type::COMPLEX;
                            this->datatype = __type_def_enum::TYPE_STRUCT;
                            this->name = string(name);
                            this->index = 0;
                            this->type_name = string(type_name);

                            loader = __complex_type_helper::get_type_loader();
                            CHECK_NOT_NULL(loader);
                            this->type_handler = loader->get_complex_type_handler(this);
                            CHECK_NOT_NULL(this->type_handler);
                        }

                        /*!
                         * Constructor to be used when creating an embedded type instance.
                         *
                         * @param parent - Parent type of this instance.
                         * @param index - Column index in the parent.
                         * @param name - Field name
                         */
                        __complex_type(__native_type *parent, const uint8_t index, const string &name,
                                       const string &type_name) : __native_type(
                                parent, index, name,
                                __type_def_enum::TYPE_STRUCT) {
                            CHECK_NOT_EMPTY(name);
                            CHECK_NOT_EMPTY(type_name);
                            this->type = __field_type::COMPLEX;
                            this->type_name = string(type_name);

                            loader = __complex_type_helper::get_type_loader();
                            CHECK_NOT_NULL(loader);
                            this->type_handler = loader->get_complex_type_handler(this);
                            CHECK_NOT_NULL(this->type_handler);
                        }

                        /*!
                         * Dispose this instance of the complex type.
                         */
                        ~__complex_type() {
                            unordered_map<uint8_t, __native_type *>::iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                CHECK_AND_FREE(iter->second);
                            }
                            fields.clear();
                        }

                        /*!
                         * Get a field by field name.
                         *
                         * @param name - Field name.
                         * @return - Field instance.
                         */
                        const __native_type *get_field(const string &name) const {
                            unordered_map<string, uint8_t>::const_iterator iter = field_index.find(name);
                            if (iter != field_index.end()) {
                                const uint8_t index = iter->second;
                                unordered_map<uint8_t, __native_type *>::const_iterator fiter = fields.find(index);
                                if (fiter != fields.end()) {
                                    return fiter->second;
                                }
                            }
                            return nullptr;
                        }

                        /*!
                         * Get a field by column index.
                         *
                         * @param index - Column index of the field.
                         * @return - Field instance.
                         */
                        const __native_type *get_field(const uint8_t index) const {
                            unordered_map<uint8_t, __native_type *>::const_iterator fiter = fields.find(index);
                            if (fiter != fields.end()) {
                                return fiter->second;
                            }
                            return nullptr;
                        }

                        /*!
                         * Get the map of all the fields in this type.
                         *
                         * @return - Map of fields.
                         */
                        const unordered_map<uint8_t, __native_type *> get_fields() const {
                            return this->fields;
                        };

                        /*!
                         * Add a field with the specified column index.
                         *
                         * @param type - Field instance.
                         * @param index - Column index.
                         */
                        void add_field(__native_type *type, uint8_t index) {
                            this->add_field(index, type->get_name(), type);
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
                            uint64_t w_offset = (offset + w_size);

                            // Write the number of fields in this definition.
                            uint8_t size = (uint8_t) fields.size();
                            LOG_DEBUG("[type=%d][name=%s] Size = %d", this->type, this->name.c_str(), size);
                            TRACE("[write][type=%d][name=%s] [field count] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), w_offset);
                            w_size += buffer_utils::write<uint8_t>(buffer, &w_offset, size);

                            unordered_map<uint8_t, __native_type *>::iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                LOG_DEBUG("Writing field [type=%d][name=%s][index=%d]", iter->second->get_datatype(),
                                          iter->second->get_name().c_str(), iter->second->get_index());
                                w_size += iter->second->write(buffer, (offset + w_size));
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
                        virtual uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(loader);
                            uint32_t r_size = __native_type::read(buffer, offset);
                            uint64_t r_offset = (offset + r_size);

                            // Read the number of fields in this definition.
                            uint8_t *size = nullptr;
                            TRACE("[read][type=%d][name=%s] [field count] Offset = %d", this->type,
                                  this->get_canonical_name().c_str(), r_offset);
                            r_size += buffer_utils::read<uint8_t>(buffer, &r_offset, &size);
                            CHECK_NOT_NULL(size);
                            LOG_DEBUG("[type=%d][name=%s] Field count = %d", this->type, this->name.c_str(), *size);

                            vector<__native_type *> types;
                            loader->read(this, buffer, (offset + r_size), *size, &types, &r_size);

                            for (__native_type *type : types) {
                                add_field(type->get_index(), type->get_name(), type);
                            }
                            return r_size;
                        }

                        /*!
                         * Utility function to print a readable format of the type structure.
                         */
                        virtual void print() const override {
                            string t = __type_enum_helper::get_type_string(this->datatype);
                            string n = this->get_canonical_name();
                            LOG_DEBUG("[index=%d] [name=%s] %s", this->index, n.c_str(), t.c_str());
                            unordered_map<uint8_t, __native_type *>::const_iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                __native_type *t = iter->second;
                                CHECK_NOT_NULL(t);
                                t->print();
                            }
                        }

                        virtual string get_type_name() const override {
                            return this->type_name;
                        }

                        virtual void get_field_index(unordered_map<string, __native_type *> *map) override {
                            unordered_map<uint8_t, __native_type *>::iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                iter->second->get_field_index(map);
                            }
                        }

                        virtual uint32_t estimate_size() override {
                            uint32_t size = 0;
                            unordered_map<uint8_t, __native_type *>::iterator iter;
                            for (iter = fields.begin(); iter != fields.end(); iter++) {
                                __native_type *nt = iter->second;
                                size += nt->estimate_size();
                            }
                            return size;
                        }

                        const uint8_t get_field_count() const {
                            return fields.size();
                        }
                    };

                    class record_struct {
                    private:
                        const __complex_type *type = nullptr;
                        __field_value *buffer = nullptr;
                        uint8_t field_count = 0;
                        __field_value **data_ptr = nullptr;


                    public:
                        record_struct(const __complex_type *type, uint8_t field_count) {
                            CHECK_NOT_NULL(type);
                            this->field_count = field_count;
                            data_ptr = new __field_value *[this->field_count];
                            CHECK_ALLOC(data_ptr, TYPE_NAME(__field_value * ));

                            uint32_t size = sizeof(__field_value) * field_count;
                            buffer = (__field_value *) malloc(size);
                            CHECK_ALLOC(buffer, TYPE_NAME(__field_value__));

                            memset(buffer, 0, size);
                            for (uint8_t ii = 0; ii < field_count; ii++) {
                                data_ptr[ii] = nullptr;
                            }
                            this->type = type;
                        }

                        ~record_struct() {
                            __complex_type_helper *helper = __complex_type_helper::get_type_loader();
                            CHECK_NOT_NULL(helper);
                            for (uint8_t ii = 0; ii < field_count; ii++) {
                                if (IS_NULL(data_ptr[ii]))
                                    continue;
                                __type_def_enum t = data_ptr[ii]->type->get_datatype();
                                if (__type_enum_helper::is_native(t)) {
                                    continue;
                                }
                                helper->free_type_node(type, data_ptr[ii], __record_mode::RM_READ);
                            }
                            FREE_PTR(buffer);
                            FREE_PTR(data_ptr);
                        }

                        void add_field(const uint8_t index, void *data) {
                            PRECONDITION(index < this->field_count);
                            CHECK_NOT_NULL(data);
                            PRECONDITION(IS_NULL(data_ptr[index]));
                            __field_value *ptr = (buffer + index);
                            CHECK_NOT_NULL(ptr);

                            const __native_type *ft = type->get_field(index);
                            CHECK_NOT_NULL(ft);

                            ptr->type = ft;
                            ptr->data = data;

                            data_ptr[index] = ptr;
                        }

                        uint8_t get_field_count() const {
                            return this->field_count;
                        }

                        const __complex_type *get_record_type() const {
                            return type;
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
                                return ptr->type->get_datatype();
                            }
                            return __type_def_enum::TYPE_UNKNOWN;
                        }

                        const __native_type *get_field_def(uint8_t index) const {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr[index];
                            if (NOT_NULL(ptr)) {
                                return ptr->type;
                            }
                            return nullptr;
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
                        const __complex_type *type = nullptr;
                        __field_value *buffer = nullptr;
                        uint8_t field_count = 0;
                        __field_value **data_ptr = nullptr;

                    public:
                        mutable_record_struct(const __complex_type *type, uint8_t field_count) {
                            CHECK_NOT_NULL(type);
                            this->field_count = field_count;
                            data_ptr = new __field_value *[this->field_count];
                            CHECK_ALLOC(data_ptr, TYPE_NAME(__field_value * ));

                            uint32_t size = sizeof(__field_value) * field_count;
                            buffer = (__field_value *) malloc(size);
                            CHECK_ALLOC(buffer, TYPE_NAME(__field_value__));

                            memset(buffer, 0, size);
                            for (uint8_t ii = 0; ii < field_count; ii++) {
                                data_ptr[ii] = nullptr;
                            }
                            this->type = type;
                        }

                        ~mutable_record_struct() {
                            __complex_type_helper *helper = __complex_type_helper::get_type_loader();
                            CHECK_NOT_NULL(helper);
                            for (uint8_t ii = 0; ii < field_count; ii++) {
                                if (IS_NULL(data_ptr[ii]))
                                    continue;
                                __type_def_enum t = data_ptr[ii]->type->get_datatype();
                                if (__type_enum_helper::is_native(t)) {
                                    continue;
                                }
                                helper->free_type_node(type, data_ptr[ii], __record_mode::RM_READ);
                            }
                            FREE_PTR(buffer);
                            FREE_PTR(data_ptr);
                        }

                        uint8_t get_field_count() const {
                            return this->field_count;
                        }

                        const __complex_type *get_record_type() const {
                            return type;
                        }

                        void add_field(const uint8_t index, void *data) {
                            PRECONDITION(index < this->field_count);
                            CHECK_NOT_NULL(data);

                            if (IS_NULL(data_ptr[index])) {
                                data_ptr[index] = (buffer + index);
                            }
                            const __native_type *ft = type->get_field(index);
                            CHECK_NOT_NULL(ft);

                            data_ptr[index]->type = ft;
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
                                return ptr->type->get_datatype();
                            }
                            return __type_def_enum::TYPE_UNKNOWN;
                        }

                        const __native_type *get_field_def(uint8_t index) const {
                            PRECONDITION(index < this->field_count);
                            __field_value *ptr = this->data_ptr[index];
                            if (NOT_NULL(ptr)) {
                                return ptr->type;
                            }
                            return nullptr;
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
                    /*!
                     * Utility class defining helper methods for type creation/instantiation.
                     */
                    class __type_init_utils {
                    public:

                        /*!
                         * Read the inner type definition from the data buffer.
                         *
                         * @param parent- Parent type instance.
                         * @param buffer - Input buffer to read from.
                         * @param offset - Offset in the input buffer to start reading.
                         * @param size - Size field to increment the read offset.
                         * @return - Read type instance.
                         */
                        static __native_type *
                        read_inner_type(__native_type *parent, void *buffer, uint64_t offset, uint32_t *size) {
                            // Read the field type.
                            uint8_t *ft = nullptr;
                            uint64_t r_offset = offset;
                            buffer_utils::read<uint8_t>(buffer, &r_offset, &ft);
                            CHECK_NOT_NULL(ft);

                            __field_type field_type = __field_type_helper::get_type(*ft);
                            if (field_type == __field_type::NATIVE) {
                                __native_type *type = new __native_type(parent);
                                CHECK_ALLOC(type, TYPE_NAME(__native_type));
                                *size += type->read(buffer, offset);
                                return type;
                            } else if (field_type == __field_type::COMPLEX) {
                                __complex_type *type = new __complex_type(parent);
                                CHECK_ALLOC(type, TYPE_NAME(__complex_type));
                                *size += type->read(buffer, offset);
                                return type;
                            }
                            throw BASE_ERROR("Cannot create inner type for datatype. [datatype=%d]", field_type);
                        }

                        /*!
                         * Read a key type instance definition from the input buffer.
                         *
                         * @param parent - Parent type instance.
                         * @param buffer - Buffer to read from.
                         * @param offset - Offset in the buffer to start reading.
                         * @param size - Size field to increment the read offset.
                         * @return - Read key type instance.
                         */
                        static __native_type *
                        read_key_type(__native_type *parent, void *buffer, uint64_t offset, uint32_t *size) {
                            // Read the field type.
                            uint8_t *ft = nullptr;
                            uint64_t r_offset = offset;
                            buffer_utils::read<uint8_t>(buffer, &r_offset, &ft);
                            CHECK_NOT_NULL(ft);

                            __field_type field_type = __field_type_helper::get_type(*ft);
                            if (field_type == __field_type::NATIVE) {
                                __native_type *type = new __native_type(parent);
                                CHECK_ALLOC(type, TYPE_NAME(__native_type));
                                *size += type->read(buffer, offset);
                                POSTCONDITION(__type_enum_helper::is_native(type->get_datatype()));
                                return type;
                            }
                            throw BASE_ERROR("Cannot create inner type for datatype. [datatype=%d]", field_type);
                        }
                    };

                    class __list_type : public __native_type {
                    private:
                        __type_def_enum inner_type;
                        __native_type *inner = nullptr;
                        __base_datatype_io *write_handler = nullptr;

                    public:
                        __list_type(__native_type *parent) : __native_type(parent) {
                            this->type = __field_type::LIST;
                        }

                        __list_type(__native_type *parent, const uint8_t index, const string &name,
                                    const __type_def_enum inner_type, __native_type *type)
                                : __native_type(parent,
                                                index, name,
                                                __type_def_enum::TYPE_LIST) {
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(inner_type));

                            this->inner_type = inner_type;
                            this->type = __field_type::LIST;
                            this->inner = type;

                        }

                        ~__list_type() {
                            CHECK_AND_FREE(inner);
                            CHECK_AND_FREE(write_handler);
                        }

                        void set_inner_type(__native_type *type) {
                            CHECK_NOT_NULL(type);
                            this->inner = type;
                        }

                        const __native_type *get_inner_type() const {
                            return this->inner;
                        }

                        /*!
                         * Get the inner datatype of this list.
                         *
                         * @return - Inner datatype.
                         */
                        const __type_def_enum get_inner_datatype() const {
                            return this->inner_type;
                        }

                        /*!
                         * Set the constraint definition for this field.
                         * Note:: Constraints cannot be defined for arrays. Call will throw exception.
                         *
                         * @param constraint - Field data constraint.
                         */
                        virtual void set_constraint(__constraint *constraint) override {
                            throw BASE_ERROR("Constraints can only be defined for basic types.");
                        }

                        /*!
                         * Set the default value for this field.
                         * Note:: Default value cannot be defined for arrays. Call will throw exception.
                         *
                         * @param default_value - Field default value.
                         */
                        virtual void set_default_value(__default *default_value) override {
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
                            uint32_t w_size = __native_type::write(buffer, offset);
                            w_size += inner->write(buffer, (offset + w_size));
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
                            this->inner = __type_init_utils::read_inner_type(this, buffer, (offset + r_size), &r_size);
                            this->inner_type = this->inner->get_datatype();

                            __complex_type_helper *loader = __complex_type_helper::get_type_loader();
                            CHECK_NOT_NULL(loader);
                            this->type_handler = loader->get_array_type_handler(this->inner_type,
                                                                                this->inner, __record_mode::RM_READ);
                            CHECK_NOT_NULL(this->type_handler);
                            this->write_handler = loader->get_array_type_handler(this->inner_type,
                                                                                 this->inner, __record_mode::RM_WRITE);
                            CHECK_NOT_NULL(this->write_handler);
                            return r_size;
                        }

                        string get_canonical_name() const override {
                            if (NOT_NULL(parent)) {
                                return parent->get_canonical_name();
                            }
                            return this->name;
                        }

                        virtual void print() const override {
                            string t = __type_enum_helper::get_type_string(this->datatype);
                            string it = __type_enum_helper::get_type_string(this->inner_type);
                            string n = inner->get_canonical_name();
                            LOG_DEBUG("[index=%d] [name=%s] %s<%s>", this->index, n.c_str(), t.c_str(), it.c_str());
                            if (this->inner_type == __type_def_enum::TYPE_STRUCT) {
                                this->inner->print();
                            }
                        }

                        virtual void get_field_index(unordered_map<string, __native_type *> *map) override {
                            this->inner->get_field_index(map);
                        }

                        virtual string get_type_name() const override {
                            CHECK_NOT_NULL(inner);
                            string it = inner->get_type_ptr();
                            return common_utils::format("vector<%s>", it.c_str());
                        }

                        __base_datatype_io *get_type_handler(__record_mode mode) override {
                            if (mode == __record_mode::RM_WRITE) {
                                return this->write_handler;
                            } else {
                                return this->type_handler;
                            }
                        }
                    };

                    class __map_type : public __native_type {
                    private:
                        __type_def_enum key_type;
                        __native_type *key = nullptr;
                        __type_def_enum value_type;
                        __native_type *value = nullptr;
                        __base_datatype_io *write_handler = nullptr;

                    public:
                        __map_type(__native_type *parent) : __native_type(parent) {
                            this->type = __field_type::MAP;
                        }

                        __map_type(__native_type *parent, const uint8_t index, const string &name,
                                   const __type_def_enum key_type,
                                   const __type_def_enum value_type, __native_type *value)
                                : __native_type(
                                parent, index, name,
                                __type_def_enum::TYPE_MAP) {
                            PRECONDITION(__type_enum_helper::is_native(key_type));
                            PRECONDITION(__type_enum_helper::is_inner_type_valid(value_type));

                            this->key_type = key_type;
                            string key_name(MAP_TYPE_KEY_NAME);

                            this->value_type = value_type;
                            string value_name(MAP_TYPE_VALUE_NAME);

                            this->type = __field_type::MAP;

                            this->value = value;

                        }

                        ~__map_type() {
                            CHECK_AND_FREE(key);
                            CHECK_AND_FREE(value);
                            CHECK_AND_FREE(write_handler);
                        }

                        void set_key_type(__native_type *type) {
                            CHECK_NOT_NULL(type);
                            this->key = type;
                        }

                        void set_value_type(__native_type *type) {
                            CHECK_NOT_NULL(type);
                            this->value = type;
                        }

                        const __native_type *get_key_type() const {
                            return this->key;
                        }

                        const __native_type *get_value_type() const {
                            return this->value;
                        }

                        /*!
                         * Get the datatype of the Map key.
                         *
                         * @return - Key datatype
                         */
                        const __type_def_enum get_key_datatype() const {
                            return this->key_type;
                        }

                        /*!
                         * Get the datatype of the Map value.
                         *
                         * @return - Value type.
                         */
                        const __type_def_enum get_value_datatype() const {
                            return this->value_type;
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
                            w_size += key->write(buffer, (offset + w_size));
                            w_size += value->write(buffer, (offset + w_size));
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
                            this->key = __type_init_utils::read_key_type(this, buffer, (offset + r_size), &r_size);
                            this->key_type = this->key->get_datatype();
                            this->value = __type_init_utils::read_inner_type(this, buffer, (offset + r_size), &r_size);
                            this->value_type = this->value->get_datatype();

                            __complex_type_helper *loader = __complex_type_helper::get_type_loader();
                            CHECK_NOT_NULL(loader);
                            this->type_handler = loader->get_map_type_handler(this->key_type,
                                                                              this->value_type, this->value,
                                                                              __record_mode::RM_READ);
                            CHECK_NOT_NULL(this->type_handler);
                            this->write_handler = loader->get_map_type_handler(this->key_type,
                                                                               this->value_type, this->value,
                                                                               __record_mode::RM_WRITE);
                            CHECK_NOT_NULL(this->write_handler);
                            return r_size;
                        }

                        string get_canonical_name() const override {
                            if (NOT_NULL(parent)) {
                                return parent->get_canonical_name();
                            }
                            return this->name;
                        }

                        virtual void print() const override {
                            string t = __type_enum_helper::get_type_string(this->datatype);
                            string kt = __type_enum_helper::get_type_string(this->key_type);
                            string vt = __type_enum_helper::get_type_string(this->value_type);
                            string n = this->get_canonical_name();
                            LOG_DEBUG("[index=%d] [name=%s] %s<%s, %s>", this->index, n.c_str(), t.c_str(), kt.c_str(),
                                      vt.c_str());
                            if (this->value_type == __type_def_enum::TYPE_STRUCT) {
                                this->value->print();
                            }
                        }

                        virtual void get_field_index(unordered_map<string, __native_type *> *map) override {
                            this->key->get_field_index(map);
                            this->value->get_field_index(map);
                        }

                        virtual string get_type_name() const override {
                            CHECK_NOT_NULL(this->key);
                            CHECK_NOT_NULL(this->value);
                            string kt = this->key->get_type_name();
                            if (this->key->get_datatype() == __type_def_enum::TYPE_STRING) {
                                kt = "std::string";
                            }
                            string vt = this->value->get_type_ptr();

                            return common_utils::format("unordered_map<%s, %s>", kt.c_str(), vt.c_str());
                        }

                        __base_datatype_io *get_type_handler(__record_mode mode) override {
                            if (mode == __record_mode::RM_WRITE) {
                                return this->write_handler;
                            } else {
                                return this->type_handler;
                            }
                        }
                    };

                }
REACTFS_NS_CORE_END
#endif //REACTFS_SCHEMA_DEF_H
