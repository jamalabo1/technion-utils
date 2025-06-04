cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)

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
        234124
)

add_executable(
        mtm_blockchain

        main.cpp
)

target_link_libraries(mtm_blockchain mtm_blockchain_lib)


install(TARGETS mtm_blockchain DESTINATION bin COMPONENT mtm_blockchain)