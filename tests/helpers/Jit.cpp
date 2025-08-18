#include "Jit.h"

#include <iostream>

#include "lir/x64/lir.h"
#include "mir/mir.h"

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
        for (const auto& bb: func->basic_blocks()) {
            verify_def_use_chain(name, &bb);
            verify_cfg(name, &bb);
        }
    }
}

AsmModule jit_compile(const Module &module, const bool verbose) {
    verify_data_and_control_flow_edges(module);
    if (verbose) {
        std::cout << module << std::endl;
    }
    Lowering lower(module);
    lower.run();
    auto result = lower.result();

    Codegen codegen(result);
    codegen.run();
    auto obj = codegen.result();
    if (verbose) {
        std::cout << obj << std::endl;
    }

    return obj;
}

JitModule jit_compile_and_assembly(const Module& module, const bool verbose) {
    static std::unordered_map<std::string, std::size_t> nothing;
    return jit_compile_and_assembly(module, nothing, verbose);
}

JitModule jit_compile_and_assembly(const Module& module, const std::unordered_map<std::string, std::size_t>& asm_size, const bool verbose) {
    const auto obj = jit_compile(module, verbose);
    for (const auto& [name, size] : asm_size) {
        const auto fun = obj.function(name);
        if (!fun.has_value()) {
            die("Function '{}' not found in module", name);
        }

        if (fun.value()->size() != size) {
            die("Function '{}' size mismatch: expected {}, got {}", name, size, fun.value()->size());
        }
    }

    static const std::unordered_map<const aasm::Symbol*, std::size_t> external_symbols;
    const auto buffer = JitModule::assembly(external_symbols, obj);
    if (verbose) {
        std::cout << buffer << std::endl;
    }

    return buffer;
}

JitModule jit_compile_and_assembly(const std::unordered_map<std::string, std::size_t>& external_symbols, const Module& module, const bool verbose) {
    static std::unordered_map<std::string, std::size_t> nothing;
    return jit_compile_and_assembly(external_symbols, module, nothing, verbose);
}

JitModule jit_compile_and_assembly(const std::unordered_map<std::string, std::size_t>& external_symbols, const Module& module, const std::unordered_map<std::string, std::size_t>& asm_size, const bool verbose) {
    const auto obj = jit_compile(module, true);
    std::unordered_map<const aasm::Symbol*, std::size_t> external_symbols_;
    for (const auto& [name, addr] : external_symbols) {
        const auto [symbol, _] = obj.symbol_table()->add(name, aasm::Linkage::INTERNAL);
        external_symbols_[symbol] = addr;
    }

    for (const auto& [name, size] : asm_size) {
        const auto fun = obj.function(name);
        if (!fun.has_value()) {
            die("Function '{}' not found in module", name);
        }

        if (fun.value()->size() != size) {
            die("Function '{}' size mismatch: expected {}, got {}", name, size, fun.value()->size());
        }
    }

    const auto buffer = JitModule::assembly(external_symbols_, obj);
    if (verbose) {
        std::cout << buffer << std::endl;
    }

    return buffer;
}