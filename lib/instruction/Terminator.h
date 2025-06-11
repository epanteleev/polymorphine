#pragma once

#include <expected>
#include <span>
#include <variant>

#include "ir_frwd.h"
#include "utility/Error.h"

class Terminator final {
public:
    explicit Terminator(TerminateInstruction * inst) noexcept;
    explicit Terminator(TerminateValueInstruction * inst) noexcept;

    [[nodiscard]]
    std::span<BasicBlock*> targets() const noexcept;

    static std::expected<Terminator, Error> from(Instruction* inst) noexcept;

private:
    std::variant<TerminateInstruction *,
        TerminateValueInstruction *> m_value;
};
