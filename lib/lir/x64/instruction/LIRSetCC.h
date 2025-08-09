#pragma once

#include "LIRProducerInstructionBase.h"

class LIRSetCC final: public LIRProducerInstructionBase {
public:
    explicit LIRSetCC(LIRCondType cond_type) :
        LIRProducerInstructionBase({}),
        m_cond_type(cond_type) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRSetCC> setcc(LIRCondType cond_type) {
        auto setcc = std::make_unique<LIRSetCC>(cond_type);
        setcc->add_def(LIRVal::reg(1, 0, setcc.get()));
        return setcc;
    }

private:
    const LIRCondType m_cond_type;
};
