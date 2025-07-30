#include <iostream>

#include "mir/value/ArgumentValue.h"
#include "mir/instruction/Instruction.h"

void ArgumentValue::add_user(const Instruction *user) {
    m_used_in.push_back(user);
}

std::ostream & operator<<(std::ostream &os, const ArgumentValue &args) {
    return os << "%arg" << args.m_index;
}
