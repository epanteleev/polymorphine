function(register_static_library NAME)
    add_library(${NAME} STATIC ${ARGN})
    target_include_directories(${NAME} PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
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

file(GLOB_RECURSE BASE_SOURCES base/*.cpp base/*.h)
register_static_library(base ${BASE_SOURCES})

file(GLOB_RECURSE MIR_SOURCES mir/*.cpp mir/*.h)
register_static_library(mir ${MIR_SOURCES})

file(GLOB_RECURSE UTILS_SOURCES utility/*.cpp utility/*.h)
register_static_library(utils ${UTILS_SOURCES})

# x64 specific sources
file(GLOB_RECURSE ASM_X64_SOURCES asm/*.cpp asm/*.h)
register_static_library(asm_x64 ${ASM_X64_SOURCES})

file(GLOB_RECURSE LIR_X64_SOURCES lir/x64/*.cpp lir/x64/*.h)
register_static_library(lir_x64 ${LIR_X64_SOURCES})

add_library(ckoof SHARED
        $<TARGET_OBJECTS:base>
        $<TARGET_OBJECTS:mir>
        $<TARGET_OBJECTS:utils>
        $<TARGET_OBJECTS:asm_x64>
        $<TARGET_OBJECTS:lir_x64>
)

try_setup_coverage_options(ckoof)
target_link_libraries(ckoof PRIVATE stdc++exp)
target_include_directories(ckoof PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)