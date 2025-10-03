#pragma once

#include <cstdint>
#include <expected>
#include <iosfwd>

#include "LIRValType.h"
#include "base/Attribute.h"
#include "lir/x64/lir_frwd.h"
#include "../asm/operand/AssignedVReg.h"
#include "../asm/operand/GPVReg.h"
#include "lir/x64/instruction/LIRUse.h"
#include "utility/Error.h"

/**
 * Represents an argument in the Low-Level Intermediate Representation (LIR).
 * This class encapsulates the index and size of the argument.
 */
class LIRArg final: public LIRUse {
public:
    explicit LIRArg(const std::uint32_t index, const LIRValType type, const AttributeSet attributes) noexcept:
        m_index(index),
        m_type(type),
        m_attributes(attributes) {}

    [[nodiscard]]
    std::uint32_t index() const noexcept { return m_index; }

    [[nodiscard]]
    LIRValType type() const noexcept { return m_type; }

    [[nodiscard]]
    AttributeSet attributes() const noexcept { return m_attributes; }

    void assign_reg(const AssignedVReg& reg) noexcept {
        assertion(m_assigned_reg.empty(), "should be");
        m_assigned_reg = reg;
    }

    [[nodiscard]]
    const AssignedVReg& assigned_reg() const noexcept {
        return m_assigned_reg;
    }

    static std::expected<LIRArg, Error> try_from(const LIRVal& val) noexcept;

    friend std::ostream& operator<<(std::ostream& os, const LIRArg& op) noexcept;

private:
    AssignedVReg m_assigned_reg;
    const std::uint32_t m_index;
    LIRValType m_type;
    AttributeSet m_attributes;
};

std::ostream& operator<<(std::ostream& os, const LIRArg& op) noexcept;