cmake_minimum_required(VERSION 3.10)
project(asm_with_cpp CXX)  # C++ only, asm will be handled manually


include(cmake-modules/load_asm.cmake)

set(BUILD_DIR ${CMAKE_BINARY_DIR}/build)
file(MAKE_DIRECTORY ${BUILD_DIR})

# === Assemble .asm files manually ===
set(ASM_SOURCES ex3.asm ex4.asm)
set(ASM_OBJECTS "")
set(ASM_START_METHOD "ASM_START_METHOD")

add_compile_definitions(ASM_START_METHOD=${ASM_START_METHOD})

foreach(ASM_FILE ${ASM_SOURCES})
    load_asm(ASM_FILE)
endforeach()

# === Add custom target to assemble .asm files ===
add_custom_target(assemble_asm ALL DEPENDS ${ASM_OBJECTS})

# === Add the C++ executable ===
add_executable(program main.cpp ${ASM_OBJECTS})

# Make sure linking waits for .asm object files to exist
add_dependencies(program assemble_asm)

add_subdirectory(runner)

target_link_libraries(program PRIVATE runner)