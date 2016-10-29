cmake_minimum_required(VERSION 3.1)


project(rfs-common VERSION 0.1.0)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE(../cmake/CMakeInclude.txt)

set(COMMON_LIB_NAME ${PROJECT_NAME})

file(GLOB_RECURSE RFS_C_SOURCES "src/*.cpp")
file(GLOB_RECURSE RFS_C_INCLUDES "includes/*.h")

list(REMOVE_DUPLICATES RFS_C_SOURCES)

MESSAGE("SOURCE FILES : [" ${RFS_C_SOURCES} "]")

add_library(${COMMON_LIB_NAME} SHARED ${RFS_C_INCLUDES} ${RFS_C_SOURCES})

if (${APPLE})
    MESSAGE("Host platform: MAC OS X")
elseif (${UNIX})
    MESSAGE("Host platform:  Other UNIX")
    set(RFS_C_LIBRARIES "pthread" "uuid" "rt")
endif ()


MESSAGE("Using libraries : #" ${RFS_C_LIBRARIES} "#")

add_executable(main_common adhoc/main_common.cpp)
target_link_libraries(main_common ${COMMON_LIB_NAME} ${RFS_C_LIBRARIES})