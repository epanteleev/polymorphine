#pragma once

#include <expected>
#include <span>
#include <variant>

#include "../mir_frwd.h"

template<typename T>
concept IsTerminator = std::derived_from<T, TerminateInstruction> ||
    std::derived_from<T, TerminateValueInstruction>;

class Terminator final {
    explicit Terminator(TerminateInstruction * inst) noexcept;
    explicit Terminator(TerminateValueInstruction * inst) noexcept;

public:
    [[nodiscard]]
    std::span<BasicBlock* const> targets() const noexcept;

    static std::expected<Terminator, Error> from(Instruction* inst) noexcept;

    template<IsTerminator T>
    [[nodiscard]]
    T* as() const noexcept {
        if constexpr (std::is_base_of_v<TerminateInstruction, T>) {
            const auto v = *std::get_if<TerminateInstruction*>(&m_value);
            return dynamic_cast<T*>(v);

        } else if constexpr (std::is_base_of_v<TerminateValueInstruction, T>) {
            const auto v = *std::get_if<TerminateValueInstruction*>(&m_value);
            return dynamic_cast<T*>(v);

        } else {
            static_assert(false, "somthing was wrong");
            return nullptr;
        }
    }

    template<IsTerminator T>
    [[nodiscard]]
    bool is() const noexcept {
        if constexpr (std::is_base_of_v<TerminateInstruction, T>) {
            return std::holds_alternative<TerminateInstruction *>(m_value) &&
                   dynamic_cast<T *>(std::get<TerminateInstruction *>(m_value)) != nullptr;

        } else if constexpr (std::is_base_of_v<TerminateValueInstruction, T>) {
            return std::holds_alternative<TerminateValueInstruction *>(m_value) &&
                   dynamic_cast<T *>(std::get<TerminateValueInstruction *>(m_value)) != nullptr;

        } else {
            static_assert(false, "somthing was wrong");
            return false;
        }
    }


private:
    std::variant<TerminateInstruction *,
        TerminateValueInstruction *> m_value;
};
