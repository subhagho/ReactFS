#ifndef REACTFS_CPP_TEMPLATE_HEADER_H
#define REACTFS_CPP_TEMPLATE_HEADER_H
#include <unordered_map>
#include "common/includes/common.h"
#include "common/includes/common_utils.h"
#include "common/includes/base_error.h"
#include "common/includes/log_utils.h"
#include "core/includes/core.h"

using namespace REACTFS_NS_COMMON_PREFIX;
REACTFS_NS_CORE
namespace parsers {		
					class cpp_template_header {
					private:
						unordered_map<string, vector<string> *> __cpp_template;

						void __init() {
							std::vector<string> *values = nullptr;

							// KEY [FILE_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FILE_DEF", values});
							values->push_back("${header}");
							values->push_back("");
							values->push_back("#ifndef ${guard_name}");
							values->push_back("#define ${guard_name}");
							values->push_back("${includes}");
							values->push_back("");
							values->push_back("${body}");
							values->push_back("#endif // ${guard_name}");
							// END KEY [FILE_DEF]

							// KEY [CLASS_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CLASS_DEF", values});
							values->push_back("/**");
							values->push_back(" * Generated code for data record type ${name}.");
							values->push_back(" *");
							values->push_back(" * Note:: Should not be modified as the changes will be lost when the code is re-generated.");
							values->push_back(" */");
							values->push_back("class ${name} ${parent} {");
							values->push_back("private:");
							values->push_back("    ${declarations}");
							values->push_back("");
							values->push_back("    ${private_functions}");
							values->push_back("public:");
							values->push_back("    ${public_functions}");
							values->push_back("};");
							// END KEY [CLASS_DEF]

							// KEY [INCLUDE_BASE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"INCLUDE_BASE", values});
							values->push_back("#include \"types/includes/type_defs.h\"");
							values->push_back("#include \"types/includes/__base_type.h\"");
							values->push_back("");
							values->push_back("${includes}");
							values->push_back("");
							values->push_back("using namespace REACTFS_NS_COMMON_PREFIX;");
							values->push_back("using namespace REACTFS_NS_CORE_PREFIX::types;");
							values->push_back("");
							// END KEY [INCLUDE_BASE]

							// KEY [COPY_CALL_STRING]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"COPY_CALL_STRING", values});
							values->push_back("// Set ${name} from the source value.");
							values->push_back("if (NOT_NULL(source.${name})) {");
							values->push_back("	string __${name} = string(source.${name});");
							values->push_back("	this->set_${name}(__${name});");
							values->push_back("}");
							// END KEY [COPY_CALL_STRING]

							// KEY [COPY_CALL_NATIVE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"COPY_CALL_NATIVE", values});
							values->push_back("// Set ${name} from the source value.");
							values->push_back("if (NOT_NULL(source.${name})) {");
							values->push_back("	this->set_${name}(*(source.${name}));");
							values->push_back("}");
							// END KEY [COPY_CALL_NATIVE]

							// KEY [FUNC_CONSTRUCTOR_COPY]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_CONSTRUCTOR_COPY", values});
							values->push_back("/**");
							values->push_back(" * Copy constructor to create a copy instance of ${name}.");
							values->push_back(" * Copy instances should be used to update existing records.");
							values->push_back(" *");
							values->push_back(" * @param source - Source instance of ${name} to copy from.");
							values->push_back(" */");
							values->push_back("${name}(const ${name} &source) {");
							values->push_back("    	this->__is_allocated = true;");
							values->push_back("    	${variable_inits}");
							values->push_back("	${variable_copy}	");
							values->push_back("}");
							// END KEY [FUNC_CONSTRUCTOR_COPY]

							// KEY [CALL_TYPE_LIST_SETTER_FROM_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_TYPE_LIST_SETTER_FROM_MAP", values});
							values->push_back("// Set the value of list ${name}");
							values->push_back("std::vector<${type_ptr}> *__var = deserialize_list_${m_name}(__ptr);");
							values->push_back("CHECK_NOT_NULL(__var);");
							values->push_back("this->${name} = __var;");
							// END KEY [CALL_TYPE_LIST_SETTER_FROM_MAP]

							// KEY [COPY_CALL_TYPE_LIST]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"COPY_CALL_TYPE_LIST", values});
							values->push_back("// Deserialize the list ${name} from the source value.");
							values->push_back("if (NOT_EMPTY_P(source.${name})) {");
							values->push_back("	std::vector<${type_ptr}> *__list = new std::vector<${type_ptr}>();");
							values->push_back("	CHECK_ALLOC(__list, TYPE_NAME(vector));");
							values->push_back("	for( ${type_ptr} v : *(source.${name})) {");
							values->push_back("		CHECK_NOT_NULL(v);");
							values->push_back("		${type_ptr} __tv = new ${type}(*v);");
							values->push_back("		CHECK_NOT_NULL(__tv);");
							values->push_back("		__list->push_back(__tv);");
							values->push_back("	}");
							values->push_back("	this->${name} = __list;");
							values->push_back("}");
							// END KEY [COPY_CALL_TYPE_LIST]

							// KEY [COPY_CALL_NATIVE_LIST]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"COPY_CALL_NATIVE_LIST", values});
							values->push_back("// Deserialize the list ${name} from the source value.");
							values->push_back("if (NOT_EMPTY_P(source.${name})) {");
							values->push_back("	std::vector<${type_ptr}> *__list = new std::vector<${type_ptr}>();");
							values->push_back("	CHECK_ALLOC(__list, TYPE_NAME(vector));");
							values->push_back("	for( ${type_ptr} v : *(source.${name})) {");
							values->push_back("		CHECK_NOT_NULL(v);");
							values->push_back("		${type_ptr} __tv = (${type_ptr}) malloc (sizeof(${type}));");
							values->push_back("		CHECK_ALLOC(__tv, TYPE_NAME(${type}));");
							values->push_back("		*__tv = *v;");
							values->push_back("		__list->push_back(__tv);");
							values->push_back("	}");
							values->push_back("	this->${name} = __list;");
							values->push_back("}");
							// END KEY [COPY_CALL_NATIVE_LIST]

							// KEY [CALL_FREE_TYPE_MAP_DATA_PTR]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_FREE_TYPE_MAP_DATA_PTR", values});
							values->push_back("// Free the map pointer and the list values, if locally allocated.");
							values->push_back("iter = __data->find(\"${name}\");");
							values->push_back("if (iter != __data->end()) {");
							values->push_back("	if (NOT_NULL(iter->second)) {");
							values->push_back("		void *__ptr = iter->second;");
							values->push_back("		std::unordered_map<${key_type}, ${value_type_ptr}> *__m_ptr = static_cast<std::unordered_map<${key_type}, ${value_type_ptr}> *>(__ptr);");
							values->push_back("		CHECK_CAST(__m_ptr, TYPE_NAME(void), TYPE_NAME(unordered_map));");
							values->push_back("		if (allocated) {");
							values->push_back("			FREE_NATIVE_MAP(__m_ptr);");
							values->push_back("		} else {");
							values->push_back("			CHECK_AND_FREE(__m_ptr);");
							values->push_back("		}");
							values->push_back("	}");
							values->push_back("}");
							// END KEY [CALL_FREE_TYPE_MAP_DATA_PTR]

							// KEY [FUNC_SETTER_FROM_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_SETTER_FROM_MAP", values});
							values->push_back("/**");
							values->push_back(" * Value ${name} setter from the the serialized data map.");
							values->push_back(" *");
							values->push_back(" * @param __data - Serialized data map pointer.");
							values->push_back(" */");
							values->push_back("void set_value_${name}(__struct_datatype__ *__data) {");
							values->push_back("    if (NOT_NULL(__data)) {");
							values->push_back("        __struct_datatype__::const_iterator iter = __data->find(\"${name}\");");
							values->push_back("        if (iter != __data->end()) {");
							values->push_back("            void *__ptr = iter->second;");
							values->push_back("            if (NOT_NULL(__ptr)) {");
							values->push_back("		${read_map_calls}");
							values->push_back("            } else {");
							values->push_back("                this->${name} = nullptr;");
							values->push_back("            }");
							values->push_back("        } else {");
							values->push_back("            this->${name} = nullptr;");
							values->push_back("        }");
							values->push_back("    } else {");
							values->push_back("        this->${name} = nullptr;");
							values->push_back("    }");
							values->push_back("}");
							// END KEY [FUNC_SETTER_FROM_MAP]

							// KEY [FUNC_MAP_STRING_ADD_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_MAP_STRING_ADD_DEF", values});
							values->push_back("/**");
							values->push_back(" * Add a map record of key/value type ${key_type}/${value_type_ptr}.");
							values->push_back(" *");
							values->push_back(" * @param m_key - Map key of type ${key_type}");
							values->push_back(" * @param m_value - String value of the map record value.");
							values->push_back(" */");
							values->push_back("void add_to_${name}(${key_type} m_key, string &m_value) {");
							values->push_back("    PRECONDITION(this->__is_allocated == true);");
							values->push_back("    if (IS_NULL(this->${name})) {");
							values->push_back("        this->${name} = new std::unordered_map<${key_type}, ${value_type_ptr}>();");
							values->push_back("        CHECK_ALLOC(this->${name}, TYPE_NAME(unordered_map));");
							values->push_back("    }");
							values->push_back("    CHECK_NOT_EMPTY(m_value);");
							values->push_back("    uint32_t __size = (m_value.length() + 1) * sizeof(char);");
							values->push_back("    char *__var = (char *)malloc(sizeof(char) * __size);");
							values->push_back("    CHECK_ALLOC(__var, TYPE_NAME(char *));");
							values->push_back("    memset(__var, 0, __size);");
							values->push_back("    memcpy(__var, m_value.c_str(), (__size - 1));");
							values->push_back("    this->${name}->insert({m_key, __var});");
							values->push_back("}");
							// END KEY [FUNC_MAP_STRING_ADD_DEF]

							// KEY [FUNC_DESTRUCTOR]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_DESTRUCTOR", values});
							values->push_back("/**");
							values->push_back(" * Descructor for ${name}");
							values->push_back(" */");
							values->push_back("~${name}() {");
							values->push_back("    	${variable_frees}");
							values->push_back("	__base_type::free_data_ptr();");
							values->push_back("}");
							// END KEY [FUNC_DESTRUCTOR]

							// KEY [FUNC_MAP_NATIVE_ADD_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_MAP_NATIVE_ADD_DEF", values});
							values->push_back("/**");
							values->push_back(" * Add a key/value pair to the map of key type ${key_type}");
							values->push_back(" * and value type ${value_type}.");
							values->push_back(" *");
							values->push_back(" * Method should only be used when this instance is being");
							values->push_back(" * used to create or update a new record instance.");
							values->push_back(" *");
							values->push_back(" * @param m_key - Map key value.");
							values->push_back(" * @param m_value - Map value.");
							values->push_back(" */ ");
							values->push_back("void add_to_${name}(${key_type} m_key, ${value_type} &m_value) {");
							values->push_back("    PRECONDITION(this->__is_allocated == true);");
							values->push_back("    if (IS_NULL(this->${name})) {");
							values->push_back("        this->${name} = new std::unordered_map<${key_type}, ${value_type_ptr}>();");
							values->push_back("        CHECK_ALLOC(this->${name}, TYPE_NAME(unordered_map));");
							values->push_back("    }");
							values->push_back("    ${value_type_ptr} __var = (${value_type_ptr})malloc(sizeof(${value_type}));");
							values->push_back("    CHECK_ALLOC(__var, TYPE_NAME(${value_type}));");
							values->push_back("    *__var = m_value;");
							values->push_back("    this->${name}->insert({m_key, __var});");
							values->push_back("}");
							// END KEY [FUNC_MAP_NATIVE_ADD_DEF]

							// KEY [CALL_TYPE_MAP_SETTER_TO_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_TYPE_MAP_SETTER_TO_MAP", values});
							values->push_back("// Serialize the map ${name} and add to the data map.");
							values->push_back("void *__ptr = serialize_map_${m_name}(this->${name});");
							values->push_back("CHECK_NOT_NULL(__ptr);");
							values->push_back("__data->insert({\"${name}\", __ptr});");
							// END KEY [CALL_TYPE_MAP_SETTER_TO_MAP]

							// KEY [FUNC_LIST_TYPE_ADD_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_LIST_TYPE_ADD_DEF", values});
							values->push_back("/**");
							values->push_back(" * Add a type element to the list ${name}. ");
							values->push_back(" * Elements are expected to be pre-allocated type ");
							values->push_back(" * pointer.");
							values->push_back(" *");
							values->push_back(" * Method should only be used when this instance is being");
							values->push_back(" * used to create or update a new record instance.");
							values->push_back(" *");
							values->push_back(" * @param ${name} - String value to add to list.");
							values->push_back(" */");
							values->push_back("void add_to_${name}(${type_ptr} ${name}) {");
							values->push_back("    PRECONDITION(this->__is_allocated == true);");
							values->push_back("    CHECK_NOT_NULL(${name});");
							values->push_back("    if (IS_NULL(this->${name})) {");
							values->push_back("        this->${name} = new std::vector<${type_ptr}>();");
							values->push_back("        CHECK_ALLOC(this->${name}, TYPE_NAME(vector));");
							values->push_back("    }");
							values->push_back("    this->${name}->push_back(${name});");
							values->push_back("}");
							// END KEY [FUNC_LIST_TYPE_ADD_DEF]

							// KEY [NAMESPACE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"NAMESPACE", values});
							values->push_back("namespace ${name} {");
							values->push_back("    ${nested}");
							values->push_back("}");
							// END KEY [NAMESPACE]

							// KEY [VARIABLE_MAP_TYPE_FREE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"VARIABLE_MAP_TYPE_FREE", values});
							values->push_back("FREE_TYPE_MAP(this->${name});");
							// END KEY [VARIABLE_MAP_TYPE_FREE]

							// KEY [FUNC_LIST_NATIVE_ADD_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_LIST_NATIVE_ADD_DEF", values});
							values->push_back("/**");
							values->push_back(" * Add an element to the list ${name}. ");
							values->push_back(" * Elements are expected to be pre-assigned data");
							values->push_back(" * value.");
							values->push_back(" *");
							values->push_back(" * Method should only be used when this instance is being");
							values->push_back(" * used to create or update a new record instance.");
							values->push_back(" *");
							values->push_back(" * @param ${name} - Data value to add to list.");
							values->push_back(" */");
							values->push_back("void add_to_${name}(${type} &${name}) {");
							values->push_back("    PRECONDITION(this->__is_allocated == true);");
							values->push_back("    if (IS_NULL(this->${name})) {");
							values->push_back("        this->${name} = new std::vector<${type_ptr}>();");
							values->push_back("        CHECK_ALLOC(this->${name}, TYPE_NAME(vector));");
							values->push_back("    }");
							values->push_back("    ${type_ptr} __var = (${type_ptr})malloc(sizeof(${type}));");
							values->push_back("    CHECK_ALLOC(__var, TYPE_NAME(${type}));");
							values->push_back("    *__var = ${name};");
							values->push_back("    this->${name}->push_back(__var);");
							values->push_back("}");
							// END KEY [FUNC_LIST_NATIVE_ADD_DEF]

							// KEY [VARIABLE_NATIVE_FREE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"VARIABLE_NATIVE_FREE", values});
							values->push_back("if (this->__is_allocated) {");
							values->push_back("	FREE_PTR(this->${name});");
							values->push_back("}");
							// END KEY [VARIABLE_NATIVE_FREE]

							// KEY [FUNC_NATIVE_SETTER_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_NATIVE_SETTER_DEF", values});
							values->push_back("/**");
							values->push_back(" * Set the value to ${name}.");
							values->push_back(" * ");
							values->push_back(" * Method should only be used when this instance is being");
							values->push_back(" * used to create or update a new record instance.");
							values->push_back(" *");
							values->push_back(" * @Param ${name} - Value reference.");
							values->push_back(" */");
							values->push_back("void set_${name}(${type} &${name}) {");
							values->push_back("    PRECONDITION(this->__is_allocated == true);");
							values->push_back("	if (IS_NULL(this->${name})) {");
							values->push_back("    		this->${name} = (${type_ptr})malloc(sizeof(${type}));");
							values->push_back("    		CHECK_ALLOC(this->${name}, TYPE_NAME(${type}));");
							values->push_back("	}");
							values->push_back("    	*(this->${name}) = ${name};");
							values->push_back("}");
							// END KEY [FUNC_NATIVE_SETTER_DEF]

							// KEY [VARIABLE_TYPE_FREE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"VARIABLE_TYPE_FREE", values});
							values->push_back("CHECK_AND_FREE(this->${name});");
							// END KEY [VARIABLE_TYPE_FREE]

							// KEY [CALL_FREE_NATIVE_DATA_PTR]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_FREE_NATIVE_DATA_PTR", values});
							values->push_back("// Free the ${name} value if locally allocated");
							values->push_back("iter = __data->find(\"${name}\");");
							values->push_back("if (iter != __data->end()) {");
							values->push_back("	if (NOT_NULL(iter->second)) {");
							values->push_back("		void *__ptr = iter->second;");
							values->push_back("		if (allocated) {");
							values->push_back("			FREE_PTR(__ptr);");
							values->push_back("		}");
							values->push_back("	}");
							values->push_back("}");
							// END KEY [CALL_FREE_NATIVE_DATA_PTR]

							// KEY [FUNC_DESERIALIZE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_DESERIALIZE", values});
							values->push_back("/**");
							values->push_back(" * Deserialize this type instance from the passed data map.");
							values->push_back(" *");
							values->push_back(" * @param __data - Serialized data map instance to load this type from.");
							values->push_back(" */");
							values->push_back("void deserialize(__struct_datatype__ *__data) override {");
							values->push_back("    CHECK_NOT_NULL(__data);");
							values->push_back("    ${read_map_calls}");
							values->push_back("    this->__data = __data;");
							values->push_back("}");
							// END KEY [FUNC_DESERIALIZE]

							// KEY [VARIABLE_LIST_NATIVE_FREE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"VARIABLE_LIST_NATIVE_FREE", values});
							values->push_back("if (this->__is_allocated) {");
							values->push_back("	FREE_NATIVE_LIST(this->${name});");
							values->push_back("} else {");
							values->push_back("	this->${name}->clear();");
							values->push_back("	CHECK_AND_FREE(this->${name});");
							values->push_back("}");
							// END KEY [VARIABLE_LIST_NATIVE_FREE]

							// KEY [COPY_CALL_NATIVE_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"COPY_CALL_NATIVE_MAP", values});
							values->push_back("// Deserialize the map ${name} from the source value.");
							values->push_back("if (NOT_EMPTY_P(source.${name})) {");
							values->push_back("	std::unordered_map<${key_type}, ${value_type_ptr}> *__map = new std::unordered_map<${key_type}, ${value_type_ptr}>();");
							values->push_back("	CHECK_ALLOC(__map, TYPE_NAME(unordered_map));");
							values->push_back("");
							values->push_back("	std::unordered_map<${key_type}, ${value_type_ptr} >::iterator iter;");
							values->push_back("	for (iter = source.${name}->begin(); iter != source.${name}->end(); iter++) {");
							values->push_back("		CHECK_NOT_NULL(iter->second);");
							values->push_back("		${value_type_ptr} __tv = (${value_type_ptr}) malloc(sizeof(${value_type}));");
							values->push_back("		CHECK_ALLOC(__tv, TYPE_NAME(${value_type}));");
							values->push_back("		*__tv = *(iter->second);");
							values->push_back("		__map->insert({iter->first, __tv});");
							values->push_back("	}	");
							values->push_back("	this->${name} = __map;");
							values->push_back("}");
							// END KEY [COPY_CALL_NATIVE_MAP]

							// KEY [VARIABLE_LIST_TYPE_FREE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"VARIABLE_LIST_TYPE_FREE", values});
							values->push_back("FREE_TYPE_LIST(this->${name});");
							// END KEY [VARIABLE_LIST_TYPE_FREE]

							// KEY [VARIABLE_NATIVE_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"VARIABLE_NATIVE_DEF", values});
							values->push_back("${type} ${name} = nullptr;");
							// END KEY [VARIABLE_NATIVE_DEF]

							// KEY [CALL_TYPE_MAP_SETTER_FROM_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_TYPE_MAP_SETTER_FROM_MAP", values});
							values->push_back("// Set the value of map ${name}");
							values->push_back("std::unordered_map<${key_type}, ${value_type_ptr}> *__var = deserialize_map_${m_name}(__ptr);");
							values->push_back("CHECK_NOT_NULL(__var);");
							values->push_back("this->${name} = __var;");
							// END KEY [CALL_TYPE_MAP_SETTER_FROM_MAP]

							// KEY [VARIABLE_MAP_NATIVE_FREE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"VARIABLE_MAP_NATIVE_FREE", values});
							values->push_back("if (this->__is_allocated) {");
							values->push_back("	FREE_NATIVE_MAP(this->${name});	");
							values->push_back("} else {");
							values->push_back("	this->${name}->clear();");
							values->push_back("	CHECK_AND_FREE(this->${name});");
							values->push_back("}");
							// END KEY [VARIABLE_MAP_NATIVE_FREE]

							// KEY [COPY_CALL_TYPE_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"COPY_CALL_TYPE_MAP", values});
							values->push_back("// Deserialize the map ${name} from the source value.");
							values->push_back("if (NOT_EMPTY_P(source.${name})) {");
							values->push_back("	std::unordered_map<${key_type}, ${value_type_ptr}> *__map = new std::unordered_map<${key_type}, ${value_type_ptr}>();");
							values->push_back("	CHECK_ALLOC(__map, TYPE_NAME(unordered_map));");
							values->push_back("");
							values->push_back("	std::unordered_map<${key_type}, ${value_type_ptr} >::iterator iter;");
							values->push_back("	for (iter = source.${name}->begin(); iter != source.${name}->end(); iter++) {");
							values->push_back("		CHECK_NOT_NULL(iter->second);");
							values->push_back("		${value_type_ptr} __tv = new ${value_type}(*iter->second);");
							values->push_back("		CHECK_ALLOC(__tv, TYPE_NAME(${value_type}));");
							values->push_back("		__map->insert({iter->first, __tv});");
							values->push_back("	}	");
							values->push_back("	this->${name} = __map;");
							values->push_back("}");
							// END KEY [COPY_CALL_TYPE_MAP]

							// KEY [FUNC_TYPE_DESERIALIZER]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_TYPE_DESERIALIZER", values});
							values->push_back("/**");
							values->push_back(" * Deserialize an instance of type ${name} from the ");
							values->push_back(" * passed input data.");
							values->push_back(" *");
							values->push_back(" * @param __input - Void pointer to input data of type __struct_datatype__");
							values->push_back(" * @return - Deserialized instance pointer of type ${type_ptr}");
							values->push_back(" */");
							values->push_back("${type_ptr} deserialize_${name}(void *__input) {");
							values->push_back("    CHECK_NOT_NULL(__input);");
							values->push_back("    __struct_datatype__ *__value = static_cast<__struct_datatype__ *>(__input);");
							values->push_back("    CHECK_CAST(__value, TYPE_NAME(void *), TYPE_NAME(__struct_datatype__));");
							values->push_back("    ${type_ptr} __data = new ${type}(__value);");
							values->push_back("    CHECK_ALLOC(__data, TYPE_NAME(${type}));");
							values->push_back("    return __data;");
							values->push_back("}");
							// END KEY [FUNC_TYPE_DESERIALIZER]

							// KEY [FUNC_GETTER_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_GETTER_DEF", values});
							values->push_back("/**");
							values->push_back(" * Get the pointer to the property ${name} of type ${return}.");
							values->push_back(" * Returns a const pointer.");
							values->push_back(" * ");
							values->push_back(" * @return ${return}");
							values->push_back(" */");
							values->push_back("const ${return} get_${name}(void) const {");
							values->push_back("    return this->${name};");
							values->push_back("}");
							// END KEY [FUNC_GETTER_DEF]

							// KEY [FUNC_SETTER_PTR_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_SETTER_PTR_DEF", values});
							values->push_back("/**");
							values->push_back(" * Set the pointer to ${name}.");
							values->push_back(" * ");
							values->push_back(" * @param ${name} - Pointer of type ${type}.");
							values->push_back(" */");
							values->push_back("void set_${name}(${type} ${name}) {");
							values->push_back("    this->${name} = ${name};");
							values->push_back("}");
							// END KEY [FUNC_SETTER_PTR_DEF]

							// KEY [FUNC_SETTER_TO_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_SETTER_TO_MAP", values});
							values->push_back("/**");
							values->push_back(" * Method extracts and set the value of ${name} from");
							values->push_back(" * the serialized data map.");
							values->push_back(" *");
							values->push_back(" * @param __data - Serialized data map pointer.");
							values->push_back(" */");
							values->push_back("void set_map_${name}(__struct_datatype__ *__data) {");
							values->push_back("    CHECK_NOT_NULL(__data);");
							values->push_back("    if (NOT_NULL(this->${name})) {");
							values->push_back("	${set_map_calls}	");
							values->push_back("    }");
							values->push_back("}");
							// END KEY [FUNC_SETTER_TO_MAP]

							// KEY [FILE_COMMENT]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FILE_COMMENT", values});
							values->push_back("/*!");
							values->push_back(" *  Auto-generated types for schema definition [source schema: ${schema_name}]");
							values->push_back(" *");
							values->push_back(" *  Reference Schema Version : ${version}");
							values->push_back(" *  Generated by : ${username}");
							values->push_back(" *  Generated on : ${date}");
							values->push_back(" */");
							// END KEY [FILE_COMMENT]

							// KEY [FUNC_TYPE_LIST_SERIALIZER]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_TYPE_LIST_SERIALIZER", values});
							values->push_back("/**");
							values->push_back(" * Serialize the list instance of value type ${type_ptr}.");
							values->push_back(" *");
							values->push_back(" * @param __value - Vector instance of ${type_ptr}.");
							values->push_back(" * @return - Vector of serialized data maps.");
							values->push_back(" */");
							values->push_back("std::vector<__struct_datatype__ *> *serialize_list_${name}(std::vector<${type_ptr}> *__value) {");
							values->push_back("    CHECK_NOT_EMPTY_P(__value);");
							values->push_back("    std::vector<__struct_datatype__ *> *__list = new std::vector<__struct_datatype__ *>();");
							values->push_back("    CHECK_ALLOC(__list, TYPE_NAME(vector));");
							values->push_back("    for (${type_ptr} rec : *(__value)) {");
							values->push_back("        CHECK_NOT_NULL(rec);");
							values->push_back("        __struct_datatype__ *rv = this->serialize_${type}(rec);");
							values->push_back("        CHECK_NOT_NULL(rv);");
							values->push_back("        __list->push_back(rv);");
							values->push_back("    }");
							values->push_back("    return __list;");
							values->push_back("}");
							// END KEY [FUNC_TYPE_LIST_SERIALIZER]

							// KEY [CALL_NATIVE_SETTER_FROM_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_NATIVE_SETTER_FROM_MAP", values});
							values->push_back("// Set the value of ${name}");
							values->push_back("${type_ptr} __var = static_cast<${type_ptr}>(__ptr);");
							values->push_back("CHECK_CAST(__var, TYPE_NAME(void *), TYPE_NAME(${type}));");
							values->push_back("this->${name} = __var;");
							// END KEY [CALL_NATIVE_SETTER_FROM_MAP]

							// KEY [FUNC_FREE_DATA_PTR]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_FREE_DATA_PTR", values});
							values->push_back("/**");
							values->push_back(" * Free an instance of the serialized data map.");
							values->push_back(" *");
							values->push_back(" * @param __data - Pointer to the instance of a serialized data map.");
							values->push_back(" * @param allocated - Are the value pointers locally allocated and should be freed?");
							values->push_back(" */");
							values->push_back("void free_data_ptr(__struct_datatype__ *__data, bool allocated) override {");
							values->push_back("	__struct_datatype__::iterator iter;");
							values->push_back("	${free_data_calls}");
							values->push_back("	__data->clear();");
							values->push_back("	CHECK_AND_FREE(__data);");
							values->push_back("}");
							// END KEY [FUNC_FREE_DATA_PTR]

							// KEY [FUNC_TYPE_MAP_DESERIALIZER]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_TYPE_MAP_DESERIALIZER", values});
							values->push_back("/**");
							values->push_back(" * Deserialize a map instance of key/value type ${key_type}/${value_type_ptr} from the passed input data.");
							values->push_back(" *");
							values->push_back(" * @param __input - Void pointer of type unordered_map<${key_type}, __struct_datatype__ *>");
							values->push_back(" * @return - Deserialized map of key/value type unordered_map<${key_type}, ${value_type_ptr}>");
							values->push_back(" */");
							values->push_back("std::unordered_map<${key_type}, ${value_type_ptr}> *deserialize_map_${name}(void *__input) {");
							values->push_back("	CHECK_NOT_NULL(__input);");
							values->push_back("	std::unordered_map<${key_type}, __struct_datatype__ *> *__value = static_cast<std::unordered_map<${key_type}, __struct_datatype__ *> *>( __input);");
							values->push_back("	CHECK_CAST(__value, TYPE_NAME(void *), TYPE_NAME(unordered_map));");
							values->push_back("	");
							values->push_back("	std::unordered_map<${key_type}, ${value_type_ptr}> *__map = new std::unordered_map<${key_type}, ${value_type_ptr}>();");
							values->push_back("	CHECK_ALLOC(__map, TYPE_NAME(unordered_map));");
							values->push_back("");
							values->push_back("	std::unordered_map<${key_type}, __struct_datatype__ *>::iterator iter;");
							values->push_back("	for (iter = __value->begin(); iter != __value->end(); iter++) {");
							values->push_back("		${value_type_ptr} __tv = deserialize_${value_type}(iter->second);");
							values->push_back("		CHECK_NOT_NULL(__tv);");
							values->push_back("		__map->insert({iter->first, __tv});");
							values->push_back("	}");
							values->push_back("	return __map;");
							values->push_back("}");
							// END KEY [FUNC_TYPE_MAP_DESERIALIZER]

							// KEY [CALL_FREE_TYPE_DATA_PTR]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_FREE_TYPE_DATA_PTR", values});
							values->push_back("iter = __data->find(\"${name}\");");
							values->push_back("// Free the ${name} value if locally allocated");
							values->push_back("if (iter != __data->end()) {");
							values->push_back("	if (NOT_NULL(iter->second)) {");
							values->push_back("		${type_ptr} __ptr = static_cast<${type_ptr}>(iter->second);");
							values->push_back("		if (allocated) {");
							values->push_back("			CHECK_AND_FREE(__ptr);");
							values->push_back("		}");
							values->push_back("	}");
							values->push_back("}");
							// END KEY [CALL_FREE_TYPE_DATA_PTR]

							// KEY [COPY_CALL_TYPE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"COPY_CALL_TYPE", values});
							values->push_back("// Set ${name} from the source value.");
							values->push_back("if (NOT_NULL(source.${name})) {");
							values->push_back("	this->${name} = new ${type}(*(source.${name}));");
							values->push_back("	CHECK_ALLOC(this->${name}, TYPE_NAME(${type}));");
							values->push_back("}");
							// END KEY [COPY_CALL_TYPE]

							// KEY [FUNC_MAP_TYPE_ADD_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_MAP_TYPE_ADD_DEF", values});
							values->push_back("/**");
							values->push_back(" * Add a key/value pair to the map with a type value.");
							values->push_back(" * The value is expected to be a pre-allocated ${type} pointer.");
							values->push_back(" *");
							values->push_back(" * Method should only be used when this instance is being");
							values->push_back(" * used to create or update a new record instance.");
							values->push_back(" *");
							values->push_back(" * @param m_key - ${key_type} key value.");
							values->push_back(" * @param m_value - ${value_type_ptr} value pointer.");
							values->push_back(" */ ");
							values->push_back("void add_to_${name}(${key_type} m_key, ${value_type_ptr} m_value) {");
							values->push_back("    PRECONDITION(this->__is_allocated == true);");
							values->push_back("    if (IS_NULL(this->${name})) {");
							values->push_back("        this->${name} = new std::unordered_map<${key_type}, ${value_type_ptr}>();");
							values->push_back("        CHECK_ALLOC(this->${name}, TYPE_NAME(unordered_map));");
							values->push_back("    }");
							values->push_back("    this->${name}->insert({m_key, m_value});");
							values->push_back("}");
							// END KEY [FUNC_MAP_TYPE_ADD_DEF]

							// KEY [CALL_FREE_NATIVE_LIST_DATA_PTR]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_FREE_NATIVE_LIST_DATA_PTR", values});
							values->push_back("// Free the list pointer and the list values, if locally allocated.");
							values->push_back("iter = __data->find(\"${name}\");");
							values->push_back("if (iter != __data->end()) {");
							values->push_back("	if (NOT_NULL(iter->second)) {");
							values->push_back("		void *__ptr = iter->second;");
							values->push_back("		std::vector<${type_ptr}> *__v_ptr = static_cast<std::vector<${type_ptr}> *>(__ptr);");
							values->push_back("		CHECK_CAST(__v_ptr, TYPE_NAME(void), TYPE_NAME(vector));");
							values->push_back("		if (allocated) {");
							values->push_back("			FREE_NATIVE_LIST(__v_ptr);");
							values->push_back("		} else {");
							values->push_back("			CHECK_AND_FREE(__v_ptr);");
							values->push_back("		}");
							values->push_back("	}");
							values->push_back("}");
							// END KEY [CALL_FREE_NATIVE_LIST_DATA_PTR]

							// KEY [CALL_FREE_TYPE_LIST_DATA_PTR]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_FREE_TYPE_LIST_DATA_PTR", values});
							values->push_back("// Free the list pointer and the list values, if locally allocated.");
							values->push_back("iter = __data->find(\"${name}\");");
							values->push_back("if (iter != __data->end()) {");
							values->push_back("	if (NOT_NULL(iter->second)) {");
							values->push_back("		void *__ptr = iter->second;");
							values->push_back("		std::vector<${type_ptr}> *__v_ptr = static_cast<std::vector<${type_ptr}> *>(__ptr);");
							values->push_back("		CHECK_CAST(__v_ptr, TYPE_NAME(void), TYPE_NAME(vector));");
							values->push_back("		if (allocated) {");
							values->push_back("			FREE_TYPE_LIST(__v_ptr);");
							values->push_back("		} else {");
							values->push_back("			CHECK_AND_FREE(__v_ptr);");
							values->push_back("		}");
							values->push_back("	}");
							values->push_back("}");
							// END KEY [CALL_FREE_TYPE_LIST_DATA_PTR]

							// KEY [CALL_SETTER_TO_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_SETTER_TO_MAP", values});
							values->push_back("// Call the method to add the value of ${name} to the serialized data map.");
							values->push_back("this->set_map_${name}(__data);");
							// END KEY [CALL_SETTER_TO_MAP]

							// KEY [FUNC_STRING_SETTER_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_STRING_SETTER_DEF", values});
							values->push_back("/**");
							values->push_back(" * Set the char buffer value from the specified string.");
							values->push_back(" *");
							values->push_back(" * Method should only be used when this instance is being");
							values->push_back(" * used to create or update a new record instance.");
							values->push_back(" *");
							values->push_back(" * @param ${name} - String value to set.");
							values->push_back(" */");
							values->push_back("void set_${name}(string &${name}) {");
							values->push_back("    	FREE_PTR(this->${name});");
							values->push_back("    	if (!IS_EMPTY(${name})) {");
							values->push_back("        	uint32_t __size = ${name}.length() + 1;");
							values->push_back("        	this->${name} = (char *) malloc(sizeof(char) * __size);");
							values->push_back("        	CHECK_ALLOC(this->${name}, TYPE_NAME(char *));");
							values->push_back("        	memset(this->${name}, 0, __size);");
							values->push_back("        	memcpy(this->${name}, ${name}.c_str(), (__size - 1));");
							values->push_back("    	}");
							values->push_back("}");
							// END KEY [FUNC_STRING_SETTER_DEF]

							// KEY [CALL_FREE_NATIVE_MAP_DATA_PTR]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_FREE_NATIVE_MAP_DATA_PTR", values});
							values->push_back("// Free the map pointer and the list values, if locally allocated.");
							values->push_back("iter = __data->find(\"${name}\");");
							values->push_back("if (iter != __data->end()) {");
							values->push_back("	if (NOT_NULL(iter->second)) {");
							values->push_back("		void *__ptr = iter->second;");
							values->push_back("		std::unordered_map<${key_type}, ${value_type_ptr}> *__m_ptr = static_cast<std::unordered_map<${key_type}, ${value_type_ptr}> *>(__ptr);");
							values->push_back("		CHECK_CAST(__m_ptr, TYPE_NAME(void), TYPE_NAME(unordered_map));");
							values->push_back("		if (allocated) {");
							values->push_back("			FREE_NATIVE_MAP(__m_ptr);");
							values->push_back("		} else {");
							values->push_back("			CHECK_AND_FREE(__m_ptr);");
							values->push_back("		}");
							values->push_back("	}");
							values->push_back("}");
							// END KEY [CALL_FREE_NATIVE_MAP_DATA_PTR]

							// KEY [CALL_NATIVE_SETTER_TO_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_NATIVE_SETTER_TO_MAP", values});
							values->push_back("// Add the value of ${name} to the data map.");
							values->push_back("__data->insert({\"${name}\", this->${name}});");
							// END KEY [CALL_NATIVE_SETTER_TO_MAP]

							// KEY [CALL_TYPE_SETTER_TO_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_TYPE_SETTER_TO_MAP", values});
							values->push_back("// Serialize the value of ${name} and add to the data map.");
							values->push_back("void *__ptr = serialize_${type}(this->${name});");
							values->push_back("CHECK_NOT_NULL(__ptr);");
							values->push_back("__data->insert({\"${name}\", __ptr});");
							// END KEY [CALL_TYPE_SETTER_TO_MAP]

							// KEY [FUNC_TYPE_MAP_SERIALIZER]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_TYPE_MAP_SERIALIZER", values});
							values->push_back("/**");
							values->push_back(" * Serialize a map instance of key/value type ${key_type}/${value_type_ptr} from the passed input data.");
							values->push_back(" *");
							values->push_back(" * @param __i_map - Input map data.");
							values->push_back(" * @return - Serialized data map of key/value type unordered_map<${key_type}, __struct_datatype__ *>");
							values->push_back(" */");
							values->push_back("std::unordered_map<${key_type}, __struct_datatype__ *> *serialize_map_${name}(std::unordered_map<${key_type}, ${value_type_ptr}> *__i_map) {");
							values->push_back("    CHECK_NOT_EMPTY_P(__i_map);");
							values->push_back("    std::unordered_map<${key_type}, __struct_datatype__ *> *__map = new std::unordered_map<${key_type}, __struct_datatype__ *>();");
							values->push_back("    CHECK_ALLOC(__map, TYPE_NAME(unordered_map));");
							values->push_back("");
							values->push_back("    std::unordered_map<${key_type}, ${value_type_ptr}>::iterator iter;");
							values->push_back("    for(iter = __i_map->begin(); iter != __i_map->end(); iter++) {");
							values->push_back("        const ${key_type} key = iter->first;");
							values->push_back("        ${value_type_ptr} value = iter->second;");
							values->push_back("        CHECK_NOT_NULL(value);");
							values->push_back("        __struct_datatype__ *rv = this->serialize_${value_type}(value);");
							values->push_back("        CHECK_NOT_NULL(rv);");
							values->push_back("        __map->insert({key, rv});");
							values->push_back("    }");
							values->push_back("    return __map;");
							values->push_back("}");
							// END KEY [FUNC_TYPE_MAP_SERIALIZER]

							// KEY [FUNC_LIST_STRING_ADD_DEF]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_LIST_STRING_ADD_DEF", values});
							values->push_back("/**");
							values->push_back(" * Add a string element to the list ${name}. ");
							values->push_back(" * Elements are expected to be pre-assigned data");
							values->push_back(" * value.");
							values->push_back(" *");
							values->push_back(" * Method should only be used when this instance is being");
							values->push_back(" * used to create or update a new record instance.");
							values->push_back(" *");
							values->push_back(" * @param ${name} - String value to add to list.");
							values->push_back(" */");
							values->push_back("void add_to_${name}(string &${name}) {");
							values->push_back("    PRECONDITION(this->__is_allocated == true);");
							values->push_back("    if (IS_NULL(this->${name})) {");
							values->push_back("        this->${name} = new std::vector<${type_ptr}>();");
							values->push_back("        CHECK_ALLOC(this->${name}, TYPE_NAME(vector));");
							values->push_back("    }");
							values->push_back("    if (!IS_EMPTY(${name})) {");
							values->push_back("        uint32_t __size = ${name}.length() + 1;");
							values->push_back("        char *__var = (char *)malloc(sizeof(char) * __size);");
							values->push_back("        CHECK_ALLOC(__var, TYPE_NAME(char *));");
							values->push_back("        memset(__var, 0, __size);");
							values->push_back("        memcpy(__var, ${name}.c_str(), (__size - 1));");
							values->push_back("        this->${name}->push_back(__var);");
							values->push_back("    }");
							values->push_back("}");
							// END KEY [FUNC_LIST_STRING_ADD_DEF]

							// KEY [CALL_TYPE_LIST_SETTER_TO_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_TYPE_LIST_SETTER_TO_MAP", values});
							values->push_back("// Serialize the list ${name} and add to the data map.");
							values->push_back("void *__ptr = serialize_list_${m_name}(this->${name});");
							values->push_back("CHECK_NOT_NULL(__ptr);");
							values->push_back("__data->insert({\"${name}\", __ptr});");
							// END KEY [CALL_TYPE_LIST_SETTER_TO_MAP]

							// KEY [CALL_DESERIALIZE_VALUE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_DESERIALIZE_VALUE", values});
							values->push_back("    // Set the value of ${name} from the serialized data map.");
							values->push_back("    this->set_value_${name}(__data);");
							// END KEY [CALL_DESERIALIZE_VALUE]

							// KEY [CALL_TYPE_SETTER_FROM_MAP]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"CALL_TYPE_SETTER_FROM_MAP", values});
							values->push_back("// Deserialize and set the value of ${name}");
							values->push_back("${type_ptr} __var = deserialize_${type}(__ptr);");
							values->push_back("CHECK_NOT_NULL(__var);");
							values->push_back("this->${name} = __var;");
							// END KEY [CALL_TYPE_SETTER_FROM_MAP]

							// KEY [FUNC_SERIALIZE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_SERIALIZE", values});
							values->push_back("/**");
							values->push_back(" * Serialize this type instance into a data map instance.");
							values->push_back(" *");
							values->push_back(" * @return - Serialized data map instance.");
							values->push_back(" */");
							values->push_back("__struct_datatype__ *serialize() override {");
							values->push_back("    __struct_datatype__ *__data = new __struct_datatype__();");
							values->push_back("    CHECK_ALLOC(__data, TYPE_NAME(__struct_datatype__));");
							values->push_back("    ${set_map_calls}");
							values->push_back("    return __data;");
							values->push_back("}");
							// END KEY [FUNC_SERIALIZE]

							// KEY [FUNC_TYPE_SERIALIZER]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_TYPE_SERIALIZER", values});
							values->push_back("/**");
							values->push_back(" * Serialize the type value for ${name} and add to the data map.");
							values->push_back(" *");
							values->push_back(" * @param __value - Pointer to the field of type ${name}.");
							values->push_back(" * @return - Serialized data map instance.");
							values->push_back(" */");
							values->push_back("__struct_datatype__ *serialize_${name}(${type_ptr} __value) {");
							values->push_back("    CHECK_NOT_NULL(__value);");
							values->push_back("    __struct_datatype__ *__data = __value->serialize();");
							values->push_back("    CHECK_NOT_NULL(__data);");
							values->push_back("    return __data;");
							values->push_back("}");
							// END KEY [FUNC_TYPE_SERIALIZER]

							// KEY [FUNC_TYPE_LIST_DESERIALIZER]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_TYPE_LIST_DESERIALIZER", values});
							values->push_back("/**");
							values->push_back(" * Deserialize a list instance of type ${type_ptr} from the passed input data.");
							values->push_back(" *");
							values->push_back(" * @param __input - Void pointer to input data of type vector<__struct_datatype__ *>");
							values->push_back(" * @return - Deserialized pointer to list of type vector<${type_ptr} *>");
							values->push_back(" */");
							values->push_back("std::vector<${type_ptr}> *deserialize_list_${name}(void *__input) {");
							values->push_back("	CHECK_NOT_NULL(__input);");
							values->push_back("	std::vector<__struct_datatype__ *> *__value = static_cast<std::vector<__struct_datatype__ *> *>( __input);");
							values->push_back("	CHECK_CAST(__value, TYPE_NAME(void *), TYPE_NAME(vector));");
							values->push_back("");
							values->push_back("	std::vector<${type_ptr}> *__list = new std::vector<${type_ptr}>();");
							values->push_back("	CHECK_ALLOC(__list, TYPE_NAME(vector));");
							values->push_back("	for( __struct_datatype__ *v : *__value) {");
							values->push_back("		CHECK_NOT_NULL(v);");
							values->push_back("		${type_ptr} __tv = deserialize_${type}(v);");
							values->push_back("		CHECK_NOT_NULL(__tv);");
							values->push_back("		__list->push_back(__tv);");
							values->push_back("	}");
							values->push_back("	return __list;");
							values->push_back("}");
							// END KEY [FUNC_TYPE_LIST_DESERIALIZER]

							// KEY [FUNC_CONSTRUCTOR_WRITABLE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_CONSTRUCTOR_WRITABLE", values});
							values->push_back("/**");
							values->push_back(" * Empty constructor when creating an instance of ${name} for setting data locally.");
							values->push_back(" * Should be used to create a new record instance of type ${name}.");
							values->push_back(" */");
							values->push_back("${name}() {");
							values->push_back("    this->__is_allocated = true;");
							values->push_back("    ${variable_inits}");
							values->push_back("}");
							// END KEY [FUNC_CONSTRUCTOR_WRITABLE]

							// KEY [FUNC_CONSTRUCTOR_READABLE]
							values = new std::vector<string>();
							CHECK_ALLOC(values, TYPE_NAME(vector));
							__cpp_template.insert({"FUNC_CONSTRUCTOR_READABLE", values});
							values->push_back("/**");
							values->push_back(" * Create a new read-only instance of ${name} and populate the data from the passed ");
							values->push_back(" * serialized data map.");
							values->push_back(" *");
							values->push_back(" * @param __data - Serialized data map to load the object data from.");
							values->push_back(" */");
							values->push_back("${name}(__struct_datatype__ *__data) {");
							values->push_back("    CHECK_NOT_NULL(__data);");
							values->push_back("    this->__is_allocated = false;");
							values->push_back("    ${variable_inits}");
							values->push_back("    this->deserialize(__data);");
							values->push_back("}");
							// END KEY [FUNC_CONSTRUCTOR_READABLE]

						}

