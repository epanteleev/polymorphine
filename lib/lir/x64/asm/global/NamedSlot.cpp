#include <ostream>

#include "NamedSlot.h"

void NamedSlot::print_description(std::ostream &os) const {
    os << m_name << ':' << std::endl;
    m_value->print_description(os);
}

std::ostream &operator<<(std::ostream &os, const NamedSlot &slot) {
    return os << '$' << slot.m_name;
}