cmake_minimum_required(VERSION 3.28)
project(polymorphine)

set(CMAKE_CXX_STANDARD 23)

# Permanently enable compile commands for cmake debugging purposes
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

include(cmake/BuildType)
if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    include(cmake/Gcovr)
endif ()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable")

add_subdirectory(lib)
add_executable(example examples/main.cpp)
target_link_libraries(example PRIVATE polymorphine)

enable_testing()
add_subdirectory(tests)