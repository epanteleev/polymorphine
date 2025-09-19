#include <ostream>

#include "lir/x64/global/LIRNamedSlot.h"


void LIRNamedSlot::print_description(std::ostream &os) const {
    os << m_name << ':' << std::endl;
    m_value.print_description(os);
}

std::ostream &operator<<(std::ostream &os, const LIRNamedSlot &slot) {
    return os << '$' << slot.m_name;
}