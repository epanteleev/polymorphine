#include <sys/mman.h>

#include "JitAssembler.h"
#include "asm/SizeEvaluator.h"

static std::size_t module_size_eval(const ObjModule& masm) {
    std::size_t acc = 0;
    for (const auto& emitter : masm.emitters() | std::views::values) {
        acc += aasm::SizeEvaluator::emit(emitter);
    }

    return acc;
}

JitCodeBlob JitAssembler::assembly(const ObjModule &module) {
    const auto buffer_size = module_size_eval(module);

    auto* mapped = static_cast<std::uint8_t*>(mmap(nullptr, buffer_size,
                                                  PROT_READ | PROT_WRITE | PROT_EXEC,
                                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

    std::unordered_map<std::string, std::size_t> offset_table;
    JitAssembler jit_assembler(mapped);
    for (const auto& [name, emitter] : module.emitters()) {
        const auto start = jit_assembler.size();
        emitter.emit(jit_assembler);
        offset_table.emplace(name, start);
    }

    return {std::move(offset_table), mapped, buffer_size};
}
