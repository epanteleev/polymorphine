#pragma once
#include "ValueInstruction.h"


class Alloc final: public ValueInstruction {
public:
    explicit Alloc(const NonTrivialType *ty) noexcept :
        ValueInstruction(PointerType::ptr(), {}),
        m_type(ty) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<Alloc> alloc(const NonTrivialType* ty) {
        return std::make_unique<Alloc>(ty);
    }

    [[nodiscard]]
    const NonTrivialType* allocated_type() const noexcept { return m_type; }

private:
    const NonTrivialType *m_type;
};