					public:
						cpp_template_header() {
							this->__init();
						}

						~cpp_template_header() {
							unordered_map<string, vector<string> *>::iterator iter;
							for (iter = __cpp_template.begin(); iter != __cpp_template.end(); iter++) {
								CHECK_AND_FREE(iter->second);							}
						}

						std::vector<string> * find_token(string token) {
							unordered_map<string, vector<string> *>::iterator iter = __cpp_template.find(token);
							if (iter != __cpp_template.end())
								return iter->second;
							return nullptr;
						}

					};
		}
REACTFS_NS_CORE_END
#define CPPT_TOKEN_FILE_DEF "FILE_DEF"
#define CPPT_TOKEN_CLASS_DEF "CLASS_DEF"
#define CPPT_TOKEN_INCLUDE_BASE "INCLUDE_BASE"
#define CPPT_TOKEN_COPY_CALL_STRING "COPY_CALL_STRING"
#define CPPT_TOKEN_COPY_CALL_NATIVE "COPY_CALL_NATIVE"
#define CPPT_TOKEN_FUNC_CONSTRUCTOR_COPY "FUNC_CONSTRUCTOR_COPY"
#define CPPT_TOKEN_CALL_TYPE_LIST_SETTER_FROM_MAP "CALL_TYPE_LIST_SETTER_FROM_MAP"
#define CPPT_TOKEN_COPY_CALL_TYPE_LIST "COPY_CALL_TYPE_LIST"
#define CPPT_TOKEN_COPY_CALL_NATIVE_LIST "COPY_CALL_NATIVE_LIST"
#define CPPT_TOKEN_CALL_FREE_TYPE_MAP_DATA_PTR "CALL_FREE_TYPE_MAP_DATA_PTR"
#define CPPT_TOKEN_FUNC_SETTER_FROM_MAP "FUNC_SETTER_FROM_MAP"
#define CPPT_TOKEN_FUNC_MAP_STRING_ADD_DEF "FUNC_MAP_STRING_ADD_DEF"
#define CPPT_TOKEN_FUNC_DESTRUCTOR "FUNC_DESTRUCTOR"
#define CPPT_TOKEN_FUNC_MAP_NATIVE_ADD_DEF "FUNC_MAP_NATIVE_ADD_DEF"
#define CPPT_TOKEN_CALL_TYPE_MAP_SETTER_TO_MAP "CALL_TYPE_MAP_SETTER_TO_MAP"
#define CPPT_TOKEN_FUNC_LIST_TYPE_ADD_DEF "FUNC_LIST_TYPE_ADD_DEF"
#define CPPT_TOKEN_NAMESPACE "NAMESPACE"
#define CPPT_TOKEN_VARIABLE_MAP_TYPE_FREE "VARIABLE_MAP_TYPE_FREE"
#define CPPT_TOKEN_FUNC_LIST_NATIVE_ADD_DEF "FUNC_LIST_NATIVE_ADD_DEF"
#define CPPT_TOKEN_VARIABLE_NATIVE_FREE "VARIABLE_NATIVE_FREE"
#define CPPT_TOKEN_FUNC_NATIVE_SETTER_DEF "FUNC_NATIVE_SETTER_DEF"
#define CPPT_TOKEN_VARIABLE_TYPE_FREE "VARIABLE_TYPE_FREE"
#define CPPT_TOKEN_CALL_FREE_NATIVE_DATA_PTR "CALL_FREE_NATIVE_DATA_PTR"
#define CPPT_TOKEN_FUNC_DESERIALIZE "FUNC_DESERIALIZE"
#define CPPT_TOKEN_VARIABLE_LIST_NATIVE_FREE "VARIABLE_LIST_NATIVE_FREE"
#define CPPT_TOKEN_COPY_CALL_NATIVE_MAP "COPY_CALL_NATIVE_MAP"
#define CPPT_TOKEN_VARIABLE_LIST_TYPE_FREE "VARIABLE_LIST_TYPE_FREE"
#define CPPT_TOKEN_VARIABLE_NATIVE_DEF "VARIABLE_NATIVE_DEF"
#define CPPT_TOKEN_CALL_TYPE_MAP_SETTER_FROM_MAP "CALL_TYPE_MAP_SETTER_FROM_MAP"
#define CPPT_TOKEN_VARIABLE_MAP_NATIVE_FREE "VARIABLE_MAP_NATIVE_FREE"
#define CPPT_TOKEN_COPY_CALL_TYPE_MAP "COPY_CALL_TYPE_MAP"
#define CPPT_TOKEN_FUNC_TYPE_DESERIALIZER "FUNC_TYPE_DESERIALIZER"
#define CPPT_TOKEN_FUNC_GETTER_DEF "FUNC_GETTER_DEF"
#define CPPT_TOKEN_FUNC_SETTER_PTR_DEF "FUNC_SETTER_PTR_DEF"
#define CPPT_TOKEN_FUNC_SETTER_TO_MAP "FUNC_SETTER_TO_MAP"
#define CPPT_TOKEN_FILE_COMMENT "FILE_COMMENT"
#define CPPT_TOKEN_FUNC_TYPE_LIST_SERIALIZER "FUNC_TYPE_LIST_SERIALIZER"
#define CPPT_TOKEN_CALL_NATIVE_SETTER_FROM_MAP "CALL_NATIVE_SETTER_FROM_MAP"
#define CPPT_TOKEN_FUNC_FREE_DATA_PTR "FUNC_FREE_DATA_PTR"
#define CPPT_TOKEN_FUNC_TYPE_MAP_DESERIALIZER "FUNC_TYPE_MAP_DESERIALIZER"
#define CPPT_TOKEN_CALL_FREE_TYPE_DATA_PTR "CALL_FREE_TYPE_DATA_PTR"
#define CPPT_TOKEN_COPY_CALL_TYPE "COPY_CALL_TYPE"
#define CPPT_TOKEN_FUNC_MAP_TYPE_ADD_DEF "FUNC_MAP_TYPE_ADD_DEF"
#define CPPT_TOKEN_CALL_FREE_NATIVE_LIST_DATA_PTR "CALL_FREE_NATIVE_LIST_DATA_PTR"
#define CPPT_TOKEN_CALL_FREE_TYPE_LIST_DATA_PTR "CALL_FREE_TYPE_LIST_DATA_PTR"
#define CPPT_TOKEN_CALL_SETTER_TO_MAP "CALL_SETTER_TO_MAP"
#define CPPT_TOKEN_FUNC_STRING_SETTER_DEF "FUNC_STRING_SETTER_DEF"
#define CPPT_TOKEN_CALL_FREE_NATIVE_MAP_DATA_PTR "CALL_FREE_NATIVE_MAP_DATA_PTR"
#define CPPT_TOKEN_CALL_NATIVE_SETTER_TO_MAP "CALL_NATIVE_SETTER_TO_MAP"
#define CPPT_TOKEN_CALL_TYPE_SETTER_TO_MAP "CALL_TYPE_SETTER_TO_MAP"
#define CPPT_TOKEN_FUNC_TYPE_MAP_SERIALIZER "FUNC_TYPE_MAP_SERIALIZER"
#define CPPT_TOKEN_FUNC_LIST_STRING_ADD_DEF "FUNC_LIST_STRING_ADD_DEF"
#define CPPT_TOKEN_CALL_TYPE_LIST_SETTER_TO_MAP "CALL_TYPE_LIST_SETTER_TO_MAP"
#define CPPT_TOKEN_CALL_DESERIALIZE_VALUE "CALL_DESERIALIZE_VALUE"
#define CPPT_TOKEN_CALL_TYPE_SETTER_FROM_MAP "CALL_TYPE_SETTER_FROM_MAP"
#define CPPT_TOKEN_FUNC_SERIALIZE "FUNC_SERIALIZE"
#define CPPT_TOKEN_FUNC_TYPE_SERIALIZER "FUNC_TYPE_SERIALIZER"
#define CPPT_TOKEN_FUNC_TYPE_LIST_DESERIALIZER "FUNC_TYPE_LIST_DESERIALIZER"
#define CPPT_TOKEN_FUNC_CONSTRUCTOR_WRITABLE "FUNC_CONSTRUCTOR_WRITABLE"
#define CPPT_TOKEN_FUNC_CONSTRUCTOR_READABLE "FUNC_CONSTRUCTOR_READABLE"
#define CPPT_TOKEN_DEF_SCHEMA_NAME "${schema_name}"
#define CPPT_TOKEN_DEF_VARIABLE_FREES "${variable_frees}"
#define CPPT_TOKEN_DEF_VARIABLE_INITS "${variable_inits}"
#define CPPT_TOKEN_DEF_GUARD_NAME "${guard_name}"
#define CPPT_TOKEN_DEF_DATE "${date}"
#define CPPT_TOKEN_DEF_VALUE_TYPE_PTR "${value_type_ptr}"
#define CPPT_TOKEN_DEF_VALUE_TYPE "${value_type}"
#define CPPT_TOKEN_DEF_SET_MAP_CALLS "${set_map_calls}"
#define CPPT_TOKEN_DEF_TYPE "${type}"
#define CPPT_TOKEN_DEF_USERNAME "${username}"
#define CPPT_TOKEN_DEF_TYPE_PTR "${type_ptr}"
#define CPPT_TOKEN_DEF_BODY "${body}"
#define CPPT_TOKEN_DEF_INCLUDES "${includes}"
#define CPPT_TOKEN_DEF_RETURN "${return}"
#define CPPT_TOKEN_DEF_HEADER "${header}"
#define CPPT_TOKEN_DEF_KEY_TYPE "${key_type}"
#define CPPT_TOKEN_DEF_DATA_VAR "${data_var}"
#define CPPT_TOKEN_DEF_DESTRUCTOR "${destructor}"
#define CPPT_TOKEN_DEF_NESTED "${nested}"
#define CPPT_TOKEN_DEF_PUBLIC_FUNCTIONS "${public_functions}"
#define CPPT_TOKEN_DEF_VERSION "${version}"
#define CPPT_TOKEN_DEF_M_NAME "${m_name}"
#define CPPT_TOKEN_DEF_PRIVATE_FUNCTIONS "${private_functions}"
#define CPPT_TOKEN_DEF_READ_MAP_CALLS "${read_map_calls}"
#define CPPT_TOKEN_DEF_PARENT "${parent}"
#define CPPT_TOKEN_DEF_NAME "${name}"
#define CPPT_TOKEN_DEF_DECLARATIONS "${declarations}"
#define CPPT_TOKEN_DEF_VARIABLE_COPY "${variable_copy}"
#define CPPT_TOKEN_DEF_FREE_DATA_CALLS "${free_data_calls}"
#define CPPT_TOKEN_DEF_CONSTRUCTOR "${constructor}"
#endif // REACTFS_CPP_TEMPLATE_HEADER_H
