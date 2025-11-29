#pragma once
#include "Projection.h"
#include "ValueInstruction.h"
#include "mir/types/TupleType.h"


class IntDiv final: public ValueInstruction {
public:
    IntDiv(const Value &lhs, const Value &rhs) noexcept:
        ValueInstruction(make_ty(lhs, rhs), {lhs, rhs}) {}

    [[nodiscard]]
    const Value& lhs() const {
        return m_values[0];
    }

    [[nodiscard]]
    const Value& rhs() const {
        return m_values[1];
    }

    [[nodiscard]]
    const Projection* quotient() const noexcept {
        return dynamic_cast<const Projection*>(m_users[0]);
    }

    [[nodiscard]]
    const Projection* remain() const noexcept {
        return dynamic_cast<const Projection*>(m_users[1]);
    }

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<IntDiv> div(const Value &lhs, const Value &rhs) {
        return std::make_unique<IntDiv>(lhs, rhs);
    }

private:
    static const TupleType *make_ty(const Value &lhs, const Value &rhs) {
        const auto lhs_type = dynamic_cast<const PrimitiveType*>(lhs.type());
        assertion(lhs_type != nullptr, "expected");
        const auto rhs_type = dynamic_cast<const PrimitiveType*>(rhs.type());
        assertion(rhs_type != nullptr, "expected");
        return TupleType::tuple(lhs_type, rhs_type);
    }
};
