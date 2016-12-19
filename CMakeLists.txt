cmake_minimum_required(VERSION 3.1)
project(ReactFS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

add_subdirectory(common)

add_subdirectory(watergate)

add_subdirectory(types)
add_subdirectory(parsers)

add_subdirectory(core)

