#include <ranges>
#include "LIRModule.h"

#include <ostream>

std::ostream & operator<<(std::ostream &os, const LIRModule &module) {
    for (const auto &val: module.m_global_data | std::views::values) {
        val.print_description(os);
        os << std::endl;
    }

    for (const auto &f: module.m_functions | std::views::values) {
        f.print(os);
    }

    return os;
}