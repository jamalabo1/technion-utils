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

        target_include_directories(${test_target} PUBLIC ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/include)

        # to enable students to keep their add_exec pipeline and not require lib + exec split, a decoy library is introduced and is created on the fly:
        # this only copies srcs & include dirs, if more customization is required, switch to lib/exec pattern instead and link to lib

        target_link_libraries(${DS_HW}_tests PRIVATE ${student_target})

        enable_testing()

        add_test(NAME ${DS_HW}_public COMMAND ${test_target})

    else ()
        message(WARNING "${DS_HW} tests directory was not found!")
    endif ()
endfunction()
