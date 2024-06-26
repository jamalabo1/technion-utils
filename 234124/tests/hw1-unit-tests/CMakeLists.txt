cmake_minimum_required(VERSION 3.19)
project(hw1-unit-tests)

set(CMAKE_CXX_STANDARD 17)

add_library(mtm_blockchain_tests
        blockchain_tests.cpp
)

create_testing_target(mtm_blockchain)