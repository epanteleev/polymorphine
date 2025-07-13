
#include <ranges>
#include "LIRModule.h"

std::ostream & LIRModule::print(std::ostream &os) const {
    for (const auto &f: m_functions | std::views::values) {
        f->print(os);
    }

    return os;
}
