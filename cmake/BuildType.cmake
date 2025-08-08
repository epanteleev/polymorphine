
# Register new build types for sanitizers and coverage
function(register_build_type name flags)
    string(TOUPPER ${name} PREFIX)
    message(STATUS "Registering sanitizer: ${PREFIX} with flags: ${flags}")
    # Add the sanitizer configuration type
    list(APPEND CMAKE_CONFIGURATION_TYPES "${name}")

    # Set the flags for the sanitizer
    set(CMAKE_C_FLAGS_${PREFIX} "${CMAKE_C_FLAGS_DEBUG} ${flags}" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS_${PREFIX} "${CMAKE_CXX_FLAGS_DEBUG} ${flags}" PARENT_SCOPE)
    set(CMAKE_EXE_LINKER_FLAGS_${PREFIX} "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${flags}" PARENT_SCOPE)
    set(CMAKE_SHARED_LINKER_FLAGS_${PREFIX} "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${flags}" PARENT_SCOPE)
endfunction()

# Try to set up coverage options for a target if '-DCMAKE_BUILD_TYPE=Coverage'
function(try_setup_coverage_options target)
    if (NOT CMAKE_BUILD_TYPE STREQUAL "Coverage")
        return()
    endif ()

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target} PRIVATE -fprofile-arcs -ftest-coverage)
        target_link_options(${target} PRIVATE -fprofile-arcs -ftest-coverage)
    else()
        message(WARNING "Coverage options are not set for compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
endfunction()

register_build_type(Asan "-fsanitize=address")
register_build_type(Usan "-fsanitize=undefined")
register_build_type(Coverage "") # No specific flags for coverage, handled in try_setup_coverage_options