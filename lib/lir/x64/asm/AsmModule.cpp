#include "AsmModule.h"

#include <iomanip>


std::ostream & operator<<(std::ostream &os, const AsmModule &module) {
    for (const auto &slot: module.m_global_slots | std::views::values) {
        slot.print_description(os);
    }

    for (const auto& [name, masm]: module.m_asm_buffers) {
        os << *name << ':' << std::endl;
        os << std::setfill(' ') << std::setw(4) << masm << std::endl << std::endl;
    }

    return os;
}
