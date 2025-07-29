#include "Jit.h"

#include <iostream>

JitCodeBlob do_jit_compilation(const Module& module, const bool verbose) {
    if (verbose) {
        std::cout << module << std::endl;
    }
    Lowering lower(module);
    lower.run();
    const auto result = lower.result();
    if (verbose) {
        std::cout << result << std::endl;
    }

    Codegen codegen(result);
    codegen.run();
    const auto obj = codegen.result();
    if (verbose) {
        std::cout << obj << std::endl;
    }

    const auto buffer = JitAssembler::assembly(obj);
    if (verbose) {
        std::cout << buffer << std::endl;
    }

    return buffer;
}
