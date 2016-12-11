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


int main(int argc, char *argv[]) {
    char **data = (char **) malloc(100 * sizeof(char *));
    char *d2[256];

    int as = (sizeof(data) / sizeof(char *));
    cout << "DATA SIZE = " << as << "\n";
    as = (sizeof(d2) / sizeof(char *));
    cout << "D2 SIZE = " << as << "\n";
}