cmake_minimum_required(VERSION 3.19)
project(technion-234124)

set(CMAKE_CXX_STANDARD 17)

add_library(test temp_dummy.cpp)

add_custom_command(
        TARGET test POST_BUILD
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