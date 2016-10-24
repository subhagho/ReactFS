cmake_minimum_required(VERSION 3.1)


project(rfs-common VERSION 0.1.0)

if (${APPLE})
    set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk)
    INCLUDE_DIRECTORIES(/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers/)
endif ()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE_DIRECTORIES(../ext/ src/ includes/)

set(COMMON_LIB_NAME ${PROJECT_NAME})

file(GLOB_RECURSE RFS_C_SOURCES "src/*.cpp")
file(GLOB_RECURSE RFS_C_INCLUDES "includes/*.h")
file(GLOB_RECURSE RFS_C_EXT_INCLUDES "src/includes/*.h")

list(REMOVE_DUPLICATES RFS_C_SOURCES)
list(REMOVE_ITEM RFS_C_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/test/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/test/main_sems.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/test/main_fs.cpp)

MESSAGE("SOURCE FILES : [" ${RFS_C_SOURCES} "]")

add_library(${COMMON_LIB_NAME} SHARED ${RFS_C_INCLUDES} ${RFS_C_SOURCES})

if (${APPLE})
    MESSAGE("Host platform: MAC OS X")
elseif (${UNIX})
    MESSAGE("Host platform:  Other UNIX")
    set(RFS_C_LIBRARIES "pthread" "uuid" "rt")
endif ()


MESSAGE("Using libraries : #" ${RFS_C_LIBRARIES} "#")

