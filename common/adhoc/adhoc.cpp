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

#define BIT_TYPE_CONSTRAINT 1
#define DEFAULT_VALUE_BIT 2

int main(int argc, char *argv[]) {
    uint32_t bit = 0;
    bit = bitset_utils::set_uint32_bit(bit, BIT_TYPE_CONSTRAINT);
    cout << "WITH CONSTRAINT BIT : " << bit << "\n";
    bit = bitset_utils::set_uint32_bit(bit, DEFAULT_VALUE_BIT);
    cout << "WITH CONSTRAINT & VALUE BIT : " << bit << "\n";

    if (bitset_utils::check_uint32_bit(bit, BIT_TYPE_CONSTRAINT)) {
        cout << "CONSTRAINT BIT SET\n";
    }

    if (bitset_utils::check_uint32_bit(bit, DEFAULT_VALUE_BIT)) {
        cout << "VALUE BIT SET\n";
    }

    if (bitset_utils::check_uint32_bit(bit, 4)) {
        cout << "BIT SET ERROR\n";
    }
}