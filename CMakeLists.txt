cmake_minimum_required(VERSION 3.16)
project(lab CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(lab_core STATIC srcs/complex.cpp)
target_include_directories(lab_core PUBLIC headers)

enable_testing()
add_subdirectory(tests)
add_subdirectory(ui)