#pragma once
#include "lir/x64/asm/AsmModule.h"

class Elf final {
public:
    static Elf collect(const AsmModule& module);

private:

};
