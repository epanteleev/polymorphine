#include "LIRFunctionCodegen.h"

#include "emitters/AddIntEmit.h"
#include "emitters/CmpGPEmit.h"
#include "emitters/CopyGPEmit.h"
#include "emitters/LoadGPEmit.h"
#include "emitters/MovGPEmit.h"
#include "emitters/StoreGPEmit.h"
#include "emitters/SubIntEmit.h"
#include "asm/reg/RegSet.h"

static aasm::CondType cvt_from(const LIRCondType cond) noexcept {
    return static_cast<aasm::CondType>(cond);
}

static aasm::Linkage cvt_linkage(const LIRLinkage linkage) noexcept {
    switch (linkage) {
        case LIRLinkage::INTERNAL: return aasm::Linkage::INTERNAL;
        case LIRLinkage::EXTERNAL: return aasm::Linkage::EXTERNAL;
        default: die("Unsupported LIRLinkage type");
    }
}

static bool is_no_prologue(const RegisterAllocation& reg_allocation) noexcept {
    return reg_allocation.local_area_size() == 0 &&
           reg_allocation.used_callee_saved_regs().empty();
}

GPOp LIRFunctionCodegen::convert_to_gp_op(const LIROperand &val) const {
    if (const auto vreg = val.vreg(); vreg.has_value()) {
        return m_reg_allocation[vreg.value()];
    }
    if (const auto cst = val.cst(); cst.has_value()) {
        return cst.value().value();
    }

    die("Invalid LIROperand");
}

aasm::GPReg LIRFunctionCodegen::convert_to_gp_reg(const LIRVal &val) const {
    const auto allocation = m_reg_allocation[val];
    const auto gp_reg = allocation.as_gp_reg();
    assertion(gp_reg.has_value(), "Invalid GPVReg for LIRVal");
    return gp_reg.value();
}

void LIRFunctionCodegen::add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) {
    const auto out_reg = m_reg_allocation[out];
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);
    AddIntEmit::emit(m_as, out.size(), out_reg, in1_reg, in2_reg);
}

void LIRFunctionCodegen::sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) {
    const auto out_reg = m_reg_allocation[out];
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);
    SubIntEmit::emit(m_as, out.size(), out_reg, in1_reg, in2_reg);
}

void LIRFunctionCodegen::setcc_i(const LIRVal &out, const LIRCondType cond_type) {
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

void LIRFunctionCodegen::cmov_i(LIRCondType cond_type, const LIRVal& out, const LIROperand& in1, const LIROperand& in2) {
}

void LIRFunctionCodegen::cmp_i(const LIROperand &in1, const LIROperand &in2) {
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);
    CmpGPEmit::emit(m_as, in1.size(), in1_reg, in2_reg);
}

void LIRFunctionCodegen::mov_i(const LIRVal &in1, const LIROperand &in2) {
    const auto in1_reg = m_reg_allocation[in1];
    const auto add_opt = in1_reg.as_address();
    assertion(add_opt.has_value(), "Invalid LIRVal for mov_i");

    const auto in2_op = convert_to_gp_op(in2);
    MovGPEmit::emit(m_as, in1.size(), add_opt.value(), in2_op);
}

void LIRFunctionCodegen::store_i(const LIRVal &pointer, const LIROperand &value) {
    const auto pointer_reg = m_reg_allocation[pointer];
    const auto value_op = convert_to_gp_op(value);
    StoreGPEmit::emit(m_as, value.size(), pointer_reg, value_op);
}

void LIRFunctionCodegen::up_stack(const aasm::GPRegSet& reg_set, const std::size_t caller_overflow_area_size) {
    for (const auto &reg: reg_set) {
        m_as.pop(8, reg);
    }

    auto size_to_adjust = caller_overflow_area_size;
    if (const auto remains = (m_reg_allocation.frame_size()+caller_overflow_area_size+reg_set.size()*8) % call_conv::STACK_ALIGNMENT; remains != 0L) {
        size_to_adjust += remains;
    }
    if (size_to_adjust != 0) {
        m_as.add(8, size_to_adjust, aasm::rsp);
    }
}

void LIRFunctionCodegen::down_stack(const aasm::GPRegSet& reg_set, const std::size_t caller_overflow_area_size) {
    auto size_to_adjust = caller_overflow_area_size;
    if (const auto remains = (m_reg_allocation.frame_size()+caller_overflow_area_size+reg_set.size()*8) % call_conv::STACK_ALIGNMENT; remains != 0L) {
        size_to_adjust += remains;
    }
    if (size_to_adjust != 0) {
        m_as.sub(8, size_to_adjust, aasm::rsp);
    }

    for (auto rev = reg_set.rbegin(); rev != reg_set.rend(); --rev) {
        m_as.push(8, *rev);
    }
}

void LIRFunctionCodegen::prologue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size) {
    if (is_no_prologue(m_reg_allocation) && caller_overflow_area_size == 0) {
        return;
    }

    m_as.push(8, aasm::rbp);
    m_as.copy(8, aasm::rsp, aasm::rbp);
    m_as.sub(8, m_reg_allocation.local_area_size(), aasm::rsp);
    for (const auto& reg: m_reg_allocation.used_callee_saved_regs()) {
        m_as.push(8, reg);
    }
}

void LIRFunctionCodegen::epilogue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size) {
    if (is_no_prologue(m_reg_allocation) && caller_overflow_area_size == 0) {
        return;
    }

    for (const auto& reg: std::ranges::reverse_view(m_reg_allocation.used_callee_saved_regs())) {
        m_as.pop(8, reg);
    }

    m_as.leave();
}

void LIRFunctionCodegen::copy_i(const LIRVal &out, const LIROperand &in) {
    const auto out_reg = m_reg_allocation[out];
    CopyGPEmit::emit(m_as, out.size(), out_reg, convert_to_gp_op(in));
}

void LIRFunctionCodegen::load_i(const LIRVal &out, const LIRVal &pointer) {
    const auto out_reg = m_reg_allocation[out];
    const auto pointer_reg = m_reg_allocation[pointer];
    LoadGPEmit::emit(m_as, out.size(), out_reg, pointer_reg);
}

void LIRFunctionCodegen::jmp(const LIRBlock *bb) {
    const auto target = m_bb_labels.at(bb);
    m_as.jmp(target);
}

void LIRFunctionCodegen::jcc(const LIRCondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) {
    const auto target_false = m_bb_labels.at(on_false);
    const auto cond = aasm::invert(cvt_from(cond_type));
    m_as.jcc(cond, target_false);
}

void LIRFunctionCodegen::call(const LIRVal &out, const std::string_view name, std::span<LIRVal const> args, LIRLinkage linkage) {
    const auto [symbol, _] = m_symbol_tab.add(name, cvt_linkage(linkage));
    m_as.call(symbol);
}

void LIRFunctionCodegen::ret(const std::span<LIRVal const> ret_values) {
#ifndef NDEBUG
    const auto values_num = ret_values.size();
    if (values_num == 1) {
        const auto ret_val = convert_to_gp_reg(ret_values[0]);
        assertion(ret_val == aasm::rax, "Return value must be in rax register");
    }
#endif
    m_as.ret();
}
