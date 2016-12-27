cmake_minimum_required(VERSION 3.1)

project(reactfs-parsers VERSION 0.1.0)

INCLUDE(../makefiles/CMakeInclude.txt)
set(PROJECT_CMAKE_COMMON ${PROJECT_ROOT_DIR}/makefiles/CMakeCommon.txt)
MESSAGE("Including common makefile : " ${PROJECT_CMAKE_COMMON})

INCLUDE(${PROJECT_CMAKE_COMMON})

INCLUDE_DIRECTORIES(${PROJECT_ROOT_DIR}/parsers/includes/ ${PROJECT_ROOT_DIR}/parsers/src/)

set(PRASERS_LIB_NAME ${PROJECT_NAME})

FIND_PACKAGE(BISON REQUIRED)
SET(YSchemaOutput ${PROJECT_ROOT_DIR}/parsers/src/schema_parser.cpp)
IF (BISON_FOUND)
    ADD_CUSTOM_COMMAND(
            OUTPUT ${YSchemaOutput}
            COMMAND ${BISON_EXECUTABLE}
            --defines=${PROJECT_ROOT_DIR}/parsers/includes/schema_tokens.h
            --output=${YSchemaOutput}
            ${PROJECT_ROOT_DIR}/parsers/src/schema_parser.yy
            COMMENT "Generating schema_parser.cpp"
    )
ENDIF ()

FIND_PACKAGE(FLEX REQUIRED)
SET(LSchemaOutput ${PROJECT_ROOT_DIR}/parsers/src/schema_lexer.cpp)
IF (FLEX_FOUND)
    ADD_CUSTOM_COMMAND(
            OUTPUT ${LSchemaOutput}
            COMMAND ${FLEX_EXECUTABLE}
            --outfile=${LSchemaOutput}
            ${PROJECT_ROOT_DIR}/parsers/src/schema_parser.l
            COMMENT "Generating schema_lexer.cpp"
    )
ENDIF ()

file(GLOB_RECURSE RFS_C_SOURCES "src/*.cpp")
file(GLOB_RECURSE RFS_C_INCLUDES "includes/*.h")

list(REMOVE_DUPLICATES RFS_C_SOURCES)
list(REMOVE_ITEM RFS_C_SOURCES ${PROJECT_ROOT_DIR}/parsers/src/template_util.cpp ${PROJECT_ROOT_DIR}/parsers/src/generator_util.cpp)

LINK_DIRECTORIES(${EXT_LINK_DIRS} ${RFS_BUILD_COMMON} ${RFS_BUILD_TYPES} ${RFS_BUILD_PARSER})

MESSAGE("SOURCE FILES : [" ${RFS_C_SOURCES} "]")

MESSAGE("Using libraries : [" ${RFS_TYPES_LIBS} "]")

add_library(${PRASERS_LIB_NAME} SHARED ${YSchemaOutput} ${LSchemaOutput} ${RFS_C_INCLUDES} ${RFS_C_SOURCES})
target_link_libraries(${PRASERS_LIB_NAME} ${RFS_TYPES_LIBS})

add_executable(main_schema_p adhoc/main_schema_p.cpp)
target_link_libraries(main_schema_p ${PRASERS_LIB_NAME} ${RFS_TYPES_LIBS})

add_executable(template_util src/template_util.cpp)
target_link_libraries(template_util ${PRASERS_LIB_NAME} ${RFS_TYPES_LIBS})

add_executable(generator_util src/generator_util.cpp)
target_link_libraries(generator_util ${PRASERS_LIB_NAME} ${RFS_TYPES_LIBS} "-lastyle")