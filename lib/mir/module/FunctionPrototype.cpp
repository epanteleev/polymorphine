#include <iostream>

#include "mir/types/Type.h"
#include "FunctionPrototype.h"
#include "mir/value/ArgumentValue.h"

void FunctionPrototype::print(std::ostream &os) const {
    os << m_name << '(';
    for (size_t i = 0; i < m_arg_types.size(); ++i) {
        if (i > 0) {
            os << ", ";
        }
        os << *m_arg_types[i] << ": ";
    }
    os << "): " << *m_ret_type;
}

void FunctionPrototype::print(std::ostream &os, const std::span<const ArgumentValue> args) const {
    os << m_name << '(';
    for (size_t i = 0; i < m_arg_types.size(); ++i) {
        if (i > 0) {
            os << ", ";
        }
        os << *m_arg_types[i] << ": ";
        args[i].print(os);
    }
    os << "): " << *m_ret_type;
}
