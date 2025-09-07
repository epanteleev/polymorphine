#pragma once
#include "ValueInstruction.h"
#include "mir/types/TupleType.h"

class Projection final: public ValueInstruction {
public:
    Projection(const PrimitiveType* type, const Value& operand, const std::uint8_t idx) noexcept:
        ValueInstruction(type, {operand}),
        m_idx(idx) {}

    [[nodiscard]]
    Value operand() const {
        return m_values.at(0);
    }

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<Projection> proj(const Value& operand, const std::uint8_t idx) {
        const auto type = dynamic_cast<const TupleType*>(operand.type());
        assertion(type != nullptr, "expected");
        return std::make_unique<Projection>(type->inner_type(idx), operand, idx);
    }

private:
    const std::uint8_t m_idx;
};
