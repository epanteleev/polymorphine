#pragma once

#include "lir/x64/lir.h"
#include "lir/x64/asm/jit/JitModule.h"

/**
 * Performs JIT compilation and assembly of the given module.
 * @param module The module to be compiled.
 * @param verbose If true, prints the intermediate representations after each step.
 * @return A JitCodeBlob containing the compiled code.
 */
JitModule jit_compile_and_assembly(const Module& module, bool verbose = false);

/**
 * Performs JIT compilation and assembly of the given module with external symbols.
 * @param external_symbols A map of external symbol names to their addresses.
 * @param module The module to be compiled.
 * @param verbose If true, prints the intermediate representations after each step.
 * @return A JitCodeBlob containing the compiled code.
 */
JitModule jit_compile_and_assembly(const std::unordered_map<std::string, std::size_t>& external_symbols, const Module& module, bool verbose = false);
JitModule jit_compile_and_assembly(const std::unordered_map<std::string, std::size_t>& external_symbols, const Module& module, const std::unordered_map<std::string, std::size_t>& asm_size, bool verbose = false);

/**
 * Performs JIT compilation, assembly of the given module and verify number of instructions in the functions.
 * @param module The module to be compiled.
 * @param asm_size A map of function names to their expected assembly sizes.
 * @param verbose If true, prints the intermediate representations after each step.
 * @return A JitCodeBlob containing the compiled code.
 */
JitModule jit_compile_and_assembly(const Module& module, const std::unordered_map<std::string, std::size_t>& asm_size, bool verbose);