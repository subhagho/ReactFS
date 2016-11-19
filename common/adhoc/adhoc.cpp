//
// Created by Subhabrata Ghosh on 17/11/16.
//

#include <iostream>

typedef enum __test__ {
    ONE, TWO, THREE
} __test;
int main(int argc, char *argv[]) {
    std::cout << "ENUM SIZE=" << sizeof(__test) << "\n";
}