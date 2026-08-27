cmake_minimum_required(VERSION 3.16)
project(lab2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(googletest)
enable_testing()

add_library(project_core STATIC
        srcs/complex.cpp)
target_include_directories(project_core INTERFACE headers)

add_executable(run_tests tests/test_complex.cpp
    tests/main.cpp tests/test_fragment.cpp)

target_include_directories(run_tests PRIVATE     tests)

target_link_libraries(run_tests PRIVATE project_core gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(run_tests)