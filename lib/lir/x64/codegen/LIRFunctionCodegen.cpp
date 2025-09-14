#include "LIRFunctionCodegen.h"

#include "asm/x64/reg/RegSet.h"
#include "lir/x64/asm/MasmEmitter.h"
#include "lir/x64/asm/visitors/GPBinarySrcAddrVisitor.h"
#include "lir/x64/asm/visitors/GPBinaryVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryOutVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryAddrVisitor.h"
#include "lir/x64/asm/visitors/GPBinaryAddrVisitor.h"

#include "lir/x64/asm/emitters/AddIntEmit.h"
#include "lir/x64/asm/emitters/CmpGPEmit.h"
#include "lir/x64/asm/emitters/CopyGPEmit.h"
#include "lir/x64/asm/emitters/LoadGPEmit.h"
#include "lir/x64/asm/emitters/MovGPEmit.h"
#include "lir/x64/asm/emitters/StoreGPEmit.h"
#include "lir/x64/asm/emitters/SubIntEmit.h"
#include "lir/x64/asm/emitters/CMovGPEmit.h"
#include "lir/x64/asm/emitters/XorIntEmit.h"
#include "lir/x64/asm/emitters/MovsxIntEmit.h"
#include "lir/x64/asm/emitters/MovzxIntEmit.h"
#include "lir/x64/asm/emitters/TruncIntEmit.h"
#include "lir/x64/asm/emitters/LeaGPEmit.h"

#include "lir/x64/asm/cc/CallConv.h"

#include "lir/x64/instruction/LIRCall.h"
#include "lir/x64/instruction/LIRInstructionBase.h"
#include "lir/x64/asm/emitters/DivIntEmit.h"
#include "lir/x64/asm/emitters/DivUIntEmit.h"
#include "lir/x64/asm/emitters/LoadByIdxIntEmit.h"
#include "lir/x64/asm/emitters/LoadFromStackGPEmit.h"
#include "lir/x64/asm/emitters/LoadStackAddrGPEmit.h"
#include "lir/x64/asm/emitters/MovByIdxIntEmit.h"
#include "lir/x64/asm/emitters/StoreOnStackGPEmit.h"
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

static aasm::Linkage cvt_linkage(const FunctionVisibility linkage) noexcept {
    switch (linkage) {
        case FunctionVisibility::DEFAULT:  return aasm::Linkage::DEFAULT;
        case FunctionVisibility::INTERNAL: return aasm::Linkage::INTERNAL;
        case FunctionVisibility::EXTERN:   return aasm::Linkage::EXTERNAL;
        default: die("Unsupported LIRLinkage type");
    }
}

GPOp LIRFunctionCodegen::convert_to_gp_op(const LIROperand &val) const {
    if (const auto vreg = val.as_vreg(); vreg.has_value()) {
        return vreg.value().assigned_reg().to_gp_op().value();
    }
    if (const auto cst = val.as_cst(); cst.has_value()) {
        return cst.value().value();
    }
    if (const auto slot = val.as_slot(); slot.has_value()) {
        const auto [symbol, _] = m_symbol_tab.add(slot.value()->name(), aasm::Linkage::INTERNAL);
        return aasm::Address(symbol);
    }

    die("Invalid LIROperand");
}

void LIRFunctionCodegen::add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);
    AddIntEmit::apply(m_as, out.size(), out_reg, in1_reg, in2_reg);
}

void LIRFunctionCodegen::sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);
    SubIntEmit::apply(m_as, out.size(), out_reg, in1_reg, in2_reg);
}

void LIRFunctionCodegen::div_i(const std::span<LIRVal const> out, const LIROperand &in1, const LIROperand &in2) {
    const auto out_reg = out[0].assigned_reg().to_gp_op().value();
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);

    DivIntEmit emitter(m_current_inst->temporal_regs(), m_as, in1.size());
    emitter.apply(out_reg, in1_reg, in2_reg);
}

void LIRFunctionCodegen::div_u(std::span<LIRVal const> out, const LIROperand &in1, const LIROperand &in2) {
    const auto out_reg = out[0].assigned_reg().to_gp_op().value();
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);

    DivUIntEmit emitter(m_current_inst->temporal_regs(), m_as, in1.size());
    emitter.apply(out_reg, in1_reg, in2_reg);
}

