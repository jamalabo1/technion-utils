cmake_minimum_required(VERSION 3.16)
project(technion-234124)

set(CMAKE_CXX_STANDARD 17)


if (ENABLE_UNIT_TESTS)
    include(cmake/utils.cmake)
    include(cmake/FindGTest.cmake)

    add_subdirectory(project)


    message(STATUS "Unit tests enabled")
    set(TESTING_TARGET_LIBRARY mtm_blockchain_lib)

    set(GLOBAL_TESTING_TARGET tests)
    message(STATUS "GLOBAL TARGET: ${GLOBAL_TESTING_TARGET}, TESTING_TARGET_LIBRARY: ${TESTING_TARGET_LIBRARY}")

    create_global_testing_target(${GLOBAL_TESTING_TARGET} main.cpp) # create a global target for sub-tests to link their test library to

    # start mtm_blockchain_lib
    add_library(${TESTING_TARGET_LIBRARY}_tests
            blockchain_tests.cpp
    )

    create_testing_target(${TESTING_TARGET_LIBRARY})
    #end mtm_blockchain_lib

    create_testing_executable(${GLOBAL_TESTING_TARGET} ${GLOBAL_TESTING_TARGET}_exc)
#    enable_global_testing_target() # enable this target by linking the global target to an executable (order is important, for the global test target to populate it's self)

    install(TARGETS ${GLOBAL_TESTING_TARGET}_exec DESTINATION bin COMPONENT ${GLOBAL_TESTING_TARGET}_exec)
endif ()