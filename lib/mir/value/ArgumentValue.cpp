#include <iostream>

#include "mir/value/ArgumentValue.h"
#include "mir/instruction/Instruction.h"

void ArgumentValue::print(std::ostream &os) const {
    os << "%arg" << m_index;
}

void ArgumentValue::add_user(const Instruction *user) {
    m_used_in.push_back(user);
}