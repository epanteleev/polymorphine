#include <iostream>

#include "ArgumentValue.h"

void ArgumentValue::print(std::ostream &os) const {
    os << "%arg" << m_index;
}
