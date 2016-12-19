cmake_minimum_required(VERSION 3.1)


project(reactfs-common VERSION 0.1.0)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE(../makefiles/CMakeInclude.txt)

set(COMMON_LIB_NAME ${PROJECT_NAME})

file(GLOB_RECURSE RFS_C_SOURCES "src/*.cpp")
file(GLOB_RECURSE RFS_C_INCLUDES "includes/*.h")

list(REMOVE_DUPLICATES RFS_C_SOURCES)
LINK_DIRECTORIES(${EXT_LINK_DIRS} ${RFS_BUILD_COMMON})

MESSAGE("SOURCE FILES : [" ${RFS_C_SOURCES} "]")

add_library(${COMMON_LIB_NAME} SHARED ${RFS_C_INCLUDES} ${RFS_C_SOURCES})
MESSAGE("Using libraries : [" ${RFS_SYS_LIBRARIES} "]")
target_link_libraries(${COMMON_LIB_NAME} ${RFS_SYS_LIBRARIES})

add_executable(main_common adhoc/main_common.cpp)
target_link_libraries(main_common ${RFS_COMMON_LIBS})

add_executable(main_shared_lock adhoc/main_shared_lock.cpp)
target_link_libraries(main_shared_lock ${RFS_COMMON_LIBS})

add_executable(main_thread_pool adhoc/main_thread_pool.cpp)
target_link_libraries(main_thread_pool ${RFS_COMMON_LIBS})

add_executable(adhoc adhoc/adhoc.cpp)
target_link_libraries(adhoc ${RFS_COMMON_LIBS})
