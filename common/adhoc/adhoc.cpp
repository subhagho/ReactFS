//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>

#include "common/includes/common_utils.h"
#include "common/includes/__threads.h"
#include "common/includes/base_error.h"

using namespace std;
using namespace com::wookler::reactfs::common;

template<int __size>
class test {
    int size = __size;
public:
    int get_size() {
        return size;
    }
};

void add_field(vector<int *> *fields, uint16_t index, int *type) {
    if (index == fields->size()) {
        fields->push_back(type);
    } else if (index > fields->size()) {
        for (uint16_t ii = fields->size(); ii <= index; ii++) {
            fields->push_back(nullptr);
        }
        (*fields)[index] = type;
    } else {
        assert((*fields)[index] == nullptr);
        (*fields)[index] = type;
    }
}

int main(int argc, char *argv[]) {
    vector<int *> v;
    int jj[10];
    for (int ii = 0; ii < 10; ii++) {
        if (ii % 2 == 0) {
            jj[ii] = ii;
            add_field(&v, ii, &jj[ii]);
            cout << "Added " << ii << "\n";
        }
    }
    for (int ii = 0; ii < 10; ii++) {
        if (ii % 2 != 0) {
            jj[ii] = ii;
            add_field(&v, ii, &jj[ii]);
            cout << "Added " << ii << "\n";
        }
    }
    for (int ii = 0; ii < v.size(); ii++) {
        cout << "INDEX[" << ii << "] : " << *(v[ii]) << "\n";
    }
}