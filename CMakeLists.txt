CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
set(CYNTATIC_VERSION 0.1.0 CACHE STRING "The version of cyntatic sugar")
project(Cyntatic
        VERSION ${CYNTATIC_VERSION}
        DESCRIPTION "Cyntatic is a transpiler for a high level programming language that tries to simplify C"
        LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
if (NOT CMAKE_BUILD_TYPE)
    # default build type is Debug
    set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Configure path for loading project cmake scripts
set(CMAKE_MODULE_PATH
        ${CMAKE_MODULE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(ENABLE_UNIT_TESTS    "Enable building of unit tests" ON)

include_directories(include)

set(CYNTATIC_SOURCES
        src/ast.cpp
        src/parser.cpp
        src/tokenizer.cpp)

add_executable(cyntatic
        src/main.cpp
        ${CYNTATIC_SOURCES})
target_compile_definitions(cyntatic PUBLIC cynt_ut=)

if (ENABLE_UNIT_TESTS)
    include(Catch.cmake)
    add_executable(cyntatic-test
            tests/main.cpp
            ${CYNTATIC_SOURCES})
    target_compile_definitions(cyntatic-test
        PUBLIC cynt_ut=:public SYNTATIC_UNITTEST)
endif()