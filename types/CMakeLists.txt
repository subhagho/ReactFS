cmake_minimum_required(VERSION 3.1)

project(reactfs-types VERSION 0.1.0)

INCLUDE(../makefiles/CMakeInclude.txt)
set(PROJECT_CMAKE_COMMON ${PROJECT_ROOT_DIR}/makefiles/CMakeCommon.txt)
MESSAGE("Including common makefile : " ${PROJECT_CMAKE_COMMON})

INCLUDE(${PROJECT_CMAKE_COMMON})

LINK_DIRECTORIES(${EXT_LINK_DIRS} ${RFS_BUILD_COMMON} ${RFS_BUILD_TYPES})

set(TYPES_LIB_NAME ${PROJECT_NAME})

file(GLOB_RECURSE RFS_C_SOURCES "src/*.cpp")
file(GLOB_RECURSE RFS_C_INCLUDES "includes/*.h")

list(REMOVE_DUPLICATES RFS_C_SOURCES)

MESSAGE("SOURCE FILES : [" ${RFS_C_SOURCES} "]")

MESSAGE("Using libraries : #" ${RFS_COMMON_LIBS} "#")

add_library(${TYPES_LIB_NAME} SHARED ${RFS_C_INCLUDES} ${RFS_C_SOURCES})
target_link_libraries(${TYPES_LIB_NAME} ${RFS_COMMON_LIBS})

add_executable(main_types_test adhoc/main_types_test.cpp)
target_link_libraries(main_types_test ${TYPES_LIB_NAME} ${RFS_COMMON_LIBS})