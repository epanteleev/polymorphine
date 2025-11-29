#pragma once

#include "mir/instruction/ValueInstruction.h"

class Select final : public ValueInstruction {
public:
    explicit Select(const Type* ty, std::vector<Value>&& values) noexcept:
        ValueInstruction(ty, std::move(values)) {}

    [[nodiscard]]
    const Value& condition() const {
        return m_values[0];
    }

    [[nodiscard]]
    const Value& on_true() const {
        return m_values[1];
    }

    [[nodiscard]]
    const Value& on_false() const {
        return m_values[2];
    }

    void visit(Visitor &visitor) {
        visitor.accept(this);
    }

    static std::unique_ptr<Select> select(const Value& cond, const Value& true_val, const Value& false_val) {
        return std::make_unique<Select>(true_val.type(), std::vector{cond, true_val, false_val});
    }
};