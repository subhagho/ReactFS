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
    test() {
        cout << "Size = " << __size << "\n";
    }
    int get_size() {
        return size;
    }
};

typedef test<24> test_24;

#define BIT_TYPE_CONSTRAINT 1
#define DEFAULT_VALUE_BIT 2

typedef enum {
    TEST = 0, TEST1, TEST2
} test_enum;

int main(int argc, char *argv[]) {
    test_enum t1 = test_enum::TEST;
    test_enum t2 = test_enum::TEST1;
    test_enum t3 = test_enum::TEST2;

    cout << t1 << "; " << t2 << "; " << t3 << "\n";
    cout << "BOOL SIZE : " << sizeof(bool) << "\n";
    if (2 == t3) {
        cout << "Worked...\n";
    }
}