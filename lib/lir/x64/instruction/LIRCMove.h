#pragma once

#include "lir/x64/instruction/LIRProducerInstructionBase.h"


class LIRCMove final: public LIRProducerInstructionBase {
public:
    explicit LIRCMove(const LIRCondType cond_type, std::vector<LIROperand> &&uses) noexcept:
        LIRProducerInstructionBase(std::move(uses)),
        m_cond_type(cond_type) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRCMove> cmov(LIRCondType cond_type, const LIROperand &src, const LIROperand &dest) {
        auto cmov = std::make_unique<LIRCMove>(cond_type, std::vector{src, dest});
        cmov->add_def(LIRVal::reg(src.size(), 0, cmov.get()));
        return cmov;
    }

private:
    LIRCondType m_cond_type;
};
