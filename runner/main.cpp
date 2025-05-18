#include <iostream>

#include "modules/ex3.h"
#include "modules/ex4.h"

int RunAllTests() {
    TestCase testCase{};

    // MOD 3
    Q3_MODULE::prepare_data(testCase);

    Q3_MODULE::FUNC();

    Q3_MODULE::verify_results();

    // MODULE 4 (EX4)
    Q4_MODULE::prepare_data(testCase);

    Q4_MODULE::FUNC();

    Q4_MODULE::verify_results();
}