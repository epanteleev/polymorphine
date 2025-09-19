#include "Directive.h"

#include <ostream>

namespace aasm {
    void Directive::print_description(std::ostream &os) const {
        os << m_symbol->name() << ':' << std::endl;
        m_slot.print_description(os);
    }

    std::ostream &operator<<(std::ostream &os, const Directive& directive) {
        return os << '$' << directive.m_symbol->name();
    }
}