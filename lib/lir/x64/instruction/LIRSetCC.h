#pragma once

#include <utility>
#include <iosfwd>

#include "LIRInstructionBase.h"
#include "LIRProducerInstructionBase.h"

enum class LIRCondType: std::uint8_t {
    O   = 0x0, // Overflow
    NO  = 0x1, // No Overflow
    NAE = 0x2, // Not Above or Equal (Below)
    AE  = 0x3, // Above or Equal (Not Below)
    E   = 0x4, // Equal (Zero)
    NE  = 0x5, // Not Equal (Not Zero)
    NA  = 0x6, // Not Above (Below or Equal)
    A   = 0x7, // Above (Not Below or Equal)
    S   = 0x8, // Sign (Negative)
    NS  = 0x9, // Not Sign (Positive)
    P   = 0xA, // Parity (Even)
    NP  = 0xB, // Not Parity (Odd)
    NGE = 0xC, // Not Greater or Equal (Less)
    GE  = 0xD, // Greater or Equal (Not Less)
    NG  = 0xE, // Not Greater (Less or Equal)
    G   = 0xF  // Greater (Not Less or Equal)
};

std::ostream& operator<<(std::ostream &os, const LIRCondType &cond);

class LIRSetCC final: public LIRProducerInstructionBase {
public:
    LIRSetCC(const std::size_t id, LIRBlock *bb, LIRCondType cond_type, std::vector<LIROperand>&& uses) :
        LIRProducerInstructionBase(id, bb, std::move(uses)),
        m_cond_type(cond_type) {}

    void visit(LIRVisitor &visitor) override;

    static LIRInstBuilder<LIRSetCC> setcc(LIRCondType cond_type, const LIROperand &op) {
        return [=](std::size_t id, LIRBlock *bb) {
            auto setcc = std::make_unique<LIRSetCC>(id, bb, cond_type, std::vector{op});
            setcc->add_def(LIRVal::reg(1, 0, setcc.get()));
            return setcc;
        };
    }

private:
    const LIRCondType m_cond_type;
};
