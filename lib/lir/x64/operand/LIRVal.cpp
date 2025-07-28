#include <ostream>

#include "LIRVal.h"
#include "LIROperand.h"

#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/instruction/LIRProducerInstruction.h"

static char size_prefix(std::size_t size) {
    switch (size) {
        case 1: return 'b';
        case 2: return 'w';
        case 4: return 'd';
        case 8: return 'q';
        default: die("wrong size {}", size);
    }
}

std::expected<LIRVal, Error> LIRVal::try_from(const LIROperand &op) {
    const auto vreg = op.vreg();
    if (!vreg.has_value()) {
        return std::unexpected(Error::CastError);
    }

    return vreg.value();
}

std::span<LIRVal const> LIRVal::try_from(const LIRInstructionBase *inst) noexcept {
    if (const auto producer = dynamic_cast<const LIRProducerInstructionBase*>(inst)) {
        return producer->defs();
    }
    if (const auto call = dynamic_cast<const LIRCall*>(inst)) {
        return call->defs();
    }

    return {};
}

std::size_t LIRVal::id() const noexcept {
    switch (m_type) {
        case Op::Arg: return m_variant.m_arg->index();
        case Op::Inst: return m_variant.m_inst->id();
        case Op::Call: return m_variant.m_call->id();
        default: std::unreachable();
    }
}

std::ostream & operator<<(std::ostream &os, const LIRVal &op) noexcept {
    const auto idx = static_cast<std::size_t>(op.m_index);
    if (auto arg_opt = op.arg(); arg_opt.has_value()) {
        os << "arg " << '[' << idx << '\'' << size_prefix(op.size()) << ']';
        return os;
    }
    if (const auto inst = op.inst(); inst.has_value()) {
        os << inst.value()->owner()->id() << 'x' << op.id() << '-' << idx << '\'' << size_prefix(op.size());
        return os;
    }

    die("wrong variant");
}
