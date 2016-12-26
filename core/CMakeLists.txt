cmake_minimum_required(VERSION 3.1)

project(reactfs-core VERSION 0.1.0)

INCLUDE(../makefiles/CMakeInclude.txt)
set(PROJECT_CMAKE_COMMON ${PROJECT_ROOT_DIR}/makefiles/CMakeCommon.txt)
MESSAGE("Including common makefile : " ${PROJECT_CMAKE_COMMON})

INCLUDE(${PROJECT_CMAKE_COMMON})

INCLUDE_DIRECTORIES(${PROJECT_ROOT_DIR}/parsers/includes/ ${PROJECT_ROOT_DIR}/parsers/src/)


set(CORE_LIB_NAME ${PROJECT_NAME})

file(GLOB_RECURSE RFS_C_SOURCES "src/*.cpp")
file(GLOB_RECURSE RFS_C_INCLUDES "includes/*.h")

list(REMOVE_DUPLICATES RFS_C_SOURCES)

MESSAGE("SOURCE FILES : [" ${RFS_C_SOURCES} "]")

set(RFS_CORE_DEP_LIBS ${RFS_PARSER_LIBS} ${RFS_WGATE_LIB})

LINK_DIRECTORIES(${EXT_LINK_DIRS} ${RFS_BUILD_COMMON} ${RFS_BUILD_TYPES} ${RFS_BUILD_PARSER} ${RFS_BUILD_CORE} ${RFS_BUILD_WGATE})
MESSAGE("Using libraries : [" ${RFS_CORE_DEP_LIBS} "]")

add_library(${CORE_LIB_NAME} SHARED ${RFS_C_INCLUDES} ${RFS_C_SOURCES})
target_link_libraries(${CORE_LIB_NAME} ${RFS_CORE_DEP_LIBS})

add_executable(main_fsb adhoc/main_fsb.cpp)
target_link_libraries(main_fsb ${CORE_LIB_NAME} ${RFS_CORE_DEP_LIBS})

add_executable(main_compress adhoc/main_compress.cpp)
target_link_libraries(main_compress ${CORE_LIB_NAME} ${RFS_CORE_DEP_LIBS})

add_executable(core_adhoc adhoc/core_adhoc.cpp)
target_link_libraries(core_adhoc ${CORE_LIB_NAME} ${RFS_CORE_DEP_LIBS})

add_executable(main_node_env adhoc/main_node_env.cpp)
target_link_libraries(main_node_env ${CORE_LIB_NAME} ${RFS_CORE_DEP_LIBS})

add_executable(main_node_client adhoc/main_node_client.cpp)
target_link_libraries(main_node_client ${CORE_LIB_NAME} ${RFS_CORE_DEP_LIBS})

add_executable(main_typed_b adhoc/main_typed_b.cpp)
target_link_libraries(main_typed_b ${CORE_LIB_NAME} ${RFS_CORE_DEP_LIBS})

find_package(Threads)

add_executable(test_node_client test/test_node_client.cpp)
target_link_libraries(test_node_client ${CMAKE_THREAD_LIBS_INIT} ${CORE_LIB_NAME} ${RFS_CORE_DEP_LIBS})
add_test(NAME test_node_client COMMAND test_node_client)

add_executable(test_node_manager test/test_node_manager.cpp)
target_link_libraries(test_node_manager ${CMAKE_THREAD_LIBS_INIT} ${CORE_LIB_NAME} ${RFS_CORE_DEP_LIBS})
add_test(NAME test_node_manager COMMAND test_node_manager)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")