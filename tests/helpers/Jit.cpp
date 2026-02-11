#include <iostream>

#include "Jit.h"
#include "lir/x64/asm/compilation/Complation.h"
#include "mir/mir.h"

static void verify(const aasm::AsmModule& obj, const std::unordered_map<std::string, std::size_t>& asm_size) {
    for (const auto &[name, size]: asm_size) {
        const auto fun = obj.function(name);
        if (!fun.has_value()) {
            die("Function '{}' not found in module", name);
        }

        if (fun.value()->size() != size) {
            die("Function '{}' size mismatch: expected {}, got {}", name, size, fun.value()->size());
        }
    }
}

CompiledModule jit_compile_and_assembly(const Module& module, const std::unordered_map<std::string, std::size_t>& asm_size, const bool verbose) {
    auto obj = jit_compile(module, verbose);
    verify(obj, asm_size);

    static const std::unordered_map<const aasm::Symbol*, std::size_t> external_symbols;
    const auto buffer = CompiledModule::assembly(external_symbols, std::move(obj));
    if (verbose) {
        std::cout << buffer << std::endl;
    }

    return buffer;
}

static std::unordered_map<std::string, std::size_t> empty_external_symbols;
static constinit OptPipeline empty_opt_pipeline;

JitCompiler::JitCompiler(const OptPipeline& opt_pipeline, const bool verbose) noexcept:
    m_external_symbols(empty_external_symbols),
    m_opt_pipeline(opt_pipeline),
    m_verbose(verbose) {}

JitCompiler::JitCompiler(const bool verbose) noexcept:
    m_external_symbols(empty_external_symbols),
    m_opt_pipeline(empty_opt_pipeline),
    m_verbose(verbose) {}

CompiledModule JitCompiler::compile(const Module &module) const {
    return jit_compile_and_assembly(m_external_symbols, module, {}, m_verbose);
}

CompiledModule jit_compile_and_assembly(const Module& module, const bool verbose) {
    static std::unordered_map<std::string, std::size_t> nothing;
    return jit_compile_and_assembly(module, nothing, verbose);
}

CompiledModule jit_compile_and_assembly(const std::unordered_map<std::string, std::size_t>& external_symbols, const Module& module, const bool verbose) {
    static std::unordered_map<std::string, std::size_t> nothing;
    return jit_compile_and_assembly(external_symbols, module, nothing, verbose);
}

CompiledModule jit_compile_and_assembly(const std::unordered_map<std::string, std::size_t>& external_symbols, const Module& module, const std::unordered_map<std::string, std::size_t>& asm_size, const bool verbose) {
    auto obj = jit_compile(module, verbose);
    std::unordered_map<const aasm::Symbol*, std::size_t> external_symbols_;
    external_symbols_.reserve(external_symbols.size());
    for (const auto& [name, addr] : external_symbols) {
        const auto [symbol, _] = obj.m_symbol_table.add(name, aasm::BindAttribute::INTERNAL);
        external_symbols_[symbol] = addr;
    }

    verify(obj, asm_size);
    const auto buffer = CompiledModule::assembly(external_symbols_, std::move(obj));
    if (verbose) {
        std::cout << buffer << std::endl;
    }

    return buffer;
}