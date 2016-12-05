//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>

#include "common/includes/common_utils.h"
#include "common/includes/__threads.h"
#include "common/includes/base_error.h"

using namespace std;
using namespace com::wookler::reactfs::common;

void func(double **p, void *ptr) {
    void *tptr = common_utils::increment_data_ptr(ptr, sizeof(uint64_t));
    *p = (double *) tptr;
}

typedef struct __t__ {
    uint64_t a;
    double b;
    char c[8];
} __t;

int main(int argc, char *argv[]) {
    __t *t = (__t *) malloc(sizeof(__t));
    CHECK_NOT_NULL(t);
    memset(t, 0, sizeof(__t));
    t->a = 12980923;
    t->b = 1813.998f;
    memcpy(t->c, "TEST", 4);
    double *p = nullptr;
    func(&p, t);
    cout << "VALUE : " << *p << "\n";
}