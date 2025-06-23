#pragma once

#include <cstdint>
#include <iosfwd>

using Elf64_Addr = std::size_t;      // Unsigned program address, uintN_t
using Elf64_Off = std::size_t;       // Unsigned file offset, uintN_t
using Elf64_Section = std::uint16_t; // Unsigned section index, uintN_t
using Elf64_Versym = std::uint16_t;  // Unsigned version symbol information, uint16_t

using Elf64_Xword = std::size_t;

using Elf64_Word = std::uint32_t;
using Elf64_Half = std::uint16_t;

using Elf64_Sxword = std::int64_t;
using Elf64_Sword = std::int32_t;

struct Elf64_Ehdr {
    unsigned char   e_ident[16];    /* ELF identification. */
    Elf64_Half      e_type;         /* Object file type. */
    Elf64_Half      e_machine;      /* Machine type. */
    Elf64_Word      e_version;      /* Object file version. */
    Elf64_Addr      e_entry;        /* Entry point address. */
    Elf64_Off       e_phoff;        /* Program header offset. */
    Elf64_Off       e_shoff;        /* Section header offset. */
    Elf64_Word      e_flags;        /* Processor-specific flags. */
    Elf64_Half      e_ehsize;       /* ELF header size. */
    Elf64_Half      e_phentsize;    /* Size of program header entry. */
    Elf64_Half      e_phnum;        /* Program header entry count. */
    Elf64_Half      e_shentsize;    /* Size of section header entry. */
    Elf64_Half      e_shnum;        /* Section header entry count. */
    Elf64_Half      e_shstrndx;     /* Section name strtab index. */
};


static constexpr auto ELFCLASS64 = 2;    /* 64-bit objects. */
static constexpr auto ELFDATA2LSB = 1;   /* Little-endian data structures. */
static constexpr auto EV_CURRENT = 1;    /* Current ELF version. */
static constexpr auto ELFOSABI_SYSV = 0; /* System V ABI. */
static constexpr auto ET_REL = 1;        /* Relocatable file. */

struct Elf64_Shdr {
    Elf64_Word      sh_name;        /* Section name. */
    Elf64_Word      sh_type;        /* Section type. */
    Elf64_Xword     sh_flags;       /* Section attributes. */
    Elf64_Addr      sh_addr;        /* Virtual address in memory. */
    Elf64_Off       sh_offset;      /* Offset in file. */
    Elf64_Xword     sh_size;        /* Size of section. */
    Elf64_Word      sh_link;        /* Link to other section. */
    Elf64_Word      sh_info;        /* Miscellaneous information. */
    Elf64_Xword     sh_addralign;   /* Address alignment boundary. */
    Elf64_Xword     sh_entsize;     /* Size of entries. */
};

#define SHN_UNDEF 0
#define SHN_ABS 0xFFF1              /* Absolute value reference. */
#define SHN_COMMON 0xFFF2           /* Tentative definitions. */

/* Section types, sh_type. */
#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8                /* Uninitialized space. */
#define SHT_DYNSYM 11

/* Section attributes, sh_flags. */
#define SHF_WRITE 0x1
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4


struct Elf64_Sym{
    Elf64_Word      st_name;        /* Symbol name. */
    unsigned char   st_info;        /* Type and Binding attributes. */
    unsigned char   st_other;       /* Reserved. */
    Elf64_Half      st_shndx;       /* Section table index. */
    Elf64_Addr      st_value;       /* Symbol value. */
    Elf64_Xword     st_size;        /* Size of object. */
};

#define STB_LOCAL 0
#define STB_GLOBAL 1

#define STT_NOTYPE 0
#define STT_OBJECT 1
#define STT_FUNC 2
#define STT_SECTION 3
#define STT_FILE 4

struct Elf64_Rela {
    Elf64_Addr      r_offset;       /* Address of reference. */
    Elf64_Xword     r_info;         /* Symbol index and reloc type. */
    Elf64_Sxword    r_addend;       /* Constant part of expression. */
} ;

enum rel_type {
    R_X86_64_NONE = 0,
    R_X86_64_64 = 1,                /* word64   S + A. */
    R_X86_64_PC32 = 2,              /* word32   S + A - P */
    R_X86_64_PLT32 = 4,             /* word32   L + A - P */
    R_X86_64_GOTPCREL = 9           /* word32   G + GOT + A - P */
};

#define ELF64_R_INFO(s, t) ((((long) s) << 32) + (((long) t) & 0xFFFFFFFFL))

struct elf_section {
    int shstrtab;
    int strtab;
    int symtab;
    int bss;
    int rodata;
    int data;
    int rela_data;
    int text;
    int rela_text;
    int debug_info;
    int rela_debug_info;
    int debug_abbrev;
};

class Elf final {
    static constexpr std::size_t SHNUM_MAX = 13;

    Elf(const elf_section& section) noexcept:
        section(section) {}

public:
    static Elf create();


    void write(std::ostream &os) const;
private:
    Elf64_Ehdr header{
        {
            '\x7f', 'E', 'L', 'F',  /* El_MAG */
            ELFCLASS64,             /* El_CLASS */
            ELFDATA2LSB,            /* El_DATA */
            EV_CURRENT,             /* El_VERSION */
            ELFOSABI_SYSV,          /* El_OSABI */
            0,                      /* El_ABIVERSION */
            0,                      /* El_PAD */
            sizeof(header.e_ident)  /* El_NIDENT */
        },
        ET_REL,             /* Relocatable file type. */
        0x3E,               /* Machine type x86_64. */
        1,                  /* Version. */
        0x0,                /* Entry point address. */
        0x0,                /* Program header offset. */
        sizeof(header),     /* Section header offset. */
        0x0,                /* Flags. */
        sizeof(header),
        0x0,                /* Program header size. */
        0,                  /* Number of program header entries. */
        sizeof(Elf64_Shdr), /* e_shentsize. */
        8,                  /* e_shnum, number of section headers. (TODO) */
        0                   /* e_shstrndx, index of shstrtab. (TODO) */
    };

    elf_section section{};
    Elf64_Shdr shdr[SHNUM_MAX]{};
};
