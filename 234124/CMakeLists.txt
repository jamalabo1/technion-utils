cmake_minimum_required(VERSION 3.16)
project(HW1)

set(CMAKE_CXX_STANDARD 17)

include(cmake/utils)

add_library(mtm_blockchain_lib
        Utilities.cpp
        BlockChain.cpp
        Transaction.cpp

        BlockChain.h
        Transaction.h
        Utilities.h
)

target_include_directories(
        mtm_blockchain_lib
        PUBLIC
        .
)

add_executable(
        mtm_blockchain

        main.cpp
)

target_link_libraries(mtm_blockchain mtm_blockchain_lib)


install_project_binaries(mtm_blockchain)