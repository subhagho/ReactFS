cmake_minimum_required(VERSION 3.1)
project(ReactFS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(common)
add_subdirectory(common/test)

add_subdirectory(watergate)
add_subdirectory(watergate/test)

add_subdirectory(core)
