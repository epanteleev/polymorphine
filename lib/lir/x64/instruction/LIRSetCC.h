#pragma once

#include "LIRProducerInstructionBase.h"
#include "asm/instruction/CondType.h"

class LIRSetCC final: public LIRProducerInstructionBase {
public:
    explicit LIRSetCC(const aasm::CondType cond_type) noexcept:
        LIRProducerInstructionBase({}),
        m_cond_type(cond_type) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRSetCC> setcc(aasm::CondType cond_type) {
        auto setcc = std::make_unique<LIRSetCC>(cond_type);
        setcc->add_def(LIRVal::reg(1, 0, setcc.get()));
        return setcc;
    }

private:
    const aasm::CondType m_cond_type;
};
