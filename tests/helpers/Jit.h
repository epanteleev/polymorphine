#pragma once

#include "lir/x64/lir.h"

ObjModule do_jit_compile(const Module& module, bool verbose = false);

/**
 * Performs JIT compilation and assembly of the given module.
 * @param module The module to be compiled.
 * @param verbose If true, prints the intermediate representations after each compilation step.
 * @return A JitCodeBlob containing the compiled code.
 */
JitCodeBlob do_compile_and_assembly(const Module& module, bool verbose = false);