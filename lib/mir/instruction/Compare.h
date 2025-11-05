#pragma once

#include "ValueInstruction.h"
#include "mir/types/FlagType.h"

class Compare: public ValueInstruction {
public:
    Compare(const Value& lhs, const Value& rhs) noexcept:
        ValueInstruction(FlagType::flag(),{lhs, rhs}) {}

    [[nodiscard]]
    const Value& lhs() const {
        return m_values.at(0);
    }

    [[nodiscard]]
    const Value& rhs() const {
        return m_values.at(1);
    }
};