#pragma once

#include <ostream>

namespace elf {
    class elf_header final {
    public:
        explicit elf_header() {
            header.e_ident[EI_MAG0] = ELFMAG0;
            header.e_ident[EI_MAG1] = ELFMAG1;
            header.e_ident[EI_MAG2] = ELFMAG2;
            header.e_ident[EI_MAG3] = ELFMAG3;
            header.e_ident[EI_CLASS] = ELFCLASS64;
            header.e_ident[EI_DATA] = ELFDATA2LSB;
            header.e_ident[EI_VERSION] = EV_CURRENT;
            header.e_version = EV_CURRENT;
            header.e_ehsize = sizeof(header);
            header.e_shstrndx = 1;
            header.e_phentsize = sizeof(Elf64_Phdr);
            header.e_shentsize = sizeof(Elf64_Shdr);
        }

        /**
         * @brief Save ELF header to stream.
         * @param stream Output stream.
         * @return True if successful, false otherwise.
         */
        bool save(std::ostream &stream) const {
            stream.write(reinterpret_cast<const char *>(&header),
                         sizeof(header));

            return stream.good();
        }

        //------------------------------------------------------------------------------
        // ELF header functions
        ELFIO_GET_ACCESS(unsigned char, class, header.e_ident[EI_CLASS]);
        ELFIO_GET_ACCESS(unsigned char, elf_version, header.e_ident[EI_VERSION]);
        ELFIO_GET_ACCESS(unsigned char, encoding, header.e_ident[EI_DATA]);
        ELFIO_GET_ACCESS(Elf_Half, header_size, header.e_ehsize);
        ELFIO_GET_ACCESS(Elf_Half, section_entry_size, header.e_shentsize);
        ELFIO_GET_ACCESS(Elf_Half, segment_entry_size, header.e_phentsize);

        ELFIO_GET_SET_ACCESS(Elf_Word, version, header.e_version);
        ELFIO_GET_SET_ACCESS(unsigned char, os_abi, header.e_ident[EI_OSABI]);
        ELFIO_GET_SET_ACCESS(unsigned char,
                             abi_version,
                             header.e_ident[EI_ABIVERSION]);
        ELFIO_GET_SET_ACCESS(Elf_Half, type, header.e_type);
        ELFIO_GET_SET_ACCESS(Elf_Half, machine, header.e_machine);
        ELFIO_GET_SET_ACCESS(Elf_Word, flags, header.e_flags);
        ELFIO_GET_SET_ACCESS(Elf_Half, section_name_str_index, header.e_shstrndx);
        ELFIO_GET_SET_ACCESS(Elf64_Addr, entry, header.e_entry);
        ELFIO_GET_SET_ACCESS(Elf_Half, sections_num, header.e_shnum);
        ELFIO_GET_SET_ACCESS(Elf64_Off, sections_offset, header.e_shoff);
        ELFIO_GET_SET_ACCESS(Elf_Half, segments_num, header.e_phnum);
        ELFIO_GET_SET_ACCESS(Elf64_Off, segments_offset, header.e_phoff);

    private:
        Elf64_Ehdr header = {};
    };
}
