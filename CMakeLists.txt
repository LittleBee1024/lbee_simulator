cmake_minimum_required(VERSION 3.16)

# Setting a project
project(WebAssembly_C++_Template VERSION 1.0
        DESCRIPTION "WebAssembly with C++ library Project Template")

# set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# "emcmake cmake .." can set the correct compiler
# set(CMAKE_C_COMPILER "emcc")
# set(CMAKE_CXX_COMPILER "em++")

# Output Directory
set(OUTUPT_DIR ${CMAKE_BINARY_DIR}/html)
file(MAKE_DIRECTORY ${OUTUPT_DIR})
MESSAGE(STATUS "OUTUPT_DIR = ${OUTUPT_DIR}")

add_subdirectory(ui)
add_subdirectory(wasm)
