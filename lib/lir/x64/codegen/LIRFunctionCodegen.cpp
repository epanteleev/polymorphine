#include "LIRFunctionCodegen.h"

#include "asm/reg/RegSet.h"
#include "lir/x64/asm/MasmEmitter.h"

#include "lir/x64/asm/emitters/AddIntEmit.h"
#include "lir/x64/asm/emitters/CmpGPEmit.h"
#include "lir/x64/asm/emitters/CopyGPEmit.h"
#include "lir/x64/asm/emitters/LoadGPEmit.h"
#include "lir/x64/asm/emitters/MovGPEmit.h"
#include "lir/x64/asm/emitters/StoreGPEmit.h"
#include "lir/x64/asm/emitters/SubIntEmit.h"
#include "lir/x64/asm/emitters/CMovGPEmit.h"
#include "lir/x64/asm/cc/CallConv.h"

#include "lir/x64/instruction/LIRCall.h"
#include "lir/x64/instruction/LIRInstructionBase.h"
#include "lir/x64/analysis/regalloc/RegisterAllocation.h"
#include "lir/x64/asm/emitters/LoadByIdxIntEmit.h"
#include "lir/x64/asm/emitters/MovByIdxIntEmit.h"
#include "lir/x64/operand/LIRVal.h"

void LIRFunctionCodegen::setup_basic_block_labels() {
    for (const auto& bb: m_preorder) {
        if (bb == m_data.first()) {
            // Skip the first basic block, it does not need a label.
            continue;
        }

        const auto label = m_as.create_label();
        m_bb_labels.emplace(bb, label);
    }
}

const TemporalRegs& LIRFunctionCodegen::temporal_reg(const LIRInstructionBase *inst) const  {
    if (const auto it = m_reg_allocation.try_get_temporal_regs(inst); it.has_value()) {
        return *it.value();
    }

    static TemporalRegs empty_clobber_regs;
    return empty_clobber_regs;
}

void LIRFunctionCodegen::traverse_instructions() {
    for (const auto [idx, bb]: std::ranges::views::enumerate(m_preorder)) {
        if (bb != m_data.first()) {
            m_as.set_label(m_bb_labels.at(bb));
        }
        
        if (static_cast<std::uint64_t>(idx) + 1 < m_preorder.size()) {
            m_next = m_preorder[idx + 1];
        }
        
        for (auto& inst: bb->instructions()) {
            m_current_inst = &inst;
            inst.visit(*this);
        }
    }
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

void LIRFunctionCodegen::setcc_i(const LIRVal &out, const aasm::CondType cond_type) {
    const auto out_reg = m_reg_allocation[out];
    const auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, aasm::GPReg>) {
            m_as.setcc(cond_type, val);

        } else if constexpr (std::is_same_v<T, aasm::Address>) {

        } else {
            static_assert(false, "Unsupported type in setcc_i");
        }
    };

    out_reg.visit(visitor);
}

void LIRFunctionCodegen::cmov_i(aasm::CondType cond_type, const LIRVal& out, const LIROperand& in1, const LIROperand& in2) {
    const auto out_reg = m_reg_allocation[out];
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);
    CMovGPEmit emitter(temporal_reg(m_current_inst), m_as, cond_type, out.size());
    emitter.emit(out_reg, in1_reg, in2_reg);
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

void LIRFunctionCodegen::mov_by_idx_i(const LIRVal &out, const LIROperand &index, const LIROperand &in) {
    const auto out_reg = m_reg_allocation[out];
    const auto index_op = convert_to_gp_op(index);
    const auto in2_op = convert_to_gp_op(in);

    MovByIdxIntEmit emitter(m_as, in.size());
    emitter.emit(out_reg, index_op, in2_op);
}

void LIRFunctionCodegen::load_by_idx_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) {
    const auto out_reg = m_reg_allocation[out];
    const auto index_op = convert_to_gp_op(index);
    const auto pointer_op = convert_to_gp_op(pointer);

    LoadByIdxIntEmit emitter(m_as, out.size());
    emitter.emit(out_reg, pointer_op, index_op);
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
        m_as.add(8, aasm::checked_cast<std::int32_t>(size_to_adjust), aasm::rsp);
    }
}

void LIRFunctionCodegen::down_stack(const aasm::GPRegSet& reg_set, const std::size_t caller_overflow_area_size) {
    auto size_to_adjust = caller_overflow_area_size;
    if (const auto remains = (m_reg_allocation.frame_size()+caller_overflow_area_size+reg_set.size()*8) % call_conv::STACK_ALIGNMENT; remains != 0L) {
        size_to_adjust += remains;
    }
    if (size_to_adjust != 0) {
        m_as.sub(8, aasm::checked_cast<std::int32_t>(size_to_adjust), aasm::rsp);
    }

    for (auto rev: std::ranges::reverse_view(reg_set)) {
        m_as.push(8, rev);
    }
}

void LIRFunctionCodegen::prologue(const aasm::GPRegSet &reg_set, const std::size_t caller_overflow_area_size) {
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

void LIRFunctionCodegen::epilogue(const aasm::GPRegSet &reg_set, const std::size_t caller_overflow_area_size) {
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

void LIRFunctionCodegen::lea_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) {

}

void LIRFunctionCodegen::jmp(const LIRBlock *bb) {
    if (m_next == bb) {
        return;
    }

    m_as.jmp(m_bb_labels.at(bb));
}

void LIRFunctionCodegen::jcc(const aasm::CondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) {
    const auto target_false = m_bb_labels.at(on_false);
    m_as.jcc(aasm::invert(cond_type), target_false);
}

void LIRFunctionCodegen::call(const LIRVal &out, const std::string_view name, std::span<LIRVal const> args, const LIRLinkage linkage) {
    const auto [symbol, _] = m_symbol_tab.add(name, cvt_linkage(linkage));
    m_as.call(symbol);
}

void LIRFunctionCodegen::ret(const std::span<LIRVal const> ret_values) {
    m_as.ret();
}