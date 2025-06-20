#include <iostream>

#include "types/Type.h"
#include "FunctionPrototype.h"
#include "value/ArgumentValue.h"

void FunctionPrototype::print(std::ostream &os) const {
    os << m_name << '(';
    for (size_t i = 0; i < m_arg_types.size(); ++i) {
        if (i > 0) {
            os << ", ";
        }
        m_arg_types[i]->print(os);
    }
    os << "): ";
    m_ret_type->print(os);
}

void FunctionPrototype::print(std::ostream &os, const std::span<const ArgumentValue> args) const {
    os << m_name << '(';
    for (size_t i = 0; i < m_arg_types.size(); ++i) {
        if (i > 0) {
            os << ", ";
        }
        m_arg_types[i]->print(os);
        os << ": ";
        args[i].print(os);
    }
    os << "): ";
    m_ret_type->print(os);
}
