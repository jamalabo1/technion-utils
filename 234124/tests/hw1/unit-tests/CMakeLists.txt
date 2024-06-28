cmake_minimum_required(VERSION 3.16)
project(hw1-unit-tests)

set(CMAKE_CXX_STANDARD 17)

add_library(mtm_blockchain_lib_tests
        blockchain_tests.cpp
)

# assuming mtm_blockchain target exists.

create_testing_target(mtm_blockchain_lib)