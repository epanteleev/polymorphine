#include <ostream>

#include "LIRVal.h"
#include "LIROperand.h"

#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/instruction/LIRProducerInstruction.h"
#include "lir/x64/instruction/LIRCall.h"

static char size_prefix(std::size_t size) {
    switch (size) {
        case 1: return 'b';
        case 2: return 'w';
        case 4: return 'd';
        case 8: return 'q';
        default: return 'L';
    }
}

void LIRVal::add_user(LIRInstructionBase *inst) const noexcept {
    const auto vis = [&]<typename T>(T& val) {
        val.add_user(inst);
    };
    visit(vis);
}

void LIRVal::kill_user(LIRInstructionBase *inst) const noexcept {
    const auto vis = [&]<typename T>(T& val) {
        val.kill_user(inst);
    };
    visit(vis);
}

std::span<LIRInstructionBase * const> LIRVal::users() const noexcept {
    const auto vis = [&]<typename T>(const T& val) {
        return val.users();
    };
    return visit(vis);
}

std::expected<LIRVal, Error> LIRVal::try_from(const LIROperand &op) {
    const auto vreg = op.vreg();
    if (!vreg.has_value()) {
        return std::unexpected(Error::CastError);
    }

    return vreg.value();
}

std::span<LIRVal const> LIRVal::defs(const LIRInstructionBase *inst) noexcept {
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
        case Op::Arg:  return m_variant.m_arg->index();
        case Op::Inst: return m_variant.m_inst->id();
        case Op::Call: return m_variant.m_call->id();
        default: std::unreachable();
    }
}

std::ostream & operator<<(std::ostream &os, const LIRVal &op) noexcept {
    const auto idx = static_cast<std::size_t>(op.m_index);
    switch (op.m_type) {
        case LIRVal::Op::Arg: {
            return os << "arg " << '[' << idx << '\'' << size_prefix(op.size()) << ']';
        }
        case LIRVal::Op::Inst: [[fallthrough]];
        case LIRVal::Op::Call: {
            return os << op.m_variant.m_inst->owner()->id() << 'x' << op.id() << '-' << idx << '\'' << size_prefix(op.size());
        }
        default: std::unreachable();
    }
}
