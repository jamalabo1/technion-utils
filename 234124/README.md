# 234124

offers a GitHub action with workflow to build and test the first assignment.

## Usage 
1. add the supplied files to ur repository, such that `.github` & `tests` is at the root level
2. create a cmake executable target `mtm_blockchain`
3. _optional_: for unit tests create a cmake library target `mtm_blockchain_lib`


#### Exmaple:
CMakeLists.txt:
```
...

include(cmake/utils)

add_library(mtm_blockchain_lib
        Blockchain.cpp
        ...
)

# headers for unit tests
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