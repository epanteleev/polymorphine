#include <ranges>
#include "LIRModule.h"


std::ostream & operator<<(std::ostream &os, const LIRModule &module) {
    for (const auto &[idx, val]: module.m_global_data | std::views::values | std::views::enumerate) {
        val.print_description(os);
    }
    if (!module.m_global_data.empty()) {
        os << std::endl;
    }

    for (const auto &f: module.m_functions | std::views::values) {
        f.print(os);
    }

    return os;
}