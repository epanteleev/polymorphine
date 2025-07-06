cmake_minimum_required(VERSION 3.31)
project(shlang_cpp)

set(CMAKE_CXX_STANDARD 23)

add_library(ckoof SHARED lib/types/Type.h
        lib/module/BasicBlock.cpp
        lib/module/BasicBlock.h
        lib/instruction/Instruction.cpp
        lib/instruction/Instruction.h
        lib/ir_frwd.h
        lib/value/Value.cpp
        lib/value/Value.h
        lib/types/TypeMatcher.h
        lib/value/ValueMatcher.h
        lib/types/TypeMatchingRules.h
        lib/utility/OrderedSet.h
        lib/instruction/InstructionVisitor.h
        lib/utility/Error.cpp
        lib/utility/Error.h
        lib/types/Type.cpp
        lib/types/TypeVisitor.h
        lib/module/FunctionData.cpp
        lib/module/FunctionData.h
        lib/module/FunctionPrototype.h
        lib/module/FunctionPrototype.cpp
        lib/value/ArgumentValue.cpp
        lib/value/ArgumentValue.h
        lib/instruction/Binary.h
        lib/instruction/ValueInstruction.h
        lib/instruction/Unary.h
        lib/instruction/TerminateValueInstruction.h
        lib/instruction/TerminateInstruction.h
        lib/ir.h
        lib/value/LocalValue.h
        lib/value/LocalValue.cpp
        lib/instruction/ValueInstruction.cpp
        lib/instruction/Terminator.h
        lib/instruction/Terminator.cpp
        lib/instruction/TerminateInstruction.cpp
        lib/instruction/TerminateValueInstruction.cpp
        lib/instruction/Store.h
        lib/instruction/Alloc.h
        lib/builder/FunctionBuilder.cpp
        lib/builder/FunctionBuilder.h
        lib/instruction/Compare.cpp
        lib/instruction/Compare.h
        lib/pass/analysis/AnalysisPass.cpp
        lib/pass/analysis/AnalysisPass.h
        lib/pass/analysis/traverse/PreorderTraverseBase.h
        lib/pass/analysis/traverse/Ordering.h
        lib/pass/analysis/AnalysisPassCacheBase.cpp
        lib/pass/analysis/AnalysisPassCacheBase.h
        lib/pass/analysis/traverse/PostOrderTraverseBase.h
        lib/pass/analysis/dom/DominatorTreeEvalBase.h
        lib/pass/analysis/dom/DominatorTree.h
        lib/asm/Assembler.cpp
        lib/asm/Assembler.h
        lib/asm/Register.h
        lib/asm/Register.cpp
        lib/asm/Address.cpp
        lib/asm/Address.h
        lib/asm/Instruction.h
        lib/asm/Immediate.h
        lib/asm/AbstractAssembler.h
        lib/module/Module.h
        lib/module/Module.cpp
        lib/builder/ModuleBuilder.cpp
        lib/builder/ModuleBuilder.h
        lib/instruction/GetElementPtr.h
        lib/pass/analysis/traverse/BFSOrderTraverseBase.h
        lib/platform/lir/x64/LIRInstructionBase.cpp
        lib/platform/lir/x64/LIRInstructionBase.h
        lib/platform/lir/x64/Vreg.h
        lib/platform/lir/x64/Chain.h
        lib/platform/lir/x64/Chain.cpp
        lib/platform/lir/x64/LIRCst.h
        lib/platform/lir/x64/LIROperand.h
        lib/platform/lir/x64/MachBlock.cpp
        lib/platform/lir/x64/MachBlock.h
        lib/platform/lir/x64/mach_frwd.h
        lib/asm/elf/Elf.cpp
        lib/asm/elf/Elf.h
        lib/pass/Constrains.h
        lib/pass/analysis/Analysis.h
        lib/platform/lir/x64/analysis/Analysis.h
        lib/platform/lir/x64/ObjFuncData.h
        lib/platform/lir/x64/LIRVisitor.h
        lib/platform/lir/x64/Vreg.cpp
        lib/platform/lir/x64/ObjModule.cpp
        lib/platform/lir/x64/ObjModule.h
        lib/platform/lir/x64/lower/Lowering.cpp
        lib/platform/lir/x64/lower/Lowering.h
        lib/platform/lir/x64/lower/VregBuilder.cpp
        lib/platform/lir/x64/lower/VregBuilder.hpp
        lib/platform/lir/x64/LIRArg.h
        lib/platform/lir/x64/LIROperand.cpp
        lib/platform/lir/x64/LIRCst.cpp
        lib/platform/lir/x64/LIRArg.cpp
        lib/platform/lir/x64/Utils.h
        lib/pass/analysis/loop/LoopInfoEvalBase.h
        lib/pass/analysis/loop/LoopInfoBase.h
        lib/pass/analysis/traverse/LinearScanOrderBase.h
        lib/base/BasicBlockBase.h
        lib/platform/lir/x64/lower/FunctionLower.cpp
        lib/platform/lir/x64/lower/FunctionLower.h
        lib/value/LocalValueMap.h
        lib/platform/lir/x64/codegen/Codegen.cpp
        lib/platform/lir/x64/codegen/Codegen.h
        lib/platform/lir/x64/analysis/regalloc/LinearScan.h
        lib/platform/lir/x64/analysis/regalloc/RegisterAllocation.h
        lib/platform/lir/x64/analysis/liveness/LivenessAnalysis.h
        lib/platform/lir/x64/analysis/liveness/LiveInfo.h
        lib/base/FunctionDataBase.h
        lib/platform/lir/x64/Matcher.h
        lib/platform/lir/x64/VRegMap.h
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