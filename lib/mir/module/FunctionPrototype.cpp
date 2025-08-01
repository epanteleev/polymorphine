#include <ostream>

#include "mir/types/Type.h"
#include "mir/value/Value.h"
#include "FunctionPrototype.h"

#include <utility>

#include "mir/value/ArgumentValue.h"

std::ostream & operator<<(std::ostream &os, const FunctionLinkage &linkage) {
    switch (linkage) {
        case FunctionLinkage::EXTERN: return os << "extern";
        case FunctionLinkage::INTERNAL: return os << "internal";
        default: std::unreachable();
    }
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

std::ostream & operator<<(std::ostream &os, const FunctionPrototype &proto) {
    os << proto.m_name << '(';
    for (size_t i = 0; i < proto.m_arg_types.size(); ++i) {
        if (i > 0) {
            os << ", ";
        }
        os << *proto.m_arg_types[i];
    }

    return os << "): " << *proto.m_ret_type;
}
