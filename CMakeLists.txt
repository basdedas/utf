cmake_minimum_required(VERSION 3.22)
project(utf)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library with the implementation, shared by the demo executable and the tests.
add_library(utf_lib STATIC
        src/utf.cpp
        inc/utf.h
        exc/NotFoundException)
target_include_directories(utf_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(utf main.cpp)
target_link_libraries(utf PRIVATE utf_lib)

# Tests (turn off with: cmake -S . -B build -DUTF_BUILD_TESTS=OFF)
option(UTF_BUILD_TESTS "Build the unit tests" ON)
if (UTF_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(utf_tests tests/utf_test.cpp)
    target_link_libraries(utf_tests PRIVATE utf_lib GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(utf_tests)
endif ()