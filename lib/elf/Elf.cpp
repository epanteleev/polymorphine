#include "Elf.h"

#include <bits/ostream.h>


void elf_section_init(Elf64_Shdr* shdr, const char *name, int type, int flags, int link, int info, int addralign, int entsize) {
    shdr->sh_type = type;
    shdr->sh_flags = flags;
    shdr->sh_link = link;
    shdr->sh_info = info;
    shdr->sh_addralign = addralign;
    shdr->sh_entsize = entsize;
}


Elf Elf::create() {
    elf_section section{};

    Elf elf{section};

    section.shstrtab = 0;
    elf_section_init(&elf.shdr[0], ".shstrtab", SHT_STRTAB, 0, SHN_UNDEF, 0, 1, 0);

    section.strtab = 1;
    elf_section_init(&elf.shdr[1], ".strtab", SHT_STRTAB, 0, SHN_UNDEF, 0, 1, 0);

    section.symtab = 2;
    elf_section_init(&elf.shdr[2], ".symtab", SHT_SYMTAB, 0, section.strtab, 0, 4, sizeof(Elf64_Sym));

    section.bss = 3;
    elf_section_init(&elf.shdr[3], ".bss", SHT_NOBITS, SHF_WRITE | SHF_ALLOC, SHN_UNDEF, 0, 4, 0);

    section.rodata = 4;
    elf_section_init(&elf.shdr[4], ".rodata", SHT_PROGBITS, SHF_ALLOC, SHN_UNDEF, 0, 16, 0);

    section.data = 5;
    elf_section_init(&elf.shdr[5], ".data", SHT_PROGBITS, SHF_WRITE | SHF_ALLOC, SHN_UNDEF, 0, 4, 0);

    section.rela_data = 6;
    elf_section_init(&elf.shdr[6], ".rela.data", SHT_RELA, 0, section.symtab, section.data, 8, sizeof(Elf64_Rela));

    section.text = 7;
    elf_section_init(&elf.shdr[7], ".text", SHT_PROGBITS, SHF_EXECINSTR | SHF_ALLOC, SHN_UNDEF, 0, 16, 0);

    section.rela_text = 8;
    elf_section_init(&elf.shdr[8], ".rela.text", SHT_RELA, 0, section.symtab, section.text, 8, sizeof(Elf64_Rela));

    return elf;
}

void Elf::write(std::ostream &os) const {
    os.write(reinterpret_cast<const char *>(&header), sizeof(Elf64_Ehdr));
}
