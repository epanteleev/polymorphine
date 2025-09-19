#include "JitComplation.h"

#include <iostream>

#include "lir/x64/codegen/Codegen.h"
#include "lir/x64/lower/Lowering.h"
#include "mir/value/LocalValue.h"

#ifndef NDEBUG
static void verify_def_use_chain(const std::string_view name, const BasicBlock* bb) {
    for (const auto& inst: bb->instructions()) {
        for (const auto& operand: inst.operands()) {
            const auto local = LocalValue::try_from(operand);
            if (!local.has_value()) {
                continue;
            }

            if (const auto& users = local.value().users(); std::ranges::contains(users, &inst)) {
                continue;
            }

            std::cerr << "Def-Use chain error in function '" << name << "': instruction " << inst.id()
                      << " uses value " << local.value() << " which has no user." << std::endl;
            std::abort();
        }
    }
}

static void verify_cfg(const std::string_view name, const BasicBlock* bb) {
    for (const auto pred: bb->predecessors()) {
        if (!std::ranges::contains(pred->successors(), bb)) {
            std::cerr << "CFG error: BasicBlock " << bb->id()
                      << " is not a successor of " << pred->id() << " in function '" << name << "'." << std::endl;
            std::abort();
        }

        if (const auto last = pred->last(); last.isa(any_return())) {
            if (!bb->successors().empty()) {
                std::cerr << "CFG error: BasicBlock " << pred->id()
                          << " ends with a return instruction but has successors in function '" << name << "'." << std::endl;
                std::abort();
            }
        }
    }
}

static void verify_data_and_control_flow_edges(const Module& module) {
    for (const auto& [name, func]: module.functions()) {
        for (const auto& bb: func.basic_blocks()) {
            verify_def_use_chain(name, &bb);
            verify_cfg(name, &bb);
        }
    }
}
#endif

AsmModule jit_compile(const Module &module, const bool verbose) {
#ifndef NDEBUG
    verify_data_and_control_flow_edges(module);
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

    Codegen codegen(std::move(result));
    codegen.run();
    auto obj = codegen.result();
    if (verbose) {
        std::cout << obj << std::endl;
    }

    return obj;
}