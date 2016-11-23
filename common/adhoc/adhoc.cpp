//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>

#include "common/includes/common_utils.h"
#include "common/includes/__threads.h"

using namespace std;
using namespace com::wookler::reactfs::common;

int main(int argc, char *argv[]) {
    uint32_t ii = 0;

    for (uint32_t jj = 0; jj < 10; jj++) {
        uint32_t k = ii++;
        cout << "[" << ii << "] value = " << k << "\n";
    }
}