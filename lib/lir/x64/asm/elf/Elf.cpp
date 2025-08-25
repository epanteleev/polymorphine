#include "Elf.h"

#include "asm/x64/SizeEvaluator.h"
#include "lir/x64/asm/jit/OpCodeBuffer.h"
#include "lir/x64/asm/jit/JitCodeBlob.h"

Elf Elf::collect(const AsmModule &module) {
    elf::elfio writer;

    writer.set_os_abi(ELFOSABI_LINUX);
    writer.set_type(ET_REL);
    writer.set_machine(EM_X86_64);

    elf::section* text_sec = writer.sections.add(".text");
    text_sec->set_type(SHT_PROGBITS);
    text_sec->set_flags(SHF_ALLOC | SHF_EXECINSTR);
    text_sec->set_addr_align( 0x10 );

    // Create string table section
    elf::section* str_sec = writer.sections.add(".strtab");
    str_sec->set_type( SHT_STRTAB );

    // Create symbol table section
    elf::section* sym_sec = writer.sections.add(".symtab");
    sym_sec->set_type(SHT_SYMTAB);
    sym_sec->set_info(1);
    sym_sec->set_addr_align(0x4);
    sym_sec->set_entry_size(elf::elfio::get_default_entry_size(SHT_SYMTAB));
    sym_sec->set_link(str_sec->get_index());

    std::vector<uint8_t> code_buffer;
    code_buffer.resize(aasm::SizeEvaluator::module_size_eval(module));
    OpCodeBuffer assembler{code_buffer};
    for (const auto &asm_buffer: module.assembler() | std::views::values) {
        asm_buffer.emit(assembler);
    }
    text_sec->set_data(reinterpret_cast<const char*>(code_buffer.data()), code_buffer.size());

    return Elf(std::move(writer));
}

void Elf::save(const std::string_view path) {
    std::ofstream stream;
    stream.open(path.data(), std::ios::out | std::ios::binary);
    if (!stream) {
        return;
    }

    m_writer.save(stream);
}