cmake_minimum_required(VERSION 3.31)
project(shlang_cpp)

set(CMAKE_CXX_STANDARD 26)

add_library(ckoof SHARED lib/types/Type.h
        lib/BasicBlock.cpp
        lib/BasicBlock.h
        lib/Instruction.cpp
        lib/Instruction.h
        lib/ir_frwd.h
        lib/value/Value.cpp
        lib/value/Value.h
        lib/types/TypeMatcher.h
        lib/value/ValueMatcher.h
        lib/types/TypeMatchingRules.h
        lib/utility/OrderedSet.h
        lib/InstructionVisitor.h
        lib/utility/Error.cpp
        lib/utility/Error.h
        lib/types/Type.cpp
        lib/types/TypeVisitor.h
)

target_link_libraries(ckoof PRIVATE stdc++exp)
target_include_directories(ckoof PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
        $<INSTALL_INTERFACE:include>
)

add_executable(shlang_cpp main.cpp)
target_link_libraries(shlang_cpp PRIVATE ckoof)

add_subdirectory(tests)