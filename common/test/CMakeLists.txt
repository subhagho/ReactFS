cmake_minimum_required(VERSION 3.1)

enable_testing()

INCLUDE(../../makefiles/CMakeTestInclude.txt)

set(PROJECT_CMAKE_COMMON ${PROJECT_ROOT_DIR}/makefiles/CMakeCommon.txt)
MESSAGE("Including common makefile : " ${PROJECT_CMAKE_COMMON})

INCLUDE(${PROJECT_CMAKE_COMMON})

find_package(Threads)

# Build Catch unit tests
add_library(catch INTERFACE)
target_include_directories(catch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

LINK_DIRECTORIES(${EXT_LINK_DIRS} ${RFS_BUILD_COMMON})

MESSAGE("Using libraries : [" ${RFS_COMMON_LIBS} "]")

add_executable(test_config test_config.cpp)
target_link_libraries(test_config ${CMAKE_THREAD_LIBS_INIT} ${RFS_COMMON_LIBS})
add_test(NAME test_config COMMAND test_config)

add_executable(test_alarm test_alarm.cpp)
target_link_libraries(test_alarm ${CMAKE_THREAD_LIBS_INIT} ${RFS_COMMON_LIBS})
add_test(NAME test_alarm COMMAND test_alarm)

add_executable(test_file_utils test_file_utils.cpp)
target_link_libraries(test_file_utils ${CMAKE_THREAD_LIBS_INIT} ${RFS_COMMON_LIBS})
add_test(NAME test_file_utils COMMAND test_file_utils)

add_executable(test_shared_lock test_shared_lock.cpp)
target_link_libraries(test_shared_lock ${CMAKE_THREAD_LIBS_INIT} ${RFS_COMMON_LIBS})
add_test(NAME test_shared_lock COMMAND test_shared_lock)

add_executable(test_thread_pool test_thread_pool.cpp)
target_link_libraries(test_thread_pool ${CMAKE_THREAD_LIBS_INIT} ${RFS_COMMON_LIBS})
add_test(NAME test_thread_pool COMMAND test_thread_pool)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")
