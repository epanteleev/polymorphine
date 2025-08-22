#include "Elf.h"

#include <ostream>

Elf64_Shdr elf_section_init(const char *name, int type, int flags, int link, int info, int addralign, int entsize) {
    Elf64_Shdr shdr{};
    shdr.sh_type = type;
    shdr.sh_flags = flags;
    shdr.sh_link = link;
    shdr.sh_info = info;
    shdr.sh_addralign = addralign;
    shdr.sh_entsize = entsize;
    return shdr;
}

Elf64_Ehdr Elf::elf_header_init() {
    Elf64_Ehdr ehdr{};
    ehdr.e_ident[EI_MAG0] = ELFMAG0;
    ehdr.e_ident[EI_MAG1] = ELFMAG1;
    ehdr.e_ident[EI_MAG2] = ELFMAG2;
    ehdr.e_ident[EI_MAG3] = ELFMAG3;
    ehdr.e_ident[EI_CLASS] = ELFCLASS64;
    ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
    ehdr.e_ident[EI_VERSION] = EV_CURRENT;
    ehdr.e_ident[EI_OSABI] = ELFOSABI_NONE;
    ehdr.e_ident[EI_ABIVERSION] = 0;
    ehdr.e_type = ET_REL; // Shared object file
    ehdr.e_machine = EM_X86_64; // AMD x86-64 architecture
    ehdr.e_version = EV_CURRENT;
    ehdr.e_ehsize = sizeof(Elf64_Ehdr);
    ehdr.e_phentsize = sizeof(Elf64_Phdr);
    ehdr.e_shentsize = sizeof(Elf64_Shdr);
    return ehdr;
}

void Elf::write(std::ostream &os) {
    const auto ehdr = elf_header_init();
    os.write(reinterpret_cast<const char*>(&ehdr), sizeof(Elf64_Ehdr));
    const auto text = elf_section_init(".text", SHT_PROGBITS, SHF_EXECINSTR | SHF_ALLOC, SHN_UNDEF, 0, 16, 0);
    os.write(reinterpret_cast<const char*>(&text), sizeof(Elf64_Shdr));
}
