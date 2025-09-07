#include "StructType.h"

#include <ostream>
#include <ranges>

void StructType::print_declaration(std::ostream &os) const {
    os << '$' << m_name << " = type {";
    for (const auto &[idx, ft]: std::ranges::enumerate_view(m_field_types)) {
        if (idx > 0) {
            os << ", ";
        }

        os << *ft;
    }

    os << '}';
}