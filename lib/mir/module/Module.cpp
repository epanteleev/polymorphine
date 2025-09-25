#include "mir/module/Module.h"

#include <ranges>

std::ostream & operator<<(std::ostream &os, const Module &module) {
    for (const auto& proto: std::ranges::views::values(module.m_prototypes)) {
        os << "declare " << proto << std::endl;
    }

    if (!module.m_prototypes.empty()) {
        os << std::endl;
    }

    for (const auto &s: std::ranges::views::values(module.m_known_structs)) {
        s.print_declaration(os);
    }

    if (!module.m_known_structs.empty()) {
        os << std::endl;
    }

    for (const auto &c: std::ranges::views::values(module.m_gvalue_pool)) {
        c.print_description(os);
        os << std::endl;
    }

    if (!module.m_gvalue_pool.empty()) {
        os << std::endl;
    }

    for (const auto &f: std::ranges::views::values(module.m_functions)) {
        os << f;
    }

    return os;
}