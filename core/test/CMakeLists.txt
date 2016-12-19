cmake_minimum_required(VERSION 3.1)

enable_testing()

INCLUDE(../../makefiles/CMakeTestInclude.txt)

set(PROJECT_CMAKE_COMMON ${PROJECT_ROOT_DIR}/makefiles/CMakeCommon.txt)
MESSAGE("Including common makefile : " ${PROJECT_CMAKE_COMMON})

INCLUDE(${PROJECT_CMAKE_COMMON})

find_package(Threads)

LINK_DIRECTORIES(${EXT_LINK_DIRS} ${RFS_BUILD_COMMON} ${RFS_BUILD_TYPES} ${RFS_BUILD_PARSER} ${RFS_BUILD_CORE} ${RFS_BUILD_WGATE})
MESSAGE("Using libraries : #" ${RFS_CORE_LIBS} "#")

add_executable(test_node_client test_node_client.cpp)
target_link_libraries(test_node_client ${CMAKE_THREAD_LIBS_INIT} ${RFS_CORE_LIBS})
add_test(NAME test_node_client COMMAND test_node_client)

add_executable(test_node_manager test_node_manager.cpp)
target_link_libraries(test_node_manager ${CMAKE_THREAD_LIBS_INIT} ${RFS_CORE_LIBS})
add_test(NAME test_node_manager COMMAND test_node_manager)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")