void LIRFunctionCodegen::xor_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);
    XorIntEmit emitter(m_as, out.size());
    emitter.apply(out_reg, in1_reg, in2_reg);
}

void LIRFunctionCodegen::setcc_i(const LIRVal &out, const aasm::CondType cond_type) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
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
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);
    CMovGPEmit emitter(m_current_inst->temporal_regs(), m_as, cond_type, out.size());
    emitter.apply(out_reg, in1_reg, in2_reg);
}

void LIRFunctionCodegen::cmp_i(const LIROperand &in1, const LIROperand &in2) {
    const auto in1_reg = convert_to_gp_op(in1);
    const auto in2_reg = convert_to_gp_op(in2);
    CmpGPEmit::apply(m_as, in1.size(), in1_reg, in2_reg);
}

void LIRFunctionCodegen::mov_i(const LIRVal &in1, const LIROperand &in2) {
    const auto in1_reg = in1.assigned_reg().to_gp_op().value();
    const auto add_opt = in1_reg.as_address();
    assertion(add_opt.has_value(), "Invalid LIRVal for mov_i");

    const auto in2_op = convert_to_gp_op(in2);
    MovGPEmit::apply(m_as, in1.size(), add_opt.value(), in2_op);
}

void LIRFunctionCodegen::mov_by_idx_i(const LIRVal &out, const LIROperand &index, const LIROperand &in) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto index_op = convert_to_gp_op(index);
    const auto in2_op = convert_to_gp_op(in);

    MovByIdxIntEmit emitter(m_current_inst->temporal_regs(), m_as, in.size());
    emitter.apply(out_reg, index_op, in2_op);
}

void LIRFunctionCodegen::store_on_stack_i(const LIRVal &pointer, const LIROperand &index, const LIROperand &value) {
    const auto out_vreg = pointer.assigned_reg().to_gp_op().value();
    const auto out_addr = out_vreg.as_address();
    assertion(out_addr.has_value(), "Invalid LIRVal for store_on_stack_i");

    const auto index_op = convert_to_gp_op(index);
    const auto value_op = convert_to_gp_op(value);
    StoreOnStackGPEmit emitter(m_current_inst->temporal_regs(), m_as, value.size());
    emitter.apply(out_addr.value(), index_op, value_op);
}

void LIRFunctionCodegen::load_by_idx_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto index_op = convert_to_gp_op(index);
    const auto pointer_op = convert_to_gp_op(pointer);

    LoadByIdxIntEmit emitter(m_as, out.size());
    emitter.apply(out_reg, pointer_op, index_op);
}

void LIRFunctionCodegen::load_from_stack_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto index_op = convert_to_gp_op(index);
    const auto pointer_op = pointer.assigned_reg().to_gp_op().value();
    const auto pointer_addr = pointer_op.as_address();
    assertion(pointer_addr.has_value(), "Invalid LIRVal for load_from_stack_i");

    LoadFromStackGPEmit emitter(m_current_inst->temporal_regs(), m_as, out.size());
    emitter.apply(out_reg, index_op, pointer_addr.value());
}

void LIRFunctionCodegen::load_stack_addr_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto index_op = convert_to_gp_op(index);
    const auto pointer_op = pointer.assigned_reg().to_gp_op().value();
    const auto pointer_addr = pointer_op.as_address();
    assertion(pointer_addr.has_value(), "Invalid LIRVal for load_stack_addr_i");

    LoadStackAddrGPEmit emitter(m_current_inst->temporal_regs(), m_as, out.size());
    emitter.apply(out_reg, index_op, pointer_addr.value());
}

void LIRFunctionCodegen::store_i(const LIRVal &pointer, const LIROperand &value) {
    const auto pointer_reg = pointer.assigned_reg().to_gp_op().value();
    const auto value_op = convert_to_gp_op(value);
    StoreGPEmit::apply(m_as, value.size(), pointer_reg, value_op);
}

