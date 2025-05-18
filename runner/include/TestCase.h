//
// Created by jamal on 18/05/2025.
//

#ifndef TEST_CASE_H
#define TEST_CASE_H

#include "nlohmann/json.hpp"

class TestCase {
public:
    int id;
    nlohmann::json data;
};

#endif //TEST_CASE_H
