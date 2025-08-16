#pragma once

#include <expected>
#include <span>
#include <variant>

#include "mir/mir_frwd.h"

template<typename T>
concept IsTerminator = std::derived_from<T, TerminateInstruction> ||
    std::derived_from<T, TerminateValueInstruction>;

class Terminator final {
    explicit Terminator(TerminateInstruction * inst) noexcept;
    explicit Terminator(TerminateValueInstruction * inst) noexcept;

public:
    [[nodiscard]]
    std::span<BasicBlock* const> targets() const noexcept;

    template<typename Fn>
    requires std::is_invocable_r_v<bool, Fn, const Instruction*>
    [[nodiscard]]
    bool isa(Fn&& matcher) const noexcept {
        return std::visit(matcher, m_value);
    }

    static std::expected<Terminator, Error> from(Instruction* inst) noexcept;

private:
    std::variant<TerminateInstruction *,
        TerminateValueInstruction *> m_value;
};