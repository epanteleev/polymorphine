#include <sys/mman.h>

#include "JitAssembler.h"

JitCodeBlob JitAssembler::assembly(const ObjModule &masm) {
    const auto buffer_size = SizeEvaluator::eval(masm);

    auto* mapped = static_cast<std::uint8_t*>(mmap(nullptr, buffer_size,
                                                  PROT_READ | PROT_WRITE | PROT_EXEC,
                                                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

    std::unordered_map<std::string, std::size_t> offset_table;
    JitAssembler jit_assembler(mapped);
    for (const auto& [name, emitter] : masm.emitters()) {
        const auto start = jit_assembler.size();
        emitter.emit(jit_assembler);
        offset_table.emplace(name, start);
    }

    return {std::move(offset_table), mapped, buffer_size};
}
