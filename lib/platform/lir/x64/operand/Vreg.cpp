#include <ostream>

#include "Vreg.h"

#include "LIROperand.h"
#include "../module/MachBlock.h"
#include "platform/lir/x64/instruction/LIRInstruction.h"

static char size_prefix(std::size_t size) {
    switch (size) {
        case 1: return 'b';
        case 2: return 'w';
        case 4: return 'l';
        case 8: return 'q';
        default: die("wrong size {}", size);
    }
}

std::expected<VReg, Error> VReg::try_from(const LIROperand &op) {
    const auto vreg = op.vreg();
    if (!vreg.has_value()) {
        return std::unexpected(Error::BadCastError);
    }

    return vreg.value();
}

std::ostream & operator<<(std::ostream &os, const VReg &op) noexcept {
    const auto idx = static_cast<std::size_t>(op.m_index);
    if (auto arg_opt = op.arg(); arg_opt.has_value()) {
        os << "arg " << '[' << idx << '\'' << size_prefix(op.size()) << ']';
        return os;
    }
    if (const auto inst = op.inst(); inst.has_value()) {
        os << inst.value()->owner()->id() << 'x' << idx << '\'' << size_prefix(op.size());
        return os;
    }

    die("wrong variant");
}
