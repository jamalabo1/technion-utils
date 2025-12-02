//
// Created by jamal on 02/12/2025.
//
#include <catch2/catch.hpp>

TEST_CASE("AVL Test cases") {
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
