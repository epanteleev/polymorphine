#pragma once

#include <cstdint>
#include <expected>
#include <iosfwd>
#include <span>
#include <vector>

#include "base/Attribute.h"
#include "lir/x64/lir_frwd.h"
#include "lir/x64/asm/GPVReg.h"
#include "utility/Error.h"

/**
 * Represents an argument in the Low-Level Intermediate Representation (LIR).
 * This class encapsulates the index and size of the argument.
 */
class LIRArg final {
public:
    explicit LIRArg(const std::uint32_t index, const std::uint8_t size, AttributeSet attributes) noexcept:
        m_index(index),
        m_size(size),
        m_attributes(attributes) {}

    [[nodiscard]]
    std::uint8_t size() const noexcept { return m_size; }

    [[nodiscard]]
    std::uint32_t index() const noexcept { return m_index; }

    [[nodiscard]]
    AttributeSet attributes() const noexcept { return m_attributes; }

    void add_user(LIRInstructionBase *inst) noexcept {
        m_used_in.push_back(inst);
    }

    void kill_user(LIRInstructionBase *inst) noexcept {
        std::erase(m_used_in, inst);
    }

    void assign_reg(const OptionalGPVReg& reg) noexcept {
        m_assigned_reg = reg;
    }

    const OptionalGPVReg& assigned_reg() const noexcept {
        return m_assigned_reg;
    }

    [[nodiscard]]
    std::span<LIRInstructionBase * const> users() const noexcept {
        return m_used_in;
    }

    static std::expected<LIRArg, Error> try_from(const LIRVal& val) noexcept;

    friend std::ostream& operator<<(std::ostream& os, const LIRArg& op) noexcept;

private:
    const std::uint32_t m_index;
    const std::uint8_t m_size;
    OptionalGPVReg m_assigned_reg;
    std::vector<LIRInstructionBase *> m_used_in;
    AttributeSet m_attributes;
};

std::ostream& operator<<(std::ostream& os, const LIRArg& op) noexcept;