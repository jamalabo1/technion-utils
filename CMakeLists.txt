cmake_minimum_required(VERSION 3.16)
project(p2340218 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

add_library(
        p2340218_lib

        TechSystem26a1.h
        TechSystem26a1.cpp
)


target_include_directories(p2340218_lib PUBLIC .)

add_executable(
        techsystem26a1
        main26a1.cpp
)

target_link_libraries(techsystem26a1 PRIVATE p2340218_lib)

# add install component
install(TARGETS techsystem26a1
        DESTINATION bin
        COMPONENT techsystem26a1
)

IF (NOT DISABLE_DS_TESTING)
    message("Using ds tester from remote")
    include(FetchContent)
    FetchContent_Declare(ds_tester
            GIT_REPOSITORY https://github.com/jamalabo1/technion-utils.git
            GIT_TAG 234218-v1-latest
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(ds_tester)
    ds_tester_attach(p2340218_lib HW hw1)
endif ()