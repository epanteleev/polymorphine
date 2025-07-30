#pragma once

#include "lir/x64/lir.h"

/**
 * Performs JIT compilation of the given module.
 * @param module The module to be compiled.
 * @param verbose If true, prints the intermediate representations after each compilation step.
 * @return A JitCodeBlob containing the compiled code.
 */
JitCodeBlob do_jit_compilation(const Module& module, bool verbose = false);