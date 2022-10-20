cmake_minimum_required(VERSION 3.13)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
project(engine)

add_subdirectory(third-party)
add_subdirectory(src)