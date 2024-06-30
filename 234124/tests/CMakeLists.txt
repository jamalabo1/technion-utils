cmake_minimum_required(VERSION 3.16)
project(technion-234124)

set(CMAKE_CXX_STANDARD 17)

if (ENABLE_UNIT_TESTS)
    include(cmake/utils.cmake)
    include(cmake/FindGTest.cmake)

    message("Unit tests enabled")

    create_global_testing_target(technion-234124_tests main.cpp) # create a global target for sub-tests to link their test library to

    add_subdirectory(hw1/unit-tests)

    enable_global_testing_target() # enable this target by linking the global target to an executable (order is important, for the global test target to populate it's self)

    install_project_binaries(technion-234124_tests_exc)
endif ()