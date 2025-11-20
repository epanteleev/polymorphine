#pragma once

#include "LIRInstruction.h"
#include "lir/x64/asm/FcmpOrdering.h"

class LIRFCmp final: public LIRInstructionBase {
public:
    explicit LIRFCmp(const FcmpOrdering cmp_type, std::vector<LIROperand>&& uses) noexcept:
        LIRInstructionBase(std::move(uses)),
        m_cmp_type(cmp_type) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRFCmp> cmp(const FcmpOrdering cmp_type, const LIROperand &lhs, const LIROperand &rhs) {
        return std::make_unique<LIRFCmp>(cmp_type, std::vector{lhs, rhs});
    }

private:
    const FcmpOrdering m_cmp_type;
};
