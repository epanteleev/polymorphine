#include <ostream>

#include "mir/value/ArgumentValue.h"
#include "mir/instruction/Instruction.h"

static_assert(!std::is_polymorphic_v<Value>, "should be");

std::ostream & operator<<(std::ostream &os, const ArgumentValue &args) {
    return os << "%arg" << args.m_index << ' ' << args.m_attributes;
}
