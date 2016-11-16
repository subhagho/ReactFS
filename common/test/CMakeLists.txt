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

LINK_DIRECTORIES(../cmake)

find_package(Threads)

# Build Catch unit tests
add_library(catch INTERFACE)
target_include_directories(catch INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


if (${APPLE})
    MESSAGE("Host platform: MAC OS X")
elseif (${UNIX})
    MESSAGE("Host platform:  Other UNIX")
    set(RFS_LIBRARIES "pthread uuid rt")
endif ()

MESSAGE("Using libraries : [" ${RFS_LIBRARIES} "]")

add_executable(test_config test_config.cpp)
target_link_libraries(test_config reactfs-common ${CMAKE_THREAD_LIBS_INIT} ${RFS_LIBRARIES})
add_test(NAME test_config COMMAND test_config)

add_executable(test_alarm test_alarm.cpp)
target_link_libraries(test_alarm reactfs-common ${CMAKE_THREAD_LIBS_INIT} ${RFS_LIBRARIES})
add_test(NAME test_alarm COMMAND test_alarm)

add_executable(test_file_utils test_file_utils.cpp)
target_link_libraries(test_file_utils reactfs-common ${CMAKE_THREAD_LIBS_INIT} ${RFS_LIBRARIES})
add_test(NAME test_file_utils COMMAND test_file_utils)

add_executable(test_shared_lock test_shared_lock.cpp)
target_link_libraries(test_shared_lock reactfs-common ${CMAKE_THREAD_LIBS_INIT} ${RFS_LIBRARIES})
add_test(NAME test_shared_lock COMMAND test_shared_lock)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")
