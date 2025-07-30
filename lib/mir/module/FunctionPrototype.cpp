#include <iostream>

#include "mir/types/Type.h"
#include "mir/value/Value.h"
#include "FunctionPrototype.h"
#include "mir/value/ArgumentValue.h"

void FunctionPrototype::print(std::ostream &os) const {
    os << m_name << '(';
    for (size_t i = 0; i < m_arg_types.size(); ++i) {
        if (i > 0) {
            os << ", ";
        }
        os << *m_arg_types[i];
    }
    os << "): " << *m_ret_type;
}

template<std::ranges::range Args>
static void print_args(std::ostream &os, const FunctionPrototype& proto, Args&& args) {
    os << proto.name() << '(';
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) {
            os << ", ";
        }
        os << *proto.arg_type(i) << ": " << args[i];
    }
    os << ')';
}

void FunctionPrototype::print(std::ostream &os, const std::span<const ArgumentValue> args) const {
    print_args(os, *this, args);
    os << ": " << *ret_type();
}

void FunctionPrototype::print(std::ostream &os, std::span<const Value> args) const {
    print_args(os, *this, args);
}
