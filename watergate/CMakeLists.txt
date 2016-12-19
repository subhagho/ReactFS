cmake_minimum_required(VERSION 3.1)


project(watergate VERSION 0.1.0)

INCLUDE(../makefiles/CMakeInclude.txt)
set(PROJECT_CMAKE_COMMON ${PROJECT_ROOT_DIR}/makefiles/CMakeCommon.txt)
MESSAGE("Including common makefile : " ${PROJECT_CMAKE_COMMON})

INCLUDE(${PROJECT_CMAKE_COMMON})

set(WATERGATE_LIB_NAME ${PROJECT_NAME})

file(GLOB_RECURSE WG_SOURCES "src/*.cpp")
file(GLOB_RECURSE WG_INCLUDES "src/*.h")
file(GLOB_RECURSE WG_EXT_INCLUDES "includes/*.h")

list(REMOVE_DUPLICATES WG_SOURCES)

MESSAGE("SOURCE FILES : [" ${WG_SOURCES} "]")

LINK_DIRECTORIES(${EXT_LINK_DIRS} ${RFS_BUILD_COMMON} ${RFS_BUILD_WGATE})

add_library(${WATERGATE_LIB_NAME} SHARED ${WG_INCLUDES} ${WG_SOURCES})

MESSAGE("Using libraries : [" ${RFS_WGATE_LIBS} "]")

target_link_libraries(${WATERGATE_LIB_NAME} ${RFS_COMMON_LIBS})

add_executable(main_basic adhoc/main_basic.cpp)
target_link_libraries(main_basic ${RFS_WGATE_LIBS})

add_executable(main_sems adhoc/main_sems.cpp)
target_link_libraries(main_sems ${RFS_WGATE_LIBS})

add_executable(main_lock_table adhoc/main_lock_table.cpp)
target_link_libraries(main_lock_table ${RFS_WGATE_LIBS})

find_package(Threads)

LINK_DIRECTORIES(${EXT_LINK_DIRS} ${RFS_BUILD_COMMON})
MESSAGE("Using libraries : [" ${RFS_BUILD_COMMON} "]")


add_executable(test_sem_lock test/test_sem_lock.cpp)
target_link_libraries(test_sem_lock watergate ${CMAKE_THREAD_LIBS_INIT} ${RFS_WGATE_LIBS})
add_test(NAME test_sem_lock COMMAND test_sem_lock)

add_executable(test_lock_client test/test_lock_client.cpp)
target_link_libraries(test_lock_client watergate ${CMAKE_THREAD_LIBS_INIT} ${RFS_WGATE_LIBS})

add_executable(test_lock_table test/test_lock_table.cpp)
target_link_libraries(test_lock_table watergate ${CMAKE_THREAD_LIBS_INIT} ${RFS_WGATE_LIBS})
add_test(NAME test_lock_table COMMAND test_lock_table)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")
