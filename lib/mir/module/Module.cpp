#include "mir/module/Module.h"

#include <ranges>

std::ostream & operator<<(std::ostream &os, const Module &module) {
    for (const auto &f: module.m_functions | std::views::values) {
        f->print(os);
    }

    return os;
}
