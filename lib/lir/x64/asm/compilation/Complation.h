#pragma once

#include "asm/x64/AsmModule.h"
#include "mir/module/Module.h"

/**
 * Performs JIT compilation
 */
aasm::AsmModule jit_compile(const Module& module, bool verbose = false);