//
// Created by jamal on 28/10/2025.
//
#define CATCH_CONFIG_MAIN // provided catch main

#include <catch2/catch.hpp>

#include "hw0.h"

TEST_CASE("Hw0 Test cases") {
    std::string test_case = "It's working!";

    Hw0Class instance(test_case);


    SECTION("your_method results correct text") {
        REQUIRE(
            instance.your_method() == test_case
        );
    }

    SECTION("failing_method is a failing method for test demo") {
        REQUIRE(
            instance.failing_method() == true
        );
    }
}
