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
    LIRInstruction(const std::size_t id, MachBlock *bb, const LIRInstKind kind, std::vector<LIROperand>&& uses, std::vector<LIRVReg>&& defs) :
        LIRInstructionBase(id, bb, std::move(uses), std::move(defs)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

    static LIRInstBuilder<LIRInstruction> copy(const LIROperand& op);

    [[nodiscard]]
    LIRInstKind kind() const noexcept {
        return m_kind;
    }

private:
    const LIRInstKind m_kind;
};
