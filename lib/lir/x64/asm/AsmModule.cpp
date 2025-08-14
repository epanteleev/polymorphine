#include "AsmModule.h"

#include <iomanip>

std::ostream & operator<<(std::ostream &os, const AsmModule &module) {
    for (const auto& [name, masm]: module.m_modules) {
        os << *name << ':' << std::endl;
        os << std::setfill(' ') << std::setw(4) << masm << std::endl << std::endl;
    }

    return os;
}
