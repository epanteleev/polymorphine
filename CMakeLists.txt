cmake_minimum_required(VERSION 3.31)
project(shlang_cpp)

set(CMAKE_CXX_STANDARD 23)

add_library(ckoof SHARED lib/mir/types/Type.h
        lib/mir/module/BasicBlock.cpp
        lib/mir/module/BasicBlock.h
        lib/mir/instruction/Instruction.cpp
        lib/mir/instruction/Instruction.h
        lib/mir/mir_frwd.h
        lib/mir/value/Value.cpp
        lib/mir/value/Value.h
        lib/mir/types/TypeMatcher.h
        lib/mir/value/ValueMatcher.h
        lib/mir/types/TypeMatchingRules.h
        lib/utility/OrderedSet.h
        lib/mir/instruction/InstructionVisitor.h
        lib/utility/Error.cpp
        lib/utility/Error.h
        lib/mir/types/Type.cpp
        lib/mir/types/TypeVisitor.h
        lib/mir/module/FunctionData.cpp
        lib/mir/module/FunctionData.h
        lib/mir/module/FunctionPrototype.h
        lib/mir/module/FunctionPrototype.cpp
        lib/mir/value/ArgumentValue.cpp
        lib/mir/value/ArgumentValue.h
        lib/mir/instruction/Binary.h
        lib/mir/instruction/ValueInstruction.h
        lib/mir/instruction/Unary.h
        lib/mir/instruction/TerminateValueInstruction.h
        lib/mir/instruction/TerminateInstruction.h
        lib/mir/mir.h
        lib/mir/value/LocalValue.h
        lib/mir/value/LocalValue.cpp
        lib/mir/instruction/ValueInstruction.cpp
        lib/mir/instruction/Terminator.h
        lib/mir/instruction/Terminator.cpp
        lib/mir/instruction/TerminateInstruction.cpp
        lib/mir/instruction/TerminateValueInstruction.cpp
        lib/mir/instruction/Store.h
        lib/mir/instruction/Alloc.h
        lib/mir/builder/FunctionBuilder.cpp
        lib/mir/builder/FunctionBuilder.h
        lib/mir/instruction/Compare.cpp
        lib/mir/instruction/Compare.h
        lib/base/analysis/AnalysisPass.h
        lib/base/analysis/traverse/PreorderTraverseBase.h
        lib/base/analysis/traverse/Ordering.h
        lib/base/analysis/AnalysisPassCacheBase.h
        lib/base/analysis/traverse/PostOrderTraverseBase.h
        lib/base/analysis/dom/DominatorTreeEvalBase.h
        lib/base/analysis/dom/DominatorTree.h
        lib/asm/Assembler.cpp
        lib/asm/Assembler.h
        lib/asm/Register.h
        lib/asm/Register.cpp
        lib/asm/Address.h
        lib/asm/Instruction.h
        lib/asm/Immediate.h
        lib/mir/module/Module.h
        lib/mir/module/Module.cpp
        lib/mir/builder/ModuleBuilder.cpp
        lib/mir/builder/ModuleBuilder.h
        lib/mir/instruction/GetElementPtr.h
        lib/base/analysis/traverse/BFSOrderTraverseBase.h
        lib/lir/x64/instruction/LIRInstructionBase.cpp
        lib/lir/x64/instruction/LIRInstructionBase.h
        lib/lir/x64/operand/LIRVReg.h
        lib/lir/x64/operand/LIRCst.h
        lib/lir/x64/operand/LIROperand.h
        lib/lir/x64/module/MachBlock.cpp
        lib/lir/x64/module/MachBlock.h
        lib/lir/x64/lir_frwd.h
        lib/asm/elf/Elf.cpp
        lib/asm/elf/Elf.h
        lib/base/Constrains.h
        lib/mir/analysis/Analysis.h
        lib/lir/x64/analysis/Analysis.h
        lib/lir/x64/module/ObjFuncData.h
        lib/lir/x64/instruction/LIRVisitor.h
        lib/lir/x64/operand/LIRVReg.cpp
        lib/lir/x64/module/ObjModule.cpp
        lib/lir/x64/module/ObjModule.h
        lib/lir/x64/lower/Lowering.cpp
        lib/lir/x64/lower/Lowering.h
        lib/lir/x64/operand/LIRArg.h
        lib/lir/x64/operand/LIROperand.cpp
        lib/lir/x64/operand/LIRCst.cpp
        lib/lir/x64/operand/LIRArg.cpp
        lib/base/BasicBlockBase.h
        lib/lir/x64/lower/FunctionLower.cpp
        lib/lir/x64/lower/FunctionLower.h
        lib/mir/value/LocalValueMap.h
        lib/lir/x64/codegen/Codegen.cpp
        lib/lir/x64/codegen/Codegen.h
        lib/lir/x64/analysis/regalloc/LinearScan.h
        lib/lir/x64/analysis/regalloc/RegisterAllocation.h
        lib/lir/x64/analysis/liveness/LivenessAnalysis.h
        lib/lir/x64/analysis/liveness/LiveInfo.h
        lib/base/FunctionDataBase.h
        lib/lir/x64/instruction/Matcher.h
        lib/lir/x64/operand/LIRVRegMap.h
        lib/asm/CPUInstruction.h
        lib/asm/Common.h
        lib/lir/x64/analysis/intervals/LiveIntervalsEval.h
        lib/lir/x64/analysis/intervals/LiveIntervals.h
        lib/lir/x64/instruction/LIRControlInstruction.h
        lib/lir/x64/instruction/LIRControlInstruction.cpp
        lib/lir/x64/instruction/LIRInstruction.cpp
        lib/lir/x64/instruction/LIRInstruction.h
        lib/lir/x64/lir.h
        lib/lir/x64/analysis/regalloc/FixedRegistersEval.h
        lib/lir/x64/analysis/regalloc/FixedRegisters.h
)

set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address")
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable")

target_link_libraries(ckoof PRIVATE stdc++exp)
target_include_directories(ckoof PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
        $<INSTALL_INTERFACE:include>
)

add_executable(shlang_cpp main.cpp)
target_link_libraries(shlang_cpp PRIVATE ckoof)

enable_testing()
add_subdirectory(tests)