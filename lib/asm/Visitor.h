#pragma once
#include "asm/Instruction.h"

namespace aasm {
    class Acceptor final {
    public:
        explicit Acceptor(const Instruction& inst): m_instruction(inst) {}

        template<typename Visitor, typename T>
        T accept(Visitor& visitor) {
            switch (m_instruction.opcode) {
                case opc::RET: return visitor.ret();
                case opc::PUSH_R: return visitor.push_r(m_instruction.source.reg());
                case opc::PUSH_M: return visitor.push_m(m_instruction.source.mem());
                case opc::POP_R: return visitor.pop_r(m_instruction.dest.reg());
                case opc::POP_M: return visitor.pop_m(m_instruction.dest.mem());
                case opc::MOV_RR: return visitor.mov_rr(m_instruction.source.reg(), m_instruction.dest.reg());
                default: std::terminate();
            }
        }

    private:
        const Instruction& m_instruction;
    };
}
