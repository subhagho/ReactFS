cmake_minimum_required(VERSION 3.1)

enable_testing()

INCLUDE(../../makefiles/CMakeTestInclude.txt)

find_package(Threads)

LINK_DIRECTORIES(${EXT_LINK_DIRS} ${RFS_BUILD_COMMON} ${RFS_BUILD_WGATE})
MESSAGE("Using libraries : [" ${RFS_BUILD_COMMON} "]")


add_executable(test_sem_lock test_sem_lock.cpp)
target_link_libraries(test_sem_lock watergate ${CMAKE_THREAD_LIBS_INIT} ${RFS_WGATE_LIBS})
add_test(NAME test_sem_lock COMMAND test_sem_lock)

add_executable(test_lock_client test_lock_client.cpp)
target_link_libraries(test_lock_client watergate ${CMAKE_THREAD_LIBS_INIT} ${RFS_WGATE_LIBS})

add_executable(test_lock_table test_lock_table.cpp)
target_link_libraries(test_lock_table watergate ${CMAKE_THREAD_LIBS_INIT} ${RFS_WGATE_LIBS})
add_test(NAME test_lock_table COMMAND test_lock_table)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")
