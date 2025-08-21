cmake_minimum_required(VERSION 3.28)
project(shlang_cpp)

set(CMAKE_CXX_STANDARD 23)

# Permanently enable compile commands for cmake debugging purposes
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/BuildType.cmake)
if (CMAKE_BUILD_TYPE STREQUAL "Coverage")
    include(cmake/Gcovr.cmake)
endif ()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fconcepts-diagnostics-depth=100")


function(register_static_library NAME)
    add_library(${NAME} STATIC ${ARGN})
    target_include_directories(${NAME} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
            $<INSTALL_INTERFACE:include>
    )
    target_compile_features(${NAME} PUBLIC cxx_std_23)
    target_compile_definitions(${NAME} PRIVATE
            $<$<CONFIG:Debug>:DEBUG>
            $<$<CONFIG:Release>:NDEBUG>
    )
    try_setup_coverage_options(${NAME})
    target_compile_options(${NAME} PRIVATE -fPIC)
    target_link_libraries(${NAME} PRIVATE stdc++exp)
endfunction()


file(GLOB_RECURSE MIR_SOURCES lib/mir/*.cpp lib/mir/*.h)
register_static_library(mir ${MIR_SOURCES})

file(GLOB_RECURSE UTILS_SOURCES lib/utility/*.cpp lib/utility/*.h)
register_static_library(utils ${UTILS_SOURCES})

# x64 specific sources
file(GLOB_RECURSE ASM_X64_SOURCES lib/asm/*.cpp lib/asm/*.h)
register_static_library(asm_x64 ${ASM_X64_SOURCES})

file(GLOB_RECURSE LIR_X64_SOURCES lib/lir/x64/*.cpp lib/lir/x64/*.h)
register_static_library(lir_x64 ${LIR_X64_SOURCES})

add_library(ckoof SHARED
        $<TARGET_OBJECTS:mir>
        $<TARGET_OBJECTS:utils>
        $<TARGET_OBJECTS:asm_x64>
        $<TARGET_OBJECTS:lir_x64>
)

try_setup_coverage_options(ckoof)
target_link_libraries(ckoof PRIVATE stdc++exp)
target_include_directories(ckoof PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
        $<INSTALL_INTERFACE:include>
)

add_executable(shlang_cpp examples/main.cpp)
target_link_libraries(shlang_cpp PRIVATE ckoof)

enable_testing()
add_subdirectory(tests)