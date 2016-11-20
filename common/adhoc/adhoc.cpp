//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>

#include "common/includes/common_utils.h"


using namespace std;
using namespace com::wookler::reactfs::common;

int main(int argc, char *argv[]) {
    string s = "This is a new line";
    if (string_utils::starts_with(&s, "This")) {
        cout << "Find [This] succeeded...\n";
    }

    if (!string_utils::starts_with(&s, "is")) {
        cout << "Find starts_with [is] failed...\n";
    }

    if (!string_utils::ends_with(&s, "is")) {
        cout << "Find ends_with [is] failed...\n";
    }

    if (!string_utils::ends_with(&s, "line")) {
        cout << "Find ends_with [line] succeeded...\n";
    }
}