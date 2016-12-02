//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>

#include "common/includes/common_utils.h"
#include "common/includes/__threads.h"
#include "common/includes/base_error.h"

using namespace std;
using namespace com::wookler::reactfs::common;


int main(int argc, char *argv[]) {
    uint32_t ii = 5;
    for(int jj=0; jj <= 5; jj++) {
        ii--;
        cout << "Current value " << ii << "\n";
    }
}