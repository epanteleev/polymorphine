#pragma once

#include "LIRInstructionBase.h"

enum class LIRInstKind: std::uint8_t {
    Add,
    Sub,
    Mul,
    Div,
    And,
    Or,
    Xor,
    Shl,
    Shr,
    Neg,
    Not,
    ParallelCopy,
    Mov,
    Copy,
    Cmp,
};

class LIRInstruction final: public LIRInstructionBase {
public:
    LIRInstruction(const std::size_t id, LIRBlock *bb, const LIRInstKind kind, std::vector<LIROperand>&& uses, std::vector<LIRVal>&& defs) :
        LIRInstructionBase(id, bb, std::move(uses), std::move(defs)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

    static LIRInstBuilder<LIRInstruction> copy(const LIROperand &op)  {
        return [=](std::size_t id, LIRBlock *bb) {
            auto copy = std::make_unique<LIRInstruction>(id, bb, LIRInstKind::Copy, std::vector{op}, std::vector<LIRVal>{});
            copy->add_def(LIRVal::reg(op.size(), 0, copy.get()));
            return copy;
        };
    }

    static LIRInstBuilder<LIRInstruction> add(const LIROperand &lhs, const LIROperand &rhs) {
        return [=](std::size_t id, LIRBlock *bb) {
            auto add = std::make_unique<LIRInstruction>(id, bb, LIRInstKind::Add, std::vector{lhs, rhs}, std::vector<LIRVal>{});
            add->add_def(LIRVal::reg(lhs.size(), 0, add.get()));
            return add;
        };
    }

    static LIRInstBuilder<LIRInstruction> cmp(const LIROperand &lhs, const LIROperand &rhs) {
        return [=](std::size_t id, LIRBlock *bb) {
            auto cmp = std::make_unique<LIRInstruction>(id, bb, LIRInstKind::Cmp, std::vector{lhs, rhs}, std::vector<LIRVal>{});
            cmp->add_def(LIRVal::reg(1, 0, cmp.get()));
            return cmp;
        };
    }

    [[nodiscard]]
    LIRInstKind kind() const noexcept {
        return m_kind;
    }

private:
    const LIRInstKind m_kind;
};
