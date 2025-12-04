# Guarded include
if (TARGET ds_tester)
    return()
endif ()

add_library(ds_tester INTERFACE)
add_library(ds_tester::ds_tester ALIAS ds_tester)

function(ds_tester_attach student_target)
    set(options)
    set(oneValueArgs HW)
    set(multiValueArgs)

    cmake_parse_arguments(DS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT TARGET ${student_target})
        message(FATAL_ERROR "Target ${student_target} not found, please refer to the docs")
    endif ()

    # Enforce common flags for fairness
    if (MSVC)
        message(FATAL_ERROR "The course does not support MSVC compiler")
    else ()
        #g++ -std=c++11 -DNDEBUG -Wall -o main.out *.cpp
        target_compile_options(${student_target} PRIVATE -Wall -DNDEBUG)
    endif ()

    message(STATUS "dir=${CMAKE_CURRENT_FUNCTION_LIST_DIR}")

    # get tests from the requested dir (released version will stip the other tests)
    set(TESTS_DIR ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/tests/${DS_HW})

    if (EXISTS ${TESTS_DIR})
        set(test_target ${DS_HW}_tests)

        file(GLOB test_SRCs CONFIGURE_DEPENDS "${TESTS_DIR}/*.cpp")

        add_executable(${test_target}
                ${test_SRCs}
                #                ${TESTS_DIR}/unit.cpp
        )
        # link student library
        target_link_libraries(${test_target} PUBLIC ${student_target})

        target_include_directories(${test_target} PUBLIC ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/include)

        # link catch2
        Include(FetchContent)

        FetchContent_Declare(
                Catch2
                GIT_REPOSITORY https://github.com/catchorg/Catch2.git
                GIT_TAG v3.8.1 # or a later release
        )

        FetchContent_MakeAvailable(Catch2)

        target_link_libraries(${test_target} PRIVATE Catch2::Catch2WithMain)

        list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

        # to enable students to keep their add_exec pipeline and not require lib + exec split, a decoy library is introduced and is created on the fly:
        # this only copies srcs & include dirs, if more customization is required, switch to lib/exec pattern instead and link to lib

#        enable_testing()

        include(CTest)
        include(Catch)

        catch_discover_tests(${test_target})


#        add_test(NAME ${DS_HW}_public COMMAND ${test_target})

    else ()
        message(WARNING "${DS_HW} tests directory was not found!")
    endif ()
endfunction()
