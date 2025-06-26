#pragma once

#include <cstdint>
#include <expected>

#include "LIRArg.h"
#include "mach_frwd.h"
#include "utility/Error.h"

class VReg final {
    template<typename T>
    VReg(std::uint8_t size, std::uint32_t index, T def):
        m_size(size),
        m_index(index),
        m_def(def) {}

public:
    [[nodiscard]]
    std::optional<LIRArg*> arg() const noexcept {
        if (std::holds_alternative<LIRArg*>(m_def)) {
            return std::get<LIRArg*>(m_def);
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::optional<LIRInstruction*> inst() const noexcept {
        if (std::holds_alternative<LIRInstruction*>(m_def)) {
            return std::get<LIRInstruction*>(m_def);
        }

        return std::nullopt;
    }

    [[nodiscard]]
    std::uint8_t size() const noexcept {
        return m_size;
    }

    static VReg reg(std::uint32_t index, LIRArg* def) noexcept {
        return {def->size(), index, def};
    }

    static VReg reg(std::uint8_t size, std::uint32_t index, LIRInstruction* def) noexcept {
        return {size, index, def};
    }

    static std::expected<VReg, Error> from(const LIROperand& op);

    friend std::ostream& operator<<(std::ostream& os, const VReg& op) noexcept;

private:
    std::uint8_t m_size;
    std::uint32_t m_index;
    std::variant<LIRInstruction*, LIRArg* > m_def;
};

std::ostream& operator<<(std::ostream& os, const VReg& op) noexcept;