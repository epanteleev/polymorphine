
#include <ostream>
#include <ranges>
#include "ObjModule.h"

std::ostream & ObjModule::print(std::ostream &os) const {
    for (const auto &f: m_functions | std::views::values) {
        f->print(os);
    }

    return os;

}
