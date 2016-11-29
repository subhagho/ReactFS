cmake_minimum_required(VERSION 3.1)

enable_testing()

if (${APPLE})
    set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk)
    INCLUDE_DIRECTORIES(/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers/)
endif ()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
INCLUDE(../../makefiles/CMakeTestInclude.txt)

LINK_DIRECTORIES(/usr/lib ../../common/cmake ../../watergate/cmake ../../core/cmake)

find_package(Threads)

# Build Catch unit tests
add_library(catch INTERFACE)
target_include_directories(catch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


if (${APPLE})
    MESSAGE("Host platform: MAC OS X")
    set(RFS_C_LIBRARIES "-lreactfs-common -lwatergate -lreactfs-core -lz")
elseif (${UNIX})
    MESSAGE("Host platform:  Other UNIX")
    set(RFS_C_LIBRARIES "-lreactfs-common -lwatergate -lreactfs-core -lpthread -luuid -lrt -lz")
endif ()

MESSAGE("Using libraries : [" ${RFS_C_LIBRARIES} "]")

add_executable(test_node_client test_node_client.cpp)
target_link_libraries(test_node_client ${CMAKE_THREAD_LIBS_INIT} ${RFS_C_LIBRARIES})
add_test(NAME test_node_client COMMAND test_node_client)

add_executable(test_node_manager test_node_manager.cpp)
target_link_libraries(test_node_manager ${CMAKE_THREAD_LIBS_INIT} ${RFS_C_LIBRARIES})
add_test(NAME test_node_manager COMMAND test_node_manager)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")
