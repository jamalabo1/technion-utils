cmake_minimum_required(VERSION 3.10)
project(test_runner CXX)

include(cmake-modules/json.cmake)

add_library(runner main.cpp
        include/runner.h
        modules/ex3.h
        modules/ex4.h
        modules/ex3.cpp
        modules/ex4.cpp
)

set_source_files_properties(modules/ex3.cpp
        APPEND
        PROPERTIES COMPILE_DEFINITIONS "ASM_START_METHOD=ASM_START_METHOD_ex3")

#set_source_files_properties(modules/ex3.cpp PROPERTIES COMPILE_DEFINITIONS "ASM_START_METHOD=ASM_START_METHOD_ex3")
set_source_files_properties(modules/ex4.cpp PROPERTIES COMPILE_DEFINITIONS "ASM_START_METHOD=ASM_START_METHOD_ex4")


target_link_libraries(runner PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(runner PUBLIC include)