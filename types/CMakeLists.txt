cmake_minimum_required(VERSION 3.1)

project(reactfs-types VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE(../makefiles/CMakeInclude.txt /usr/local/opt/flex/include)

LINK_DIRECTORIES(/usr/lib ../common/cmake ../watergate/cmake)

set(TYPES_LIB_NAME ${PROJECT_NAME})

file(GLOB_RECURSE RFS_C_SOURCES "src/*.cpp")
file(GLOB_RECURSE RFS_C_INCLUDES "includes/*.h")

list(REMOVE_DUPLICATES RFS_C_SOURCES)

MESSAGE("SOURCE FILES : [" ${RFS_C_SOURCES} "]")

if (${APPLE})
    MESSAGE("Host platform: MAC OS X")
    set(RFS_C_LIBRARIES "-lreactfs-common")
elseif (${UNIX})
    MESSAGE("Host platform:  Other UNIX")
    set(RFS_C_LIBRARIES "-lreactfs-common -lpthread -luuid -lrt")
endif ()

MESSAGE("Using libraries : #" ${RFS_C_LIBRARIES} "#")

add_library(${TYPES_LIB_NAME} SHARED ${RFS_C_INCLUDES} ${RFS_C_SOURCES})
target_link_libraries(${TYPES_LIB_NAME} ${RFS_C_LIBRARIES})
