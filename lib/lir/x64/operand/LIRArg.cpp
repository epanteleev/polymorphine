#include "base/Prefix.h"
#include "lir/x64/operand/LIRArg.h"
#include "lir/x64/operand/LIRVal.h"

#include <ostream>


std::expected<LIRArg, Error> LIRArg::try_from(const LIRVal &val) noexcept {
    const auto arg = val.arg();
    if (!arg.has_value()) {
        return std::unexpected(Error::CastError);
    }

    return LIRArg{val.index(), val.size(), arg.value()->attributes()};
}

std::ostream & operator<<(std::ostream &os, const LIRArg &op) noexcept {
    return os << "arg[" << op.m_index << '\'' << size_prefix(op.size()) << ']' << op.m_attributes;
}