#include "MachFunctionCodegen.h"

#include "emitters/AddIntEmit.h"
#include "emitters/CmpGPEmit.h"
#include "emitters/CopyGPEmit.h"
#include "emitters/LoadGPEmit.h"
#include "emitters/MovGPEmit.h"
#include "emitters/StoreGPEmit.h"

static aasm::CondType cvt_from(const LIRCondType cond) {
    return static_cast<aasm::CondType>(cond);
}

GPOp MachFunctionCodegen::convert_to_gp_op(const LIROperand &val) const {
    if (const auto vreg = val.vreg(); vreg.has_value()) {
        return m_reg_allocation[vreg.value()];
    }
    if (const auto cst = val.cst(); cst.has_value()) {
        return cst.value().value();
    }

    die("Invalid LIROperand");
}

aasm::GPReg MachFunctionCodegen::convert_to_gp_reg(const LIRVal &val) const {
    const auto allocation = m_reg_allocation[val];
    const auto gp_reg = allocation.as_gp_reg();
    assertion(gp_reg.has_value(), "Invalid GPVReg for LIRVal");
    return gp_reg.value();
}

void MachFunctionCodegen::add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) {
    const auto out_reg = m_reg_allocation[out];
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);
    AddIntEmit::emit(m_as, out.size(), out_reg, in1_reg, in2_reg);
}

void MachFunctionCodegen::setcc_i(const LIRVal &out, const LIRCondType cond_type, const LIROperand &in1) {
    const auto out_reg = m_reg_allocation[out];
    const auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, aasm::GPReg>) {
            m_as.setcc(cvt_from(cond_type), val);

        } else if constexpr (std::is_same_v<T, aasm::Address>) {

        } else {
            static_assert(false, "Unsupported type in setcc_i");
        }
    };

    out_reg.visit(visitor);
}

void MachFunctionCodegen::cmp_i(const LIROperand &in1, const LIROperand &in2) {
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);
    CmpGPEmit::emit(m_as, in1.size(), in1_reg, in2_reg);
}

void MachFunctionCodegen::mov_i(const LIRVal &in1, const LIROperand &in2) {
    const auto in1_reg = m_reg_allocation[in1];
    const auto add_opt = in1_reg.as_address();
    assertion(add_opt.has_value(), "Invalid LIRVal for mov_i");

    const auto in2_op = convert_to_gp_op(in2);
    MovGPEmit::emit(m_as, in1.size(), add_opt.value(), in2_op);
}

void MachFunctionCodegen::store_i(const LIRVal &pointer, const LIROperand &value) {
    const auto pointer_reg = m_reg_allocation[pointer];
    const auto value_op = convert_to_gp_op(value);
    StoreGPEmit::emit(m_as, value.size(), pointer_reg, value_op);
}

void MachFunctionCodegen::copy_i(const LIRVal &out, const LIROperand &in) {
    const auto out_reg = m_reg_allocation[out];
    CopyGPEmit::emit(m_as, out.size(), out_reg, convert_to_gp_op(in));
}

void MachFunctionCodegen::load_i(const LIRVal &out, const LIRVal &pointer) {
    const auto out_reg = m_reg_allocation[out];
    const auto pointer_reg = m_reg_allocation[pointer];
    LoadGPEmit::emit(m_as, out.size(), out_reg, pointer_reg);
}

void MachFunctionCodegen::jmp(const LIRBlock *bb) {
    const auto target = m_bb_labels.at(bb);
    m_as.jmp(target);
}

void MachFunctionCodegen::jcc(const LIRCondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) {
    const auto target_false = m_bb_labels.at(on_false);
    const auto cond = aasm::invert(cvt_from(cond_type));
    m_as.jcc(cond, target_false);
}

void MachFunctionCodegen::ret(const std::span<LIRVal const> ret_values) {
#ifdef ENABLE_ASSERTIONS
    const auto values_num = ret_values.size();
    if (values_num == 1) {
        const auto ret_val = convert_to_gp_reg(ret_values[0]);
        assertion(ret_val == aasm::rax, "Return value must be in rax register");
    }
#endif
    m_as.ret();
}
