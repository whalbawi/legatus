cmake_minimum_required(VERSION 3.20)
project(legatus CXX)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

# Directories
set(LEGATUS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(LEGATUS_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)

set(LEGATUS_THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
set(LEGATUS_GOOGLETEST_DIR ${LEGATUS_THIRD_PARTY_DIR}/googletest)

# Dependencies
add_subdirectory(${LEGATUS_GOOGLETEST_DIR} EXCLUDE_FROM_ALL)

# Project version
file(READ VERSION LEGATUS_PROJECT_VERSION)
string(STRIP ${LEGATUS_PROJECT_VERSION} LEGATUS_PROJECT_VERSION)

# and commit
execute_process(COMMAND git describe --always --dirty
    OUTPUT_VARIABLE LEGATUS_GIT_COMMIT_HASH
    ERROR_VARIABLE LEGATUS_GIT_COMMIT_ERROR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE LEGATUS_GIT_COMMIT_RESULT
)
if (NOT ${LEGATUS_GIT_COMMIT_RESULT} STREQUAL 0)
    message(WARNING "Could not retrieve git commit: " ${LEGATUS_GIT_COMMIT_ERROR})
    set(LEGATUS_GIT_COMMIT_HASH unknown)
endif()

message("legatus version information: " ${LEGATUS_PROJECT_VERSION} " - " ${LEGATUS_GIT_COMMIT_HASH})
configure_file(${LEGATUS_SRC_DIR}/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/gen/version.h @ONLY)

# Source files
set(LEGATUS_SRC_LIST
    ${LEGATUS_SRC_DIR}/legatus.cpp
)

# Test files
set(LEGATUS_TEST_LIST
    ${LEGATUS_TEST_DIR}/legatus_test.cpp
)

add_library(legatus-lib ${LEGATUS_SRC_LIST})
target_include_directories(legatus-lib PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gen)
set_target_properties(legatus-lib PROPERTIES OUTPUT_NAME legatus)

add_executable(legatus-tests ${LEGATUS_TEST_LIST})
target_include_directories(legatus-tests PRIVATE ${LEGATUS_SRC_DIR})
target_link_libraries(legatus-tests legatus-lib gtest_main)

add_test(unit-tests legatus-tests)
