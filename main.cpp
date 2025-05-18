#include <iostream>
#include "runner.h"

extern "C" {
    void test_func();
}

int main() {
    RunAllTests();

    std::cout << "functions ran\n" << std::endl;
}
