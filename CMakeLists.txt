cmake_minimum_required(VERSION 3.28)
project(shlang_cpp)

set(CMAKE_CXX_STANDARD 23)

# Permanently enable compile commands for cmake debugging purposes
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/Sanitizers.cmake)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -O3 -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable")

file(GLOB_RECURSE MIR_SOURCES lib/mir/*.cpp lib/mir/*.h)
file(GLOB_RECURSE ASM_SOURCES lib/asm/*.cpp lib/asm/*.h)
file(GLOB_RECURSE UTILS_SOURCES lib/utility/*.cpp lib/utility/*.h)

# x64 specific sources
file(GLOB_RECURSE LIR_X64_SOURCES lib/lir/x64/*.cpp lib/lir/x64/*.h)
file(GLOB_RECURSE ASM_X64_SOURCES lib/asm/*.cpp lib/asm/*.h)

add_library(ckoof SHARED
        ${MIR_SOURCES}
        ${LIR_X64_SOURCES}
        ${ASM_SOURCES}
        ${ASM_X64_SOURCES}
        ${UTILS_SOURCES}
)

target_link_libraries(ckoof PRIVATE stdc++exp)
target_include_directories(ckoof PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
        $<INSTALL_INTERFACE:include>
)

add_executable(shlang_cpp examples/main.cpp)
target_link_libraries(shlang_cpp PRIVATE ckoof)

enable_testing()
add_subdirectory(tests)