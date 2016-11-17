//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>

int main(int argc, char *argv[]) {
    uint32_t data = 0;
    for (int ii = 0; ii < 32; ii++) {
        data |= (1 << ii);
        std::cout << "Index=" << ii << " : " << data << "\n";
    }
}