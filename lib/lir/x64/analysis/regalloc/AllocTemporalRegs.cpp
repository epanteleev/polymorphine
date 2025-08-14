#include "AllocTemporalRegs.h"

#include "lir/x64/asm/EmptyEmitter.h"
#include "lir/x64/asm/GPOp.h"
#include "lir/x64/asm/emitters/CMovGPEmit.h"

namespace details {
    GPOp AllocTemporalRegs::convert_to_gp_op(const LIROperand &val) const {
        if (const auto vreg = val.vreg(); vreg.has_value()) {
            return m_reg_allocation.at(vreg.value());
        }
        if (const auto cst = val.cst(); cst.has_value()) {
            return cst.value().value();
        }

        die("Invalid LIROperand");
    }

    void AllocTemporalRegs::cmov_i(aasm::CondType cond_type, const LIRVal &out, const LIROperand &in1, const LIROperand &in2) {
        const auto out_reg = m_reg_allocation.at(out);
        const auto in1_reg = convert_to_gp_op(in1);
        const auto in2_reg = convert_to_gp_op(in2);
        EmptyEmitter empty_emitter;
        CMovGPEmit emitter( m_temp_counter, empty_emitter, cond_type, out.size());
        emitter.emit(out_reg, in1_reg, in2_reg);
    }
}
