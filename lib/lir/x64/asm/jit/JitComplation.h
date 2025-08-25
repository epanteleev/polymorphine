#pragma once

#include "lir/x64/asm/AsmModule.h"
#include "mir/module/Module.h"

/**
 * Performs JIT compilation
 */
AsmModule jit_compile(const Module& module, bool verbose = false);