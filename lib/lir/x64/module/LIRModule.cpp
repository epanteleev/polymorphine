
#include <ranges>
#include "LIRModule.h"

std::ostream & operator<<(std::ostream &os, const LIRModule &module) {
    for (const auto &f: module.m_functions | std::views::values) {
        f->print(os);
    }

    return os;
}
