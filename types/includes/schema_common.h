
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
// Created by Subhabrata Ghosh on 15/12/16.
//

#ifndef REACTFS_SCHEMA_COMMON_H
#define REACTFS_SCHEMA_COMMON_H

#include "common/includes/common.h"
#include "core/includes/core.h"

using namespace REACTFS_NS_COMMON_PREFIX;

REACTFS_NS_CORE
                namespace types {



                    /*!
                     * Singleton utility class for managing data type IO handlers.
                     *
                     * Native type handlers are automatically added. Other type handlers
                     * should be added externally.
                     *
                     * Note:: Needs to be disposed prior to application existing.
                     */
                    class __type_defs_utils {
                    private:
                        /// Thread lock mutex for creating handlers.
                        static mutex thread_mutex;

                        /// Static map of the registered data type IO handlers.
                        static unordered_map<string, __base_datatype_io *> type_handlers;


                        /*!
                         * Create a new instance of the IO handler.
                         *
                         * @param type - Datatype enum
                         * @return - Datatype IO handler
                         */
                        static __base_datatype_io *create_type_handler(__type_def_enum type) {
                            lock_guard<std::mutex> lock(thread_mutex);
                            string type_n = __type_enum_helper::get_type_string(type);
                            unordered_map<string, __base_datatype_io *>::iterator iter = type_handlers.find(type_n);
                            if (iter != type_handlers.end()) {
                                return nullptr;
                            }
                            __base_datatype_io *t = nullptr;
                            switch (type) {
                                case __type_def_enum::TYPE_BYTE:
                                    t = new __dt_byte();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_byte));
                                    break;
                                case __type_def_enum::TYPE_CHAR:
                                    t = new __dt_char();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_char));
                                    break;
                                case __type_def_enum::TYPE_DOUBLE:
                                    t = new __dt_double();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_double));
                                    break;
                                case __type_def_enum::TYPE_FLOAT:
                                    t = new __dt_float();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_float));
                                    break;
                                case __type_def_enum::TYPE_INTEGER:
                                    t = new __dt_int();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_int));
                                    break;
                                case __type_def_enum::TYPE_LONG:
                                    t = new __dt_long();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_long));
                                    break;
                                case __type_def_enum::TYPE_SHORT:
                                    t = new __dt_short();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_short));
                                    break;
                                case __type_def_enum::TYPE_STRING:
                                    t = new __dt_string();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_string));
                                    break;
                                case __type_def_enum::TYPE_STRUCT:
                                    t = new __dt_byte();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_byte));
                                    break;
                                case __type_def_enum::TYPE_TEXT:
                                    t = new __dt_text();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_text));
                                    break;
                                case __type_def_enum::TYPE_TIMESTAMP:
                                    t = new __dt_timestamp();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_timestamp));
                                    break;
                                case __type_def_enum::TYPE_BOOL:
                                    t = new __dt_bool();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_bool));
                                    break;
                                case __type_def_enum::TYPE_DATETIME:
                                    t = new __dt_datetime();
                                    CHECK_ALLOC(t, TYPE_NAME(__dt_datetime));
                                    break;
                                default:
                                    throw BASE_ERROR("Type not supported as basic type handler. [type=%s]",
                                                     __type_enum_helper::get_type_string(type).c_str());
                            }
                            return t;
                        }

                    public:

                        static string create_list_key(__type_def_enum type, __type_def_enum inner_type) {
                            return common_utils::format("%s::%s", __type_enum_helper::get_type_string(type).c_str(),
                                                        __type_enum_helper::get_type_string(inner_type).c_str());
                        }

                        static string
                        create_map_key(__type_def_enum type, __type_def_enum key_type, __type_def_enum value_type) {
                            return common_utils::format("%s::%s::%s", __type_enum_helper::get_type_string(type).c_str(),
                                                        __type_enum_helper::get_type_string(key_type).c_str(),
                                                        __type_enum_helper::get_type_string(value_type).c_str());
                        }

                        /*!
                         * Get the datatype IO handler for the specified datatype enum.
                         *
                         * Note: Only valid inner type datatype handlers are auto-created, others should be
                         * added explicitly.
                         *
                         * @param type - Datatype enum
                         * @return - Datatype IO handler
                         */
                        static __base_datatype_io *get_type_handler(__type_def_enum type) {
                            PRECONDITION(__type_enum_helper::is_native(type) || type == __type_def_enum::TYPE_TEXT);
                            string type_n = __type_enum_helper::get_type_string(type);
                            unordered_map<string, __base_datatype_io *>::iterator iter = type_handlers.find(type_n);
                            if (iter != type_handlers.end()) {
                                return iter->second;
                            } else {
                                if (__type_enum_helper::is_native(type) || type == __type_def_enum::TYPE_TEXT) {
                                    // If not available and native type, create a new instance.
                                    __base_datatype_io *ptr = create_type_handler(type);
                                    if (IS_NULL(ptr)) {
                                        // Another thread should have added this type handler.
                                        iter = type_handlers.find(type_n);
                                        if (iter != type_handlers.end()) {
                                            return iter->second;
                                        }
                                    } else {
                                        type_handlers.insert({type_n, ptr});
                                        return ptr;
                                    }
                                }
                            }
                            return nullptr;
                        }

                        /*!
                         * Get the datatype IO handler for the specified datatype key.
                         *
                         * Note: Only valid inner type datatype handlers are auto-created, others should be
                         * added explicitly.
                         *
                         * @param type - Datatype enum
                         * @return - Datatype IO handler
                         */
                        static __base_datatype_io *get_type_handler(string &key) {
                            CHECK_NOT_EMPTY(key);
                            unordered_map<string, __base_datatype_io *>::iterator iter = type_handlers.find(key);
                            if (iter != type_handlers.end()) {
                                return iter->second;
                            }
                            return nullptr;
                        }

                        /*!
                         * Add a non-native IO handler that was instantiated externally.
                         *
                         *
                         * @param key - String key for array/list/map types
                         * @param handler - Handler instance.
                         * @return - Has been added? Will return false if handler already registered.
                         */
                        static bool add_external_handler(string &key, __base_datatype_io *handler) {
                            CHECK_NOT_NULL(handler);
                            CHECK_NOT_EMPTY(key);
                            lock_guard<std::mutex> lock(thread_mutex);
                            unordered_map<string, __base_datatype_io *>::iterator iter = type_handlers.find(key);
                            if (iter != type_handlers.end()) {
                                return false;
                            }
                            type_handlers.insert({key, handler});
                            return true;
                        }

                        /*!
                         * Add a non-native IO handler that was instantiated externally.
                         *
                         *
                         * @param type - Datatype enum
                         * @param handler - Handler instance.
                         * @return - Has been added? Will return false if handler already registered.
                         */
                        static bool add_external_handler(__type_def_enum type, __base_datatype_io *handler) {
                            CHECK_NOT_NULL(handler);
                            lock_guard<std::mutex> lock(thread_mutex);
                            string type_n = __type_enum_helper::get_type_string(type);
                            unordered_map<string, __base_datatype_io *>::iterator iter = type_handlers.find(type_n);
                            if (iter != type_handlers.end()) {
                                return false;
                            }
                            type_handlers.insert({type_n, handler});
                            return true;
                        }

                        /*!
                         * Dispose all the registered instances of the IO handlers.
                         */
                        static void dispose() {
                            lock_guard<std::mutex> lock(thread_mutex);
                            unordered_map<string, __base_datatype_io *>::iterator iter;
                            for (iter = type_handlers.begin(); iter != type_handlers.end(); iter++) {
                                CHECK_AND_FREE(iter->second);
                            }
                            type_handlers.clear();
                        }
                    };

                    /*!
                     * Absract base class for defining type constraints.
                     */
                    class __constraint {
                    protected:
                        bool is_not = false;
                    public:
                        /*!
                         * Virtual base desctructor.
                         */
                        virtual ~__constraint() {}

                        /*!
                         * Validate the data by applying this constraint instance.
                         *
                         * @param value - Data value to validate
                         * @return - Constraint passed?
                         */
                        virtual bool validate(void *value) const = 0;

                        /*!
                         * Write (serialize) this constraint instance.
                         *
                         * @param buffer - Output buffer to write the constraint to.
                         * @param offset - Offset in the buffer to start writing.
                         * @return - Number of byte written.
                         */
                        virtual uint32_t write(void *buffer, uint64_t offset) = 0;

                        /*!
                         * Read (de-serialize) the constraint instance.
                         *
                         * @param buffer - Input buffer to read data from.
                         * @param offset - Offset in the input buffer to read from.
                         * @return - Number of bytes consumed.
                         */
                        virtual uint32_t read(void *buffer, uint64_t offset) = 0;

                        /*!
                         * Get the datatype enum that this constraint instance supports.
                         *
                         * @return - Supported datatype enum.
                         */
                        virtual __type_def_enum get_datatype() = 0;

                        /*!
                         * Set if this constraint should return the negative of the constraint definition.
                         *
                         * @param is_not - Should be reversed?
                         */
                        void set_not(bool is_not) {
                            this->is_not = is_not;
                        }
                    };

                    /*!
                     * Abstract base class for defining default values.
                     */
                    class __default {
                    protected:
                        /// Datatype IO handler.
                        __base_datatype_io *handler = nullptr;

                    public:

                        /*!<destructor
                         * Base virtual destructor.
                         */
                        virtual ~__default() {

                        }

                        /*!
                         * Write (serialize) this default value instance.
                         *
                         * @param buffer - Output buffer to write the constraint to.
                         * @param offset - Offset in the buffer to start writing.
                         * @return - Number of byte written.
                         */
                        virtual uint32_t write(void *buffer, uint64_t offset) = 0;

                        /*!
                         * Read (de-serialize) the default value instance.
                         *
                         * @param buffer - Input buffer to read data from.
                         * @param offset - Offset in the input buffer to read from.
                         * @return - Number of bytes consumed.
                         */
                        virtual uint32_t read(void *buffer, uint64_t offset) = 0;

                        /*!
                         * Get the datatype enum that this default value instance supports.
                         *
                         * @return - Supported datatype enum.
                         */
                        virtual __type_def_enum get_datatype() = 0;

                        /*!
                         * Set the passed pointer to the default value.
                         *
                         * @param value - Value pointer.
                         */
                        virtual void set_default(void *value) const = 0;
                    };

                    /*!
                     * Template base class for defining typed default value instances.
                     *
                     * @tparam __T - Datatype of the value.
                     * @tparam __type - Datatype enum of the value.
                     */
                    template<typename __T, __type_def_enum __type>
                    class __typed_default : public __default {
                    private:
                        /// Datatype enum of the value.
                        __type_def_enum datatype = __type;
                        /// Typed default value
                        __T value;
                    public:
                        /*!<constructor
                         * Default empty constructor.
                         */
                        __typed_default() {
                            PRECONDITION(__type_enum_helper::is_native(datatype));
                            handler = __type_defs_utils::get_type_handler(this->datatype);
                            CHECK_NOT_NULL(handler);
                        }

                        /*!
                         * Get the default value.
                         *
                         * @return - Typed default value.
                         */
                        const __T get_value() const {
                            CHECK_NOT_NULL(value);
                            return value;
                        }

                        /*!
                         * Set the default value.
                         *
                         * @param value - Typed default value.
                         */
                        void set_value(__T value) {
                            this->value = value;
                        }

                        /*!
                         * Set the passed pointer to the default value.
                         *
                         * @param value - Value pointer.
                         */
                        void set_default(void *value) const override {
                            __T *t = (__T *) value;
                            CHECK_NOT_NULL(t);
                            *t = this->value;
                        }

                        /*!
                         * Get the datatype enum of the supported value type.
                         *
                         * @return - Supported datatype enum.
                         */
                        __type_def_enum get_datatype() override {
                            return this->datatype;
                        }

                        /*!
                         * Write (serialize) this default value instance.
                         *
                         * @param buffer - Output buffer to write the constraint to.
                         * @param offset - Offset in the buffer to start writing.
                         * @return - Number of byte written.
                         */
                        uint32_t write(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            return handler->write(buffer, &value, offset, ULONG_MAX);
                        }

                        /*!
                         * Read (de-serialize) the default value instance.
                         *
                         * @param buffer - Input buffer to read data from.
                         * @param offset - Offset in the input buffer to read from.
                         * @return - Number of bytes consumed.
                         */
                        uint32_t read(void *buffer, uint64_t offset) override {
                            CHECK_NOT_NULL(buffer);
                            CHECK_NOT_NULL(handler);

                            __T *t = nullptr;
                            uint64_t r_size = handler->read(buffer, &t, offset, ULONG_MAX);
                            CHECK_NOT_NULL(t);

                            this->value = *t;

                            return r_size;
                        }
                    };

                    typedef __typed_default<char, __type_def_enum::TYPE_CHAR> __default_char;
                    typedef __typed_default<bool, __type_def_enum::TYPE_BOOL> __default_bool;
                    typedef __typed_default<short, __type_def_enum::TYPE_SHORT> __default_short;
                    typedef __typed_default<int, __type_def_enum::TYPE_INTEGER> __default_int;
                    typedef __typed_default<long, __type_def_enum::TYPE_LONG> __default_long;
                    typedef __typed_default<uint64_t, __type_def_enum::TYPE_TIMESTAMP> __default_timestamp;
                    typedef __typed_default<float, __type_def_enum::TYPE_FLOAT> __default_float;
                    typedef __typed_default<double, __type_def_enum::TYPE_DOUBLE> __default_double;
                    typedef __typed_default<string, __type_def_enum::TYPE_STRING> __default_string;

                }
REACTFS_NS_CORE_END
#endif //REACTFS_SCHEMA_COMMON_H
