//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>

#include "common/includes/common_utils.h"
#include "common/includes/__threads.h"
#include "common/includes/mm/memory_manager.h"

using namespace std;
using namespace com::wookler::reactfs::common;

int main(int argc, char *argv[]) {
    for (uint32_t ii = 1000; ii < 2000; ii++) {
        uint32_t p = common_utils::find_prime(ii);
        if (p != 0) {
            cout << "Found prime number : " << p << "\n";
        }
    }
}