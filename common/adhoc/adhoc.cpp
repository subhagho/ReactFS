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
    cout << "TYPE : " << TYPE_NAME(__callback) << "\n";
    cout << "TYPE : " << TYPE_NAME(base_error) << "\n";
}