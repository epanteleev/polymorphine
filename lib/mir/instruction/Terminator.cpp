#include "Terminator.h"

#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/TerminateValueInstruction.h"
#include "utility/Error.h"

Terminator::Terminator(TerminateInstruction *inst) noexcept: m_value(inst) {}
Terminator::Terminator(TerminateValueInstruction *inst) noexcept: m_value(inst) {}

std::span<BasicBlock *const> Terminator::targets() const noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, TerminateInstruction *> || std::is_same_v<T, TerminateValueInstruction*>) {
            return val->successors();
        } else {
            static_assert(false, "Somthing was wrong");
        }
    };

    return std::visit(visitor, m_value);
}

std::expected<Terminator, Error> Terminator::from(Instruction *inst) noexcept {
    if (const auto term = dynamic_cast<TerminateInstruction *>(inst)) {
        return Terminator(term);
    }
    if (const auto term = dynamic_cast<TerminateValueInstruction *>(inst)) {
        return Terminator(term);
    }

    return std::unexpected(Error::CastError);
}