void LIRFunctionCodegen::up_stack(const aasm::GPRegSet& reg_set, const std::size_t caller_overflow_area_size, const std::size_t local_area_size) {
    for (const auto &reg: reg_set) {
        m_as.pop(8, reg);
    }

    auto size_to_adjust = caller_overflow_area_size;
    if (const auto remains = (local_area_size+caller_overflow_area_size+reg_set.size()*8) % call_conv::STACK_ALIGNMENT; remains != 0L) {
        size_to_adjust += remains;
    }
    if (size_to_adjust != 0) {
        m_as.add(8, aasm::checked_cast<std::int32_t>(size_to_adjust), aasm::rsp);
    }
}

void LIRFunctionCodegen::down_stack(const aasm::GPRegSet& reg_set, const std::size_t caller_overflow_area_size, const std::size_t local_area_size) {
    auto size_to_adjust = caller_overflow_area_size;
    if (const auto remains = (local_area_size+caller_overflow_area_size+reg_set.size()*8) % call_conv::STACK_ALIGNMENT; remains != 0L) {
        size_to_adjust += remains;
    }
    if (size_to_adjust != 0) {
        m_as.sub(8, aasm::checked_cast<std::int32_t>(size_to_adjust), aasm::rsp);
    }

    for (auto rev: std::ranges::reverse_view(reg_set)) {
        m_as.push(8, rev);
    }
}

void LIRFunctionCodegen::prologue(const aasm::GPRegSet &reg_set, const std::size_t caller_overflow_area_size, const std::size_t local_area_size) {
    if (reg_set.empty() && caller_overflow_area_size == 0 && local_area_size == 0) {
        return;
    }

    m_as.push(8, aasm::rbp);
    m_as.copy(8, aasm::rsp, aasm::rbp);
    m_as.sub(8, aasm::checked_cast<std::int32_t>(local_area_size), aasm::rsp);
    for (const auto& reg: reg_set) {
        m_as.push(8, reg);
    }
}

void LIRFunctionCodegen::epilogue(const aasm::GPRegSet &reg_set, const std::size_t caller_overflow_area_size, const std::size_t local_area_size) {
    if (reg_set.empty() && caller_overflow_area_size == 0 && local_area_size == 0) {
        return;
    }

    for (const auto& reg: std::ranges::reverse_view(reg_set)) {
        m_as.pop(8, reg);
    }

    m_as.leave();
}

void LIRFunctionCodegen::copy_i(const LIRVal &out, const LIROperand &in) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    CopyGPEmit::apply(m_as, out.size(), out_reg, convert_to_gp_op(in));
}

void LIRFunctionCodegen::load_i(const LIRVal &out, const LIRVal &pointer) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto pointer_reg = pointer.assigned_reg().to_gp_op().value();
    LoadGPEmit::apply(m_as, out.size(), out_reg, pointer_reg);
}

void LIRFunctionCodegen::lea_i(const LIRVal &out, const LIROperand &pointer, const LIROperand &index) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto index_op = convert_to_gp_op(index);
    const auto pointer_op = convert_to_gp_op(pointer);

    LeaStackAddrGPEmit emitter(m_current_inst->temporal_regs(), m_as, out.size());
    emitter.apply(out_reg, index_op, pointer_op.as_address().value());
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

void LIRFunctionCodegen::movsx_i(const LIRVal &out, const LIROperand &in) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto pointer_reg = convert_to_gp_op(in);
    MovsxGPEmit emitter(m_as, out.size(), in.size());
    emitter.apply(out_reg, pointer_reg);
}

void LIRFunctionCodegen::movzx_i(const LIRVal &out, const LIROperand &in) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto pointer_reg = convert_to_gp_op(in);
    MovzxGPEmit emitter(m_as, out.size(), in.size());
    emitter.apply(out_reg, pointer_reg);
}

void LIRFunctionCodegen::trunc_i(const LIRVal &out, const LIROperand &in) {
    const auto out_reg = out.assigned_reg().to_gp_op().value();
    const auto pointer_reg = convert_to_gp_op(in);
    TruncGPEmit emitter(m_as, out.size(), in.size());
    emitter.emit(out_reg, pointer_reg);
}

void LIRFunctionCodegen::call(const LIRVal &out, const std::string_view name, std::span<LIRVal const> args, const FunctionVisibility linkage) {
    const auto [symbol, _] = m_symbol_tab.add(name, cvt_linkage(linkage));
    m_as.call(symbol);
}

void LIRFunctionCodegen::ret(const std::span<LIRVal const> ret_values) {
    m_as.ret();
}