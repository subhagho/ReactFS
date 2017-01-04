//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>

#include "common/includes/common_utils.h"
#include "common/includes/__threads.h"

using namespace std;
using namespace com::wookler::reactfs::common;

typedef struct __field_value__ {
    int type;
    void *data = nullptr;
} __field_value;

template <uint8_t __size>
struct __field_list {
    uint8_t count = __size;
    __field_value *values[__size];
};

int main(int argc, char *argv[]) {
    const uint8_t max = 10;
    struct __field_list<max> *fields = (__field_list<max> *) malloc(sizeof(__field_list<max>));
    CHECK_ALLOC(fields, TYPE_NAME(__field_list__));

    __field_value *values = (__field_value *)malloc(sizeof(__field_value) * max);
    CHECK_ALLOC(values, TYPE_NAME(__field_value__));
    memset(values, 0, (sizeof(__field_value) * max));

    for(int ii=0; ii < max; ii++) {
        fields->values[ii] = (values + ii);
    }

    for(int ii=0; ii < max; ii++) {
        if (ii % 2 == 0) {
            __field_value *nptr = fields->values[ii];
            nptr->type = ii;
            nptr->data = (int *) malloc(sizeof(int));
            memcpy(nptr->data, &ii, sizeof(int));
        } else {
            fields->values[ii]->data = nullptr;
        }
    }
    for(int ii=0; ii < max; ii++) {
        __field_value *ptr = fields->values[ii];
        if (NOT_NULL(ptr->data)) {
            int *v = static_cast<int *>(ptr->data);
            cout << ptr->type << ":" << *v << "\n";
            FREE_PTR(ptr->data);
        } else {
            cout << "Empty : " << ii << "\n";
        }
    }
    FREE_PTR(fields);
    FREE_PTR(values);
}