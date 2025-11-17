#include "LIRFunctionCodegen.h"

#include "asm/x64/reg/RegSet.h"
#include "lir/x64/asm/operand/XOp.h"
#include "lir/x64/asm/MasmEmitter.h"
#include "lir/x64/asm/cc/CallConv.h"
#include "lir/x64/asm/visitors/GPBinarySrcAddrVisitor.h"
#include "lir/x64/asm/visitors/GPBinaryVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryOutVisitor.h"
#include "lir/x64/asm/visitors/GPUnaryAddrVisitor.h"
#include "lir/x64/asm/visitors/GPBinaryAddrVisitor.h"
#include "lir/x64/asm/visitors/XBinaryVisitor.h"
#include "lir/x64/asm/visitors/XUnaryVisitor.h"
#include "lir/x64/asm/visitors/XUnaryOutVisitor.h"

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
#include "lir/x64/asm/emitters/AddFloatEmit.h"
#include "lir/x64/asm/emitters/CmpFloatEmit.h"
#include "lir/x64/asm/emitters/CopyFloatEmit.h"

#include "lir/x64/instruction/LIRCall.h"
#include "lir/x64/instruction/LIRInstructionBase.h"
#include "lir/x64/asm/emitters/DivIntEmit.h"
#include "lir/x64/asm/emitters/DivUIntEmit.h"
#include "lir/x64/asm/emitters/LoadByIdxIntEmit.h"
#include "lir/x64/asm/emitters/LoadFromStackGPEmit.h"
#include "lir/x64/asm/emitters/MovByIdxIntEmit.h"
#include "lir/x64/asm/emitters/StoreOnStackGPEmit.h"
#include "lir/x64/asm/map/LIRInstuctionMapping.h"
#include "lir/x64/asm/map/LIROperandMapping.h"
#include "lir/x64/operand/LIRVal.h"

static aasm::BindAttribute cvt_bind_attribute(const FunctionBind bind) noexcept {
    switch (bind) {
        case FunctionBind::DEFAULT:  return aasm::BindAttribute::DEFAULT;
        case FunctionBind::INTERNAL: return aasm::BindAttribute::INTERNAL;
        case FunctionBind::EXTERN:   return aasm::BindAttribute::EXTERNAL;
        default: std::unreachable();
    }
}

namespace {
    class LIRInstructionCodegen final: public details::LIRInstructionMapping<TemporalRegs, MasmEmitter> {
    public:
        explicit LIRInstructionCodegen(MasmEmitter& as, const TemporalRegs& regs, aasm::SymbolTable& symbol_table, const LIRBlock* next, std::unordered_map<const LIRBlock*, aasm::Label>& bb_labels) noexcept:
            LIRInstructionMapping(regs, as, symbol_table),
            m_next(next),
            m_bb_labels(bb_labels) {}

        void gen(const LIRVal &out) override {}

        void mul_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void and_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void or_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shl_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shr_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void setcc_i(const LIRVal &out, aasm::CondType cond_type) override {
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

        void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override {}

        void neg_i(const LIRVal &out, const LIROperand &in) override {}

        void not_i(const LIRVal &out, const LIROperand &in) override {}

        void up_stack(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, const std::size_t local_area_size) override {
            for (const auto &reg: reg_set) {
                m_as.pop(8, reg);
            }

            auto size_to_adjust = caller_overflow_area_size;
            if (const auto remains = (local_area_size+caller_overflow_area_size+reg_set.size()*8) % call_conv::STACK_ALIGNMENT; remains != 0L) {
                size_to_adjust += remains;
            }
            if (size_to_adjust != 0) {
                m_as.add(8, checked_cast<std::int32_t>(size_to_adjust), aasm::rsp);
            }
        }

        void down_stack(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {
            auto size_to_adjust = caller_overflow_area_size;
            if (const auto remains = (local_area_size+caller_overflow_area_size+reg_set.size()*8) % call_conv::STACK_ALIGNMENT; remains != 0L) {
                size_to_adjust += remains;
            }
            if (size_to_adjust != 0) {
                m_as.sub(8, checked_cast<std::int32_t>(size_to_adjust), aasm::rsp);
            }

            for (auto rev: std::ranges::reverse_view(reg_set)) {
                m_as.push(8, rev);
            }
        }


        void prologue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {
            if (reg_set.empty() && caller_overflow_area_size == 0 && local_area_size == 0) {
                return;
            }

            m_as.push(8, aasm::rbp);
            m_as.copy(8, aasm::rsp, aasm::rbp);

            auto size_to_adjust = local_area_size;
            if (const auto remains = local_area_size % call_conv::STACK_ALIGNMENT; remains != 0L) {
                size_to_adjust += remains; // Stack must be aligned on 16.
            }

            m_as.sub(8, checked_cast<std::int32_t>(size_to_adjust), aasm::rsp);
            for (const auto& reg: reg_set) {
                m_as.push(8, reg);
            }
        }

        void epilogue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {
            if (reg_set.empty() && caller_overflow_area_size == 0 && local_area_size == 0) {
                return;
            }

            for (const auto& reg: std::ranges::reverse_view(reg_set)) {
                m_as.pop(8, reg);
            }

            m_as.leave();
        }

        void load_f(const LIRVal &out, const LIRVal &pointer) override {}

        void mov_f(const LIROperand &in1, const LIROperand &in2) override {}

        void jmp(const LIRBlock *bb) override {
            if (m_next == bb) {
                return;
            }

            m_as.jmp(m_bb_labels.at(bb));
        }

        void jcc(const aasm::CondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) override {
            const auto target_false = m_bb_labels.at(on_false);
            m_as.jcc(aasm::invert(cond_type), target_false);
        }

        void call(const LIRVal &out, std::string_view name, std::span<LIRVal const> args, FunctionBind bind) override {
            const auto [symbol, _] = m_symbol_tab.add(name, cvt_bind_attribute(bind));
            m_as.call(symbol);
        }

        void vcall(const std::string_view name, std::span<LIRVal const> args, FunctionBind bind) override {
            const auto [symbol, _] = m_symbol_tab.add(name, cvt_bind_attribute(bind));
            m_as.call(symbol);
        }

        void icall(const LIRVal &out, const LIRVal &pointer, std::span<LIRVal const> args) override {}

        void ivcall(const LIRVal &pointer, std::span<LIRVal const> args) override {}

        void ret(std::span<LIRVal const> ret_values) override {
            m_as.ret();
        }

        const LIRBlock* m_next{};
        std::unordered_map<const LIRBlock*, aasm::Label>& m_bb_labels;
    };
}

void LIRFunctionCodegen::setup_basic_block_labels() {
    for (const auto &bb: m_preorder) {
        if (bb == m_data.first()) {
            // Skip the first basic block, it does not need a label.
            continue;
        }

        const auto label = m_as.create_label();
        m_bb_labels.emplace(bb, label);
    }
}

void LIRFunctionCodegen::traverse_instructions() {
    const LIRBlock* m_next{};
    for (const auto [idx, bb]: std::ranges::views::enumerate(m_preorder)) {
        if (bb != m_data.first()) {
            m_as.set_label(m_bb_labels.at(bb));
        }
        
        if (static_cast<std::uint64_t>(idx) + 1 < m_preorder.size()) {
            m_next = m_preorder[idx + 1];
        }
        
        for (auto& inst: bb->instructions()) {
            LIRInstructionCodegen codegen(m_as, inst.temporal_regs(), m_sym_tab, m_next, m_bb_labels);
            inst.visit(codegen);
        }
    }
}