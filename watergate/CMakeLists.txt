cmake_minimum_required(VERSION 3.1)


project(watergate VERSION 0.1.0)

if (${APPLE})
    set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk)
    INCLUDE_DIRECTORIES(/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers/)
endif ()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE(../makefiles/CMakeInclude.txt)

LINK_DIRECTORIES(../common/cmake)

set(WATERGATE_LIB_NAME ${PROJECT_NAME})

file(GLOB_RECURSE WG_SOURCES "src/*.cpp")
file(GLOB_RECURSE WG_INCLUDES "src/*.h")
file(GLOB_RECURSE WG_EXT_INCLUDES "includes/*.h")

list(REMOVE_DUPLICATES WG_SOURCES)
list(REMOVE_ITEM WG_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/test/main.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/test/main_sems.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/test/main_fs.cpp)

MESSAGE("SOURCE FILES : [" ${WG_SOURCES} "]")

add_library(${WATERGATE_LIB_NAME} SHARED ${WG_INCLUDES} ${WG_SOURCES})

if (${APPLE})
    MESSAGE("Host platform: MAC OS X")
    set(WG_LIBRARIES reactfs-common)
elseif (${UNIX})
    MESSAGE("Host platform:  Other UNIX")
    set(WG_LIBRARIES "reactfs-common" "pthread" "uuid" "rt")
endif ()


MESSAGE("Using libraries : #" ${WG_LIBRARIES} "#")

target_link_libraries(${WATERGATE_LIB_NAME} ${WG_LIBRARIES})

add_executable(main src/test/main.cpp)
target_link_libraries(main watergate ${WG_LIBRARIES})

add_executable(main_sems src/test/main_sems.cpp)
target_link_libraries(main_sems watergate ${WG_LIBRARIES})

add_executable(main_fs src/test/main_fs.cpp)
target_link_libraries(main_fs watergate ${WG_LIBRARIES})