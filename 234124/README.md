# 234124

offers a GitHub action with workflow to build and test the first assignment.

## Usage 
1. add the supplied files to ur repository, such that `.github` & `cmake` & `tests` is at the root level
2. create a cmake executable target `mtm_blockchain`
3. _optional_: for unit tests create a cmake library target `mtm_blockchain_lib`
4. go to `GitHub Actions` section and `Test Runner` then Click on `Run Workflow` and choose your options accordingly.


#### Example:
CMakeLists.txt:
```
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
```



_**that's it!**_

## Results

go to the `actions` tab in ur github repo, and check the results of the runners.
you can also check the binaries built by the runner, and test `.out` results

### Note
new features are planed to be published, keep an eye :).