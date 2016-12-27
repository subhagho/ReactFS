/*!
 *  Auto-generated types for schema definition [source schema: test_schema]
 *
 *  Reference Schema Version : 0001.0000
 *  Generated by : subho
 *  Generated on : 2016-12-27 13:12:55.849
 */


#ifndef TEST_SCHEMA_H
#define TEST_SCHEMA_H
#include "types/includes/type_defs.h"
#include "types/includes/__base_type.h"

#include "test_type.h"
#include "test_ref_type.h"


using namespace REACTFS_NS_COMMON_PREFIX;
using namespace REACTFS_NS_CORE_PREFIX::types;



namespace com {
    namespace wookler {
    namespace test {
    class test_schema  : public com::wookler::reactfs::types::__base_type {
private:
    char * key = nullptr;

double * dkey = nullptr;

short * sval = nullptr;

test_ref_type * __map = nullptr;

vector<test_type *> * __ref = nullptr;



    void set_value_key(__struct_datatype__ *__data) {
    if (NOT_NULL(__data)) {
        __struct_datatype__::const_iterator iter = __data->find("key");
        if (iter != __data->end()) {
            void *__ptr = iter->second;
            if (NOT_NULL(__ptr)) {
		char * __var = static_cast<char *>(__ptr);
CHECK_CAST(__var, TYPE_NAME(void *), TYPE_NAME(char));
this->key = __var;

            } else {
                this->key = nullptr;
            }
        } else {
            this->key = nullptr;
        }
    } else {
        this->key = nullptr;
    }
}

void set_map_key(__struct_datatype__ *__data) {
    CHECK_NOT_NULL(__data);
    if (NOT_NULL(this->key)) {
	__data->insert({"key", this->key});
	
    }
}

void set_value_dkey(__struct_datatype__ *__data) {
    if (NOT_NULL(__data)) {
        __struct_datatype__::const_iterator iter = __data->find("dkey");
        if (iter != __data->end()) {
            void *__ptr = iter->second;
            if (NOT_NULL(__ptr)) {
		double * __var = static_cast<double *>(__ptr);
CHECK_CAST(__var, TYPE_NAME(void *), TYPE_NAME(double));
this->dkey = __var;

            } else {
                this->dkey = nullptr;
            }
        } else {
            this->dkey = nullptr;
        }
    } else {
        this->dkey = nullptr;
    }
}

void set_map_dkey(__struct_datatype__ *__data) {
    CHECK_NOT_NULL(__data);
    if (NOT_NULL(this->dkey)) {
	__data->insert({"dkey", this->dkey});
	
    }
}

void set_value_sval(__struct_datatype__ *__data) {
    if (NOT_NULL(__data)) {
        __struct_datatype__::const_iterator iter = __data->find("sval");
        if (iter != __data->end()) {
            void *__ptr = iter->second;
            if (NOT_NULL(__ptr)) {
		short * __var = static_cast<short *>(__ptr);
CHECK_CAST(__var, TYPE_NAME(void *), TYPE_NAME(short));
this->sval = __var;

            } else {
                this->sval = nullptr;
            }
        } else {
            this->sval = nullptr;
        }
    } else {
        this->sval = nullptr;
    }
}

void set_map_sval(__struct_datatype__ *__data) {
    CHECK_NOT_NULL(__data);
    if (NOT_NULL(this->sval)) {
	__data->insert({"sval", this->sval});
	
    }
}

void *serialize_test_ref_type(test_ref_type * __value) {
    CHECK_NOT_NULL(__value);
    __struct_datatype__ *__data = __value->serialize();
    CHECK_NOT_NULL(__data);
    return __data;
}

test_ref_type * deserialize_test_ref_type(void *__input) {
    CHECK_NOT_NULL(__input);
    __struct_datatype__ *__value = (__struct_datatype__ *)__input;
    CHECK_CAST(__value, TYPE_NAME(void *), TYPE_NAME(__struct_datatype__));
    test_ref_type * __data = new test_ref_type(false);
    CHECK_ALLOC(__data, TYPE_NAME(test_ref_type));
    __data->deserialize(__value);
    return __data;
}

void set_value___map(__struct_datatype__ *__data) {
    if (NOT_NULL(__data)) {
        __struct_datatype__::const_iterator iter = __data->find("__map");
        if (iter != __data->end()) {
            void *__ptr = iter->second;
            if (NOT_NULL(__ptr)) {
		test_ref_type * __var = deserialze_test_ref_type(__ptr);
CHECK_NOT_NULL(__var);
this->__map = __var;

            } else {
                this->__map = nullptr;
            }
        } else {
            this->__map = nullptr;
        }
    } else {
        this->__map = nullptr;
    }
}

void set_map___map(__struct_datatype__ *__data) {
    CHECK_NOT_NULL(__data);
    if (NOT_NULL(this->__map)) {
	void *__ptr = serialize_test_ref_type(this->__map);
CHECK_NOT_NULL(__ptr);
__data->insert({"__map", __ptr});
	
    }
}

void *serialize_test_type(test_type * __value) {
    CHECK_NOT_NULL(__value);
    __struct_datatype__ *__data = __value->serialize();
    CHECK_NOT_NULL(__data);
    return __data;
}

test_type * deserialize_test_type(void *__input) {
    CHECK_NOT_NULL(__input);
    __struct_datatype__ *__value = (__struct_datatype__ *)__input;
    CHECK_CAST(__value, TYPE_NAME(void *), TYPE_NAME(__struct_datatype__));
    test_type * __data = new test_type(false);
    CHECK_ALLOC(__data, TYPE_NAME(test_type));
    __data->deserialize(__value);
    return __data;
}

std::vector<__struct_datatype__ *> *serialize_list_vector_test_type___(std::vector<test_type *> *__value) {
    CHECK_NOT_EMPTY_P(__value);
    std::vector<__struct_datatype__ *> *__list = new std::vector<__struct_datatype__ *>();
    CHECK_ALLOC(__list, TYPE_NAME(vector));
    for (test_type * rec : *(__value)) {
        CHECK_NOT_NULL(rec);
        __struct_datatype__ *rv = this->serialize_test_type(rec);
        CHECK_NOT_NULL(rv);
        __list->push_back(rv);
    }
    return __list;
}

std::vector<test_type *> *deserialize_list_vector_test_type___(void *__input) {
	CHECK_NOT_NULL(__input);
	std::vector<__struct_datatype__ *> *__value = (std::vector<__struct_datatype__ *> *) __input;
	CHECK_CAST(__value, TYPE_NAME(void *), TYPE_NAME(vector));

	std::vector<test_type *> *__list = new std::vector<test_type *>();
	CHECK_ALLOC(__list, TYPE_NAME(vector));
	for( __struct_datatype__ *v : *__value) {
		CHECK_NOT_NULL(v);
		test_type * __tv = deserialize_test_type(v);
		CHECK_NOT_NULL(__tv);
		__list->push_back(__tv);
	}
	return __list;
}

void set_value___ref(__struct_datatype__ *__data) {
    if (NOT_NULL(__data)) {
        __struct_datatype__::const_iterator iter = __data->find("__ref");
        if (iter != __data->end()) {
            void *__ptr = iter->second;
            if (NOT_NULL(__ptr)) {
		std::vector<test_type *> *__var = deserialize_list_vector_test_type___(__ptr);
CHECK_NOT_NULL(__var);
this->__ref = __var;

            } else {
                this->__ref = nullptr;
            }
        } else {
            this->__ref = nullptr;
        }
    } else {
        this->__ref = nullptr;
    }
}

void set_map___ref(__struct_datatype__ *__data) {
    CHECK_NOT_NULL(__data);
    if (NOT_NULL(this->__ref)) {
	void *__ptr = serialize_list_vector_test_type___(this->__ref);
CHECK_NOT_NULL(__ptr);
__data->insert({"__ref", __ptr});
	
    }
}


public:
    ${constructor}

