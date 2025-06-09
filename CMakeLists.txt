cmake_minimum_required(VERSION 3.31)
project(shlang_cpp)

set(CMAKE_CXX_STANDARD 26)

add_executable(shlang_cpp main.cpp
        lib/types/Type.h
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
        lib/utlility/OrderedSet.h
        lib/InstructionList.h)
