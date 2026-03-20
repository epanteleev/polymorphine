#pragma once

#include <expected>
#include <span>
#include <variant>

#include "mir/mir_frwd.h"
#include "mir/instruction/Instruction.h"

template<typename T>
concept IsTerminator = std::derived_from<T, TerminateInstruction> ||
    std::derived_from<T, TerminateValueInstruction>;

class Terminator final {
    explicit Terminator(const TerminateInstruction * inst) noexcept;
    explicit Terminator(const TerminateValueInstruction * inst) noexcept;

public:
    [[nodiscard]]
    std::span<BasicBlock* const> targets() const noexcept;

    template<InstructionMatcher Fn>
    [[nodiscard]]
    bool isa(Fn&& matcher) const noexcept {
        return std::visit(matcher, m_value);
    }

    template<typename T>
    const T* get() const noexcept {
        if constexpr (std::derived_from<T, TerminateInstruction>) {
            return dynamic_cast<const T*>(std::get<const TerminateInstruction*>(m_value));

        } else if constexpr (std::derived_from<T, TerminateValueInstruction>) {
            return dynamic_cast<const T*>(std::get<const TerminateValueInstruction*>(m_value));

        } else {
            static_assert(false);
            std::unreachable();
        }
    }

    [[nodiscard]]
    static std::expected<Terminator, Error> from(const Instruction* inst) noexcept;

private:
    std::variant<const TerminateInstruction *,
        const TerminateValueInstruction *> m_value;
};