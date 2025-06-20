#include "module/Module.h"

#include <ranges>

std::ostream &Module::print(std::ostream &os) const {
    for (const auto &f: m_functions | std::views::values) {
        f->print(os);
    }

    return os;
}
