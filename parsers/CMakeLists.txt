cmake_minimum_required(VERSION 3.1)

project(reactfs-parsers VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR}/.. ABSOLUTE)
set(PROJECT_EXT_DIR ${PROJECT_ROOT_DIR}/ext)

MESSAGE("Project root directory : " ${PROJECT_ROOT_DIR})
MESSAGE("Project ext directory : " ${PROJECT_EXT_DIR})

set(LOCAL_INCLUDES "/usr/local/include")

INCLUDE_DIRECTORIES(${PROJECT_ROOT_DIR} ${PROJECT_EXT_DIR} ${LOCAL_INCLUDES})

if (${APPLE})
    set(CMAKE_OSX_SYSROOT /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk)
    INCLUDE_DIRECTORIES(/opt/local/include /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers/)
endif ()

INCLUDE_DIRECTORIES(${PROJECT_ROOT_DIR}/parsers/includes/ ${PROJECT_ROOT_DIR}/parsers/src/)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -DYYDEBUG=1")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -DYYDEBUG=1")

LINK_DIRECTORIES(/usr/lib ../common/cmake ../types/cmake)

set(PRASERS_LIB_NAME ${PROJECT_NAME})

FIND_PACKAGE(BISON REQUIRED)
SET(YSchemaOutput ${PROJECT_ROOT_DIR}/parsers/src/schema_parser.cpp)
IF(BISON_FOUND)
    ADD_CUSTOM_COMMAND(
            OUTPUT ${YSchemaOutput}
            COMMAND ${BISON_EXECUTABLE}
            --defines=${PROJECT_ROOT_DIR}/parsers/includes/schema_tokens.h
            --output=${YSchemaOutput}
            ${PROJECT_ROOT_DIR}/parsers/src/schema_parser.yy
            COMMENT "Generating schema_parser.cpp"
    )
ENDIF()

FIND_PACKAGE(FLEX REQUIRED)
SET(LSchemaOutput ${PROJECT_ROOT_DIR}/parsers/src/schema_lexer.cpp)
IF(FLEX_FOUND)
    ADD_CUSTOM_COMMAND(
            OUTPUT ${LSchemaOutput}
            COMMAND ${FLEX_EXECUTABLE}
            --outfile=${LSchemaOutput}
            ${PROJECT_ROOT_DIR}/parsers/src/schema_parser.l
            COMMENT "Generating schema_lexer.cpp"
    )
ENDIF()

file(GLOB_RECURSE RFS_C_SOURCES "src/*.cpp")
file(GLOB_RECURSE RFS_C_INCLUDES "includes/*.h")

list(REMOVE_DUPLICATES RFS_C_SOURCES)

MESSAGE("SOURCE FILES : [" ${RFS_C_SOURCES} "]")

if (${APPLE})
    MESSAGE("Host platform: MAC OS X")
    set(RFS_C_LIBRARIES "-lreactfs-common -lreactfs-types")
elseif (${UNIX})
    MESSAGE("Host platform:  Other UNIX")
    set(RFS_C_LIBRARIES "-lreactfs-common -lreactfs-types -lpthread -luuid -lrt ")
endif ()

MESSAGE("Using libraries : #" ${RFS_C_LIBRARIES} "#")

add_library(${PRASERS_LIB_NAME} SHARED ${YSchemaOutput} ${LSchemaOutput} ${RFS_C_INCLUDES} ${RFS_C_SOURCES})
target_link_libraries(${PRASERS_LIB_NAME} ${RFS_C_LIBRARIES})

add_executable(main_schema_p adhoc/main_schema_p.cpp)
target_link_libraries(main_schema_p ${PRASERS_LIB_NAME} ${RFS_C_LIBRARIES})
