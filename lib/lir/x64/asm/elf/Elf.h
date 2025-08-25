#pragma once
#include "asm/elf/elfio.hpp"
#include "lir/x64/asm/AsmModule.h"

class Elf final {
public:
    void save(std::string_view path);

    static Elf collect(const AsmModule& module);

private:
    explicit Elf(elf::elfio&& writer) noexcept: m_writer(std::move(writer)) {}

    elf::elfio m_writer;
};
