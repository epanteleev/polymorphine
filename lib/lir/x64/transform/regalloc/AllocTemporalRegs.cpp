#include "AllocTemporalRegs.h"

#include "lir/x64/asm/visitors/GPBinaryVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryAddrVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryOutVisitor.h"
#include "lir/x64/asm/visitors/GPBinaryAddrVisitor.h"

#include "lir/x64/asm/EmptyEmitter.h"
#include "../../asm/operand/GPOp.h"
#include "lir/x64/asm/emitters/CMovGPEmit.h"
#include "lir/x64/asm/emitters/CmpGPEmit.h"
#include "lir/x64/asm/emitters/DivIntEmit.h"
#include "lir/x64/asm/emitters/DivUIntEmit.h"
#include "lir/x64/asm/emitters/LoadByIdxIntEmit.h"
#include "lir/x64/asm/emitters/MovByIdxIntEmit.h"
#include "lir/x64/asm/emitters/MovGPEmit.h"
#include "lir/x64/asm/emitters/StoreGPEmit.h"
#include "lir/x64/asm/emitters/StoreOnStackGPEmit.h"

namespace details {
    GPOp AllocTemporalRegs::convert_to_gp_op(const LIROperand &val) const {
        if (const auto vreg = val.as_vreg(); vreg.has_value()) {
            return vreg.value().assigned_reg().to_gp_op().value();
        }
        if (const auto cst = val.as_cst(); cst.has_value()) {
            return cst.value().value();
        }
        if (const auto slot = val.as_slot(); slot.has_value()) {
            const auto [symbol, _] = m_symbol_tab.add(slot.value()->name(), aasm::BindAttribute::INTERNAL);
            return aasm::Address(symbol);
        }

        die("Invalid LIROperand");
    }

    void AllocTemporalRegs::div_i(const std::span<LIRVal const> outs, const LIROperand &in1, const LIROperand &in2) {
        const auto out_reg = outs[0].assigned_reg().to_gp_op().value();
        const auto in1_reg = convert_to_gp_op(in1);
        const auto in2_reg = convert_to_gp_op(in2);

        EmptyEmitter empty_emitter;
        DivIntEmit emitter(m_temp_counter, empty_emitter, in1.size());
        emitter.apply(out_reg, in1_reg, in2_reg);
    }

    void AllocTemporalRegs::div_u(std::span<LIRVal const> outs, const LIROperand &in1, const LIROperand &in2) {
        const auto out_reg = outs[0].assigned_reg().to_gp_op().value();
        const auto in1_reg = convert_to_gp_op(in1);
        const auto in2_reg = convert_to_gp_op(in2);

        EmptyEmitter empty_emitter;
        DivUIntEmit emitter(m_temp_counter, empty_emitter, in1.size());
        emitter.apply(out_reg, in1_reg, in2_reg);
    }

    void AllocTemporalRegs::cmov_i(aasm::CondType cond_type, const LIRVal &out, const LIROperand &in1, const LIROperand &in2) {
        const auto out_reg = out.assigned_reg().to_gp_op().value();
        const auto in1_reg = convert_to_gp_op(in1);
        const auto in2_reg = convert_to_gp_op(in2);
        EmptyEmitter empty_emitter;
        CMovGPEmit emitter(m_temp_counter, empty_emitter, cond_type, out.size());
        emitter.apply(out_reg, in1_reg, in2_reg);
    }

    void AllocTemporalRegs::cmp_i(const LIROperand &in1, const LIROperand &in2) {
        const auto in1_reg = convert_to_gp_op(in1);
        const auto in2_reg = convert_to_gp_op(in2);

        EmptyEmitter empty_emitter;
        CmpGPEmit emitter(m_temp_counter, empty_emitter, in1.size());
        emitter.apply(in1_reg, in2_reg);
    }

    void AllocTemporalRegs::mov_i(const LIROperand &in1, const LIROperand &in2) {
        const auto in1_reg = convert_to_gp_op(in1);
        const auto add_opt = in1_reg.as_address();
        assertion(add_opt.has_value(), "Invalid LIRVal for mov_i");

        const auto in2_op = convert_to_gp_op(in2);
        EmptyEmitter empty_emitter;
        MovGPEmit emitter(m_temp_counter, empty_emitter, in1.size());
        emitter.apply(add_opt.value(), in2_op);
    }

    void AllocTemporalRegs::mov_by_idx_i(const LIRVal &pointer, const LIROperand &index, const LIROperand &in) {
        const auto out_reg = pointer.assigned_reg().to_gp_op().value();
        const auto index_op = convert_to_gp_op(index);
        const auto in2_op = convert_to_gp_op(in);

        EmptyEmitter empty_emitter;
        MovByIdxIntEmit emitter(m_temp_counter, empty_emitter, in.size());
        emitter.apply(out_reg, index_op, in2_op);
    }

    void AllocTemporalRegs::store_by_offset_i(const LIROperand &pointer, const LIROperand &index, const LIROperand &value) {
        const auto out_vreg = convert_to_gp_op(pointer);
        const auto out_addr = out_vreg.as_address();
        assertion(out_addr.has_value(), "Invalid LIRVal for store_on_stack_i");

        const auto index_op = convert_to_gp_op(index);
        const auto value_op = convert_to_gp_op(value);

        EmptyEmitter empty_emitter;
        StoreOnStackGPEmit emitter(m_temp_counter, empty_emitter, value.size());
        emitter.apply(out_addr.value(), index_op, value_op);
    }

    void AllocTemporalRegs::store_i(const LIRVal &pointer, const LIROperand &value) {
        const auto pointer_reg = pointer.assigned_reg().to_gp_op().value();
        const auto value_op = convert_to_gp_op(value);

        EmptyEmitter empty_emitter;
        StoreGPEmit emitter(m_temp_counter, empty_emitter, value.size());
        emitter.apply(pointer_reg, value_op);
    }

    void AllocTemporalRegs::load_by_idx_i(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) {
        const auto out_reg = out.assigned_reg().to_gp_op().value();
        const auto index_op = convert_to_gp_op(index);
        const auto pointer_op = convert_to_gp_op(pointer);

        EmptyEmitter empty_emitter;
        LoadByIdxIntEmit emitter(m_temp_counter, empty_emitter, out.size());
        emitter.apply(out_reg, pointer_op, index_op);
    }
}
