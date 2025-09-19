#include <iostream>

#include "Jit.h"
#include "lir/x64/lir.h"
#include "lir/x64/asm/jit/JitComplation.h"
#include "mir/mir.h"

JitModule jit_compile_and_assembly(const Module& module, const std::unordered_map<std::string, std::size_t>& asm_size, const bool verbose) {
    auto obj = jit_compile(module, verbose);
    for (const auto &[name, size]: asm_size) {
        const auto fun = obj.function(name);
        if (!fun.has_value()) {
            die("Function '{}' not found in module", name);
        }

        if (fun.value()->size() != size) {
            die("Function '{}' size mismatch: expected {}, got {}", name, size, fun.value()->size());
        }
    }

    static const std::unordered_map<const aasm::Symbol*, std::size_t> external_symbols;
    const auto buffer = JitModule::assembly(external_symbols, std::move(obj));
    if (verbose) {
        std::cout << buffer << std::endl;
    }

    return buffer;
}

JitModule jit_compile_and_assembly(const Module& module, const bool verbose) {
    static std::unordered_map<std::string, std::size_t> nothing;
    return jit_compile_and_assembly(module, nothing, verbose);
}

JitModule jit_compile_and_assembly(const std::unordered_map<std::string, std::size_t>& external_symbols, const Module& module, const bool verbose) {
    static std::unordered_map<std::string, std::size_t> nothing;
    return jit_compile_and_assembly(external_symbols, module, nothing, verbose);
}

JitModule jit_compile_and_assembly(const std::unordered_map<std::string, std::size_t>& external_symbols, const Module& module, const std::unordered_map<std::string, std::size_t>& asm_size, const bool verbose) {
    auto obj = jit_compile(module, verbose);
    std::unordered_map<const aasm::Symbol*, std::size_t> external_symbols_;
    external_symbols_.reserve(external_symbols.size());
    for (const auto& [name, addr] : external_symbols) {
        const auto [symbol, _] = obj.m_symbol_table.add(name, aasm::BindAttribute::INTERNAL);
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

    const auto buffer = JitModule::assembly(external_symbols_, std::move(obj));
    if (verbose) {
        std::cout << buffer << std::endl;
    }

    return buffer;
}