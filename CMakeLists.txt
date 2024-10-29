cmake_minimum_required(VERSION 3.21)

project(abcg)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Common.cmake)

add_subdirectory(abcg)
add_subdirectory(examples)
