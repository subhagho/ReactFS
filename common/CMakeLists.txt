cmake_minimum_required(VERSION 3.1)


project(reactfs-common VERSION 0.1.0)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE(../makefiles/CMakeInclude.txt)
set(PROJECT_CMAKE_COMMON ${PROJECT_ROOT_DIR}/makefiles/CMakeCommon.txt)
MESSAGE("Including common makefile : " ${PROJECT_CMAKE_COMMON})

INCLUDE(${PROJECT_CMAKE_COMMON})

set(COMMON_LIB_NAME ${PROJECT_NAME})

file(GLOB_RECURSE RFS_C_SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE RFS_C_INCLUDES "includes/*.h")

list(REMOVE_DUPLICATES RFS_C_SOURCES)
LINK_DIRECTORIES(${EXT_LINK_DIRS} ${RFS_BUILD_COMMON})

MESSAGE("SOURCE FILES : [" ${RFS_C_SOURCES} "]")

add_library(${COMMON_LIB_NAME} SHARED ${RFS_C_INCLUDES} ${RFS_C_SOURCES})
MESSAGE("Using libraries : [" ${RFS_SYS_LIBRARIES} "]")
target_link_libraries(${COMMON_LIB_NAME} ${RFS_SYS_LIBRARIES})

add_executable(main_common adhoc/main_common.cpp)
target_link_libraries(main_common ${COMMON_LIB_NAME} ${RFS_SYS_LIBRARIES})

add_executable(main_murmur adhoc/main_murmur.cpp)
target_link_libraries(main_murmur ${COMMON_LIB_NAME} ${RFS_SYS_LIBRARIES})

add_executable(main_shared_lock adhoc/main_shared_lock.cpp)
target_link_libraries(main_shared_lock ${COMMON_LIB_NAME} ${RFS_SYS_LIBRARIES})

add_executable(main_thread_pool adhoc/main_thread_pool.cpp)
target_link_libraries(main_thread_pool ${COMMON_LIB_NAME} ${RFS_SYS_LIBRARIES})

add_executable(adhoc adhoc/adhoc.cpp)
target_link_libraries(adhoc ${COMMON_LIB_NAME} ${RFS_SYS_LIBRARIES})


find_package(Threads)

# Adding boost libraries
find_package(Boost REQUIRED COMPONENTS system thread filesystem)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(reactfs-common ${Boost_LIBRARIES})

# Build Catch unit tests
add_library(catch INTERFACE)
target_include_directories(catch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test_config test/test_config.cpp)
target_link_libraries(test_config ${CMAKE_THREAD_LIBS_INIT} ${COMMON_LIB_NAME} ${RFS_SYS_LIBRARIES})
add_test(NAME test_config COMMAND test_config)

add_executable(test_alarm test/test_alarm.cpp)
target_link_libraries(test_alarm ${CMAKE_THREAD_LIBS_INIT} ${COMMON_LIB_NAME} ${RFS_SYS_LIBRARIES})
add_test(NAME test_alarm COMMAND test_alarm)

add_executable(test_file_utils test/test_file_utils.cpp)
target_link_libraries(test_file_utils ${CMAKE_THREAD_LIBS_INIT} ${COMMON_LIB_NAME} ${RFS_SYS_LIBRARIES})
add_test(NAME test_file_utils COMMAND test_file_utils)

add_executable(test_shared_lock test/test_shared_lock.cpp)
target_link_libraries(test_shared_lock ${CMAKE_THREAD_LIBS_INIT} ${COMMON_LIB_NAME} ${RFS_SYS_LIBRARIES})
add_test(NAME test_shared_lock COMMAND test_shared_lock)

add_executable(test_thread_pool test/test_thread_pool.cpp)
target_link_libraries(test_thread_pool ${CMAKE_THREAD_LIBS_INIT} ${COMMON_LIB_NAME} ${RFS_SYS_LIBRARIES})
add_test(NAME test_thread_pool COMMAND test_thread_pool)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")