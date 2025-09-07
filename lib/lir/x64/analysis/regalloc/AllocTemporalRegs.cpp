#include "AllocTemporalRegs.h"

#include "lir/x64/asm/EmptyEmitter.h"
#include "lir/x64/asm/GPOp.h"
#include "lir/x64/asm/emitters/CMovGPEmit.h"
#include "lir/x64/asm/emitters/MovByIdxIntEmit.h"
#include "lir/x64/asm/emitters/StoreOnStackGPEmit.h"

namespace details {
    GPOp AllocTemporalRegs::convert_to_gp_op(const LIROperand &val) const {
        if (const auto vreg = val.as_vreg(); vreg.has_value()) {
            return m_reg_allocation.at(vreg.value());
        }
        if (const auto cst = val.as_cst(); cst.has_value()) {
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
        emitter.apply(out_reg, in1_reg, in2_reg);
    }

    void AllocTemporalRegs::mov_by_idx_i(const LIRVal &pointer, const LIROperand &index, const LIROperand &in) {
        const auto out_reg = m_reg_allocation.at(pointer);
        const auto index_op = convert_to_gp_op(index);
        const auto in2_op = convert_to_gp_op(in);

        EmptyEmitter empty_emitter;
        MovByIdxIntEmit emitter(m_temp_counter, empty_emitter, in.size());
        emitter.apply(out_reg, index_op, in2_op);
    }

    void AllocTemporalRegs::store_on_stack_i(const LIRVal &pointer, const LIROperand &index, const LIROperand &value) {
        const auto out_vreg = m_reg_allocation.at(pointer);
        const auto out_addr = out_vreg.as_address();
        assertion(out_addr.has_value(), "Invalid LIRVal for store_on_stack_i");

        const auto index_op = convert_to_gp_op(index);
        const auto value_op = convert_to_gp_op(value);

        EmptyEmitter empty_emitter;
        StoreOnStackGPEmit emitter(m_temp_counter, empty_emitter, value.size());
        emitter.apply(out_addr.value(), index_op, value_op);
    }
}
