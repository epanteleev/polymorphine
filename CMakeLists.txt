cmake_minimum_required(VERSION 3.28)
project(shlang_cpp)

set(CMAKE_CXX_STANDARD 23)

# Permanently enable compile commands for cmake debugging purposes
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

include(cmake/BuildType)
if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    include(cmake/Gcovr)
endif ()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")

add_subdirectory(lib)
add_executable(shlang_cpp examples/main.cpp)
target_link_libraries(shlang_cpp PRIVATE ckoof)

enable_testing()
add_subdirectory(tests)