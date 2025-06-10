cmake_minimum_required(VERSION 3.31)
project(shlang_cpp)

set(CMAKE_CXX_STANDARD 23)

add_library(ckoof SHARED lib/types/Type.h
        lib/BasicBlock.cpp
        lib/BasicBlock.h
        lib/instruction/Instruction.cpp
        lib/instruction/Instruction.h
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
        lib/FunctionData.cpp
        lib/FunctionData.h
        lib/FunctionPrototype.h
        lib/FunctionPrototype.cpp
        lib/ArgumentValue.cpp
        lib/ArgumentValue.h
        lib/instruction/Binary.h
        lib/instruction/ValueInstruction.h
        lib/instruction/Unary.h
        lib/instruction/TerminateValueInstruction.h
        lib/instruction/TerminateInstruction.h
        lib/ir.h
        lib/value/LocalValue.h
)

target_link_libraries(ckoof PRIVATE stdc++exp)
target_include_directories(ckoof PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
        $<INSTALL_INTERFACE:include>
)

add_executable(shlang_cpp main.cpp)
target_link_libraries(shlang_cpp PRIVATE ckoof)

add_subdirectory(tests)