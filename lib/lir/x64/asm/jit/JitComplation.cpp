#include "JitComplation.h"

#include <iostream>

#include "lir/x64/codegen/Codegen.h"
#include "lir/x64/lower/Lowering.h"
#include "mir/module/verify/Verifier.h"


aasm::AsmModule jit_compile(const Module &module, const bool verbose) {
#ifndef NDEBUG
    if (const auto verifier_result = Verifier::apply(module); verifier_result.has_value()) {
        std::cerr << "Invalid instruction: " << verifier_result.value() << std::endl;
        std::cerr << module << std::endl;
        std::abort();
    }
#endif
    if (verbose) {
        std::cout << module << std::endl;
    }
    Lowering lower(module);
    lower.run();
    auto result = lower.result();
    if (verbose) {
        std::cout << result << std::endl;
    }

    Codegen codegen(result);
    codegen.run();
    auto obj = codegen.result();
    if (verbose) {
        std::cout << obj << std::endl;
    }

    return obj;
}