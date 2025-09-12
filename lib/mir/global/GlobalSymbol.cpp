#include "GlobalSymbol.h"

#include <ostream>

std::ostream& operator<<(std::ostream& os, const GlobalConstant& sym) {
    return os << '$' << sym.name();
}

void GlobalConstant::print_description(std::ostream &os) const {
    const auto vis = [&]<typename T>(const T& val) {
        os << val;
    };
    os << '$' << m_name << " = constant " << *m_type << '{';
    std::visit(vis, m_value);
    os << '}';
}