#include <ostream>

#include "Vreg.h"

#include "LIROperand.h"
#include "Utils.h"

std::expected<VReg, Error> VReg::from(const LIROperand &op) {
    const auto vreg = op.vreg();
    if (!vreg.has_value()) {
        return std::unexpected(Error::BadCastError);
    }

    return vreg.value();
}

std::ostream & operator<<(std::ostream &os, const VReg &op) noexcept {
    if (auto arg_opt = op.arg(); arg_opt.has_value()) {
        os << "arg " << size_prefix(op.size()) << '[' << op.m_index << "]";
        return os;
    }
    if (auto inst = op.inst(); inst.has_value()) {
        os << size_prefix(op.size()) << '[' << op.m_index << "]";
        return os;
    }

    die("wrong variant");
}
