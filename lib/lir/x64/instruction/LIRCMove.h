#pragma once

#include "lir/x64/instruction/LIRProducerInstructionBase.h"


class LIRCMove final: public LIRProducerInstructionBase {
public:
    explicit LIRCMove(const aasm::CondType cond_type, std::vector<LIROperand> &&uses) noexcept:
        LIRProducerInstructionBase(std::move(uses)),
        m_cond_type(cond_type) {}

    void visit(LIRVisitor &visitor) override {
        visitor.cmov_i(m_cond_type, def(0), in(0), in(1));
    }

    static std::unique_ptr<LIRCMove> cmov(aasm::CondType cond_type, const LIROperand &src, const LIROperand &dest) {
        auto cmov = std::make_unique<LIRCMove>(cond_type, std::vector{src, dest});
        cmov->add_def(LIRVal::reg(src.size(), 0, cmov.get()));
        return cmov;
    }

private:
    aasm::CondType m_cond_type;
};
