
# CMake module to register sanitizers for debugging
function(register_sanitizer name flags)
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

register_sanitizer(Asan "-fsanitize=address")
register_sanitizer(Usan "-fsanitize=undefined")