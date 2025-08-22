#pragma once

#include <iosfwd>
#include <elf.h>

class Elf final {
    static constexpr auto SHNUM_MAX = 13;

public:
    static Elf64_Ehdr elf_header_init();

    static void write(std::ostream& os);
};