    ${destructor}

    void set_key(char * key) {
    this->key = key;
}

char * get_key(void) {
    return this->key;
}

void set_key(string &key) {
    PRECONDITION(this->__is_allocated == true);
    FREE_PTR(this->key);
    if (!IS_EMPTY(key)) {
        uint32_t __size = name.length() + 1;
        this->key = (char) malloc(sizeof(char) * __size);
        CHECK_ALLOC(this->key, TYPE_NAME(char *));
        memset(this->key, 0, __size);
        memcpy(this->key, key.c_str(), (__size - 1));
    }
}

void set_dkey(double * dkey) {
    this->dkey = dkey;
}

double * get_dkey(void) {
    return this->dkey;
}

void set_dkey(double &dkey) {
    PRECONDITION(this->__is_allocated == true);
    FREE_PTR(this->dkey);
    this->dkey = (double *)malloc(sizeof(double));
    CHECK_ALLOC(this->dkey, TYPE_NAME(double));
    *(this->dkey) = dkey;
}

void set_sval(short * sval) {
    this->sval = sval;
}

short * get_sval(void) {
    return this->sval;
}

void set_sval(short &sval) {
    PRECONDITION(this->__is_allocated == true);
    FREE_PTR(this->sval);
    this->sval = (short *)malloc(sizeof(short));
    CHECK_ALLOC(this->sval, TYPE_NAME(short));
    *(this->sval) = sval;
}

test_ref_type * get___map(void) {
    return this->__map;
}

void set___map(test_ref_type * __map) {
    this->__map = __map;
}

vector<test_type *> * get___ref(void) {
    return this->__ref;
}

void set___ref(vector<test_type *> * __ref) {
    this->__ref = __ref;
}

void add_to___ref(test_type * __ref) {
    PRECONDITION(this->__is_allocated == true);
    CHECK_NOT_NULL(__ref);
    if (IS_NULL(this->__ref)) {
        this->__ref = new std::vector<test_type *>();
        CHECK_ALLOC(this->__ref, TYPE_NAME(vector));
    }
    this->__ref->push_back(__ref);
}

void deserialize(__struct_datatype__ *__data) {
    CHECK_NOT_NULL(__data);
        this->set_value_key(__data);

    this->set_value_dkey(__data);

    this->set_value_sval(__data);

    this->set_value___map(__data);

    this->set_value___ref(__data);


}

__struct_datatype__ *serialize() override {
    __struct_datatype__ *__data = new __struct_datatype__();
    CHECK_ALLOC(__data, TYPE_NAME(__struct_datatype__));
    this->set_map_key(__data);

this->set_map_dkey(__data);

this->set_map_sval(__data);

this->set_map___map(__data);

this->set_map___ref(__data);


    return __data;
}


};

}

}

}

#endif // TEST_SCHEMA_H

