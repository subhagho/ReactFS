//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>
#include <malloc.h>

#include "common/includes/common_utils.h"
#include "common/includes/__threads.h"
#include "common/includes/base_error.h"

using namespace std;
using namespace com::wookler::reactfs::common;

typedef struct __field_value__ {
    int type;
    void *data = nullptr;
} __field_value;


int main(int argc, char *argv[]) {
    int max = 10;
    int s = sizeof(__field_value);
    __field_value **r = (__field_value **) malloc(sizeof(__field_value *) * max);
    CHECK_ALLOC(r, TYPE_NAME(__field_value));
    __field_value **ptr = r;
    for(int ii=0; ii < max; ii++) {
        if (ii % 2 == 0) {
            __field_value *nptr = (__field_value *) malloc (sizeof(__field_value));
            CHECK_ALLOC(nptr, TYPE_NAME(__field_value));
            *ptr = nptr;
            (*ptr)->type = ii;
            (*ptr)->data = (int *) malloc(sizeof(int));
            memcpy((*ptr)->data, &ii, sizeof(int));
        } else {
            *ptr = nullptr;
        }
        ptr++;
    }
    for(int ii=0; ii < max; ii++) {
        ptr = r + ii;
        if (NOT_NULL(ptr)) {
            int *v = static_cast<int *>((*ptr)->data);
            cout << (*ptr)->type << ":" << *v << "\n";
            FREE_PTR((*ptr)->data);
            FREE_PTR(*ptr);
        }
    }

}