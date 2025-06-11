#pragma once
#include "ValueInstruction.h"


class Alloc final: public ValueInstruction {
public:
    Alloc(std::size_t id, BasicBlock *bb, const NonTrivialType *ty) noexcept :
        ValueInstruction(id, bb, PointerType::ptr(), {}),
        m_type(ty) {}


    void visit(Visitor &visitor) override { visitor.accept(this); }

    static InstructionBuilder<Alloc> alloc(const NonTrivialType* ty) {
        return [&](std::size_t id, BasicBlock *bb) {
            return std::make_unique<Alloc>(id, bb, ty);
        };
    }

    [[nodiscard]]
    const NonTrivialType* allocated_type() const noexcept { return m_type; }

private:
    const NonTrivialType *m_type;
};
