#include <ostream>

#include "LIRVReg.h"
#include "LIROperand.h"

#include "lir/x64/module/MachBlock.h"
#include "lir/x64/instruction/LIRInstruction.h"

static char size_prefix(std::size_t size) {
    switch (size) {
        case 1: return 'b';
        case 2: return 'w';
        case 4: return 'l';
        case 8: return 'q';
        default: die("wrong size {}", size);
    }
}

std::expected<LIRVReg, Error> LIRVReg::try_from(const LIROperand &op) {
    const auto vreg = op.vreg();
    if (!vreg.has_value()) {
        return std::unexpected(Error::BadCastError);
    }

    return vreg.value();
}

std::ostream & operator<<(std::ostream &os, const LIRVReg &op) noexcept {
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
