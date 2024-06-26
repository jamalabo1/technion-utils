cmake_minimum_required(VERSION 3.19)
project(technion-234124)

set(CMAKE_CXX_STANDARD 17)

include(cmake/utils.cmake)
include(cmake/FindGTest.cmake)

create_global_testing_target(technion-234124_tests main.cpp) # create a global target for sub-tests to link their test library to

add_subdirectory(hw1-unit-tests)

enable_global_testing_target() # enable this target by linking the global target to an executable (order is important, for the global test target to populate it's self)

# python testing
add_custom_command(
        TARGET technion-234124_tests POST_BUILD
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        COMMAND pip install click --target ${PROJECT_BINARY_DIR}/engine --upgrade
        COMMENT "installing required modules"
)

install(
        DIRECTORY
        engine
        DESTINATION bin
)

install(
        DIRECTORY
        ${PROJECT_BINARY_DIR}/engine
        DESTINATION bin
)

install(FILES
        settings.json
        DESTINATION bin
)
install(
        DIRECTORY
        hw1-tests
        DESTINATION bin
)