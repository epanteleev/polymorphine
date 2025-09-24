#include "JitModule.h"

#include "OpCodeBuffer.h"
#include "asm/x64/SizeEvaluator.h"
#include "utility/ArithmeticUtils.h"

#include "RelocResolver.h"

static constexpr auto PAGE_SIZE = 4096;

struct MmapAllocation final {
    std::span<std::uint8_t> memory;
    std::span<std::uint8_t> plt_table;
    std::span<std::uint8_t> code_buffer;
};

static MmapAllocation map_memory(const std::size_t plt_size, const std::size_t code_buffer_size) {
    const auto plt_table_size = align_up(plt_size, PAGE_SIZE);
    const auto total_size = plt_table_size + code_buffer_size;
    const auto memory = static_cast<std::uint8_t*>(mmap(nullptr, total_size,
                                              PROT_READ | PROT_WRITE | PROT_EXEC,
                                              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

    return {std::span(memory, total_size), std::span(memory, plt_table_size), std::span(memory + plt_table_size, code_buffer_size)};
}

JitModule JitModule::assembly(const std::unordered_map<const aasm::Symbol *, std::size_t> &external_symbols, aasm::AsmModule &&module) {
    const auto code_buffer_size = aasm::ModuleSizeEvaluator::module_size_eval(module);
    const auto plt_size = external_symbols.size() * sizeof(std::int64_t);
    const auto [memory, plt_table, code_buffer] = map_memory(plt_size, code_buffer_size);

    std::unordered_map<const aasm::Symbol*, std::size_t> plt_table_map;
    plt_table_map.reserve(external_symbols.size());

    std::span plt_table_span{reinterpret_cast<std::uint64_t*>(plt_table.data()), plt_table.size() / sizeof(std::uint64_t)};
    std::size_t plt_table_offset{};
    for (const auto& [symbol, address] : external_symbols) {
        plt_table_span[plt_table_offset] = address;
        plt_table_offset += 1;
        plt_table_map.emplace(symbol, sizeof(std::int64_t) * (plt_table_offset-1));
    }

    details::RelocResolver resolver(plt_table_map, module, code_buffer, plt_table.size());
    resolver.run();

    JitDataBlob code_blob(resolver.result(), code_buffer);
    return {std::move(module.m_symbol_table), memory, std::move(code_blob)};
}

std::ostream & operator<<(std::ostream &os, const JitModule &blob) {
    return os << blob.m_code_blob;
}