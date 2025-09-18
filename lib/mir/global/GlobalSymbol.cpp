#include "GlobalSymbol.h"

#include <ostream>

std::ostream& operator<<(std::ostream& os, const GlobalConstant& sym) {
    return os << '$' << sym.name();
}

void GlobalConstant::print_description(std::ostream &os) const {
    os << '$' << m_name << " = constant " << *m_type << m_value;
}