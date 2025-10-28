cmake_minimum_required(VERSION 3.16)
project(include/tester LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

file(READ "${CMAKE_CURRENT_LIST_DIR}/VERSION" TESTER_VER)

MESSAGE(STATUS "using tester ${TESTER_VER}")

include(tester.cmake)