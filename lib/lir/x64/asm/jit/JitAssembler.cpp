#include <sys/mman.h>

#include "JitAssembler.h"
#include "asm/SizeEvaluator.h"

static std::size_t module_size_eval(const ObjModule& masm) {
    std::size_t acc = 0;
    for (const auto& emitter : masm.assembler() | std::views::values) {
        acc += aasm::SizeEvaluator::emit(emitter);
    }

    return acc;
}

JitCodeBlob JitAssembler::assembly(ObjModule&& module) {
    const auto buffer_size = module_size_eval(module);

    auto* mapped = static_cast<std::uint8_t*>(mmap(nullptr, buffer_size,
                                                  PROT_READ | PROT_WRITE | PROT_EXEC,
                                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

    std::unordered_map<const aasm::Symbol*, std::unordered_map<const aasm::Symbol*, std::int32_t>> relocation_table;
    std::unordered_map<const aasm::Symbol*, JitCodeChunk> offset_table;
    JitAssembler jit_assembler(mapped);
    for (const auto& [name, emitter] : module.assembler()) {
        const auto start = jit_assembler.size();
        auto reloc = emitter.emit(jit_assembler);
        relocation_table.emplace(name, std::move(reloc));

        offset_table.emplace(name, JitCodeChunk(start, jit_assembler.size() - start));
    }

    for (const auto& relocation : relocation_table | std::views::values) {
        for (const auto& [label, offset_to_patch] : relocation) {
            const auto chunk = offset_table.find(label);
            if (chunk == offset_table.end()) {
                die("Relocation for label '{}' not found in offset table", label->name());
            }

            const auto& [start, size] = chunk->second;
            const auto offset = static_cast<std::int64_t>(start) - offset_to_patch;
            if (!std::in_range<std::int32_t>(offset)) {
                die("Offset {} is out of range for 32-bit patching", offset);
            }

            jit_assembler.patch32(offset_to_patch - sizeof(std::int32_t), offset);
        }
    }

    return {module.symbol_table(), std::move(offset_table), mapped, buffer_size};
}