#include "Terminator.h"

#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/TerminateValueInstruction.h"
#include "utility/Error.h"

Terminator::Terminator(const TerminateInstruction *inst) noexcept: m_value(inst) {}
Terminator::Terminator(const TerminateValueInstruction *inst) noexcept: m_value(inst) {}

std::span<BasicBlock *const> Terminator::targets() const noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        return val->successors();
    };

    return std::visit(visitor, m_value);
}

std::expected<Terminator, Error> Terminator::from(const Instruction *inst) noexcept {
    if (const auto term = dynamic_cast<const TerminateInstruction *>(inst)) {
        return Terminator(term);
    }
    if (const auto term = dynamic_cast<const TerminateValueInstruction *>(inst)) {
        return Terminator(term);
    }

    return std::unexpected(Error::CastError);
}

