#pragma once

#include "ArgumentAllocator.h"
#include "CallConv.h"
#include "FixedRegisters.h"
#include "base/analysis/AnalysisPassCacheBase.h"
#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/module/LIRFuncData.h"

class FixedRegistersEval final {
public:
    using result_type = FixedRegisters;
    using basic_block = LIRBlock;
    static constexpr auto analysis_kind = AnalysisType::FixedRegisters;

private:
    explicit FixedRegistersEval(const LIRFuncData &obj_func_data) noexcept:
        m_obj_func_data(obj_func_data){}

public:
    void run() {
        handle_argument_values();
        handle_basic_blocks();
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<FixedRegisters>(std::move(m_reg_map));
    }

    static FixedRegistersEval create(AnalysisPassCacheBase<LIRFuncData> *, const LIRFuncData *data) {
        return FixedRegistersEval(*data);
    }

private:
    void handle_argument_values() {
        ArgumentAllocator arguments(call_conv::GP_ARGUMENT_REGISTERS);
        for (const auto& arg: m_obj_func_data.args()) {
            m_reg_map.emplace(arg, arguments.get_reg());
        }
    }

    void handle_basic_blocks() {
        for (const auto& bb: m_obj_func_data.basic_blocks()) {
            for (const auto& inst: bb.instructions()) {
                LIRInstTraverse traverser(m_reg_map);
                traverser.run(inst);
            }
        }
    }

    class LIRInstTraverse final: public LIRVisitor {
    public:
        explicit LIRInstTraverse(LIRValMap<GPVReg>& vreg_rax) noexcept:
            m_reg_map(vreg_rax) {}

        void run(const LIRInstructionBase& inst) {
            const_cast<LIRInstructionBase&>(inst).visit(*this);
        }

    private:
        void add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void mul_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void div_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void and_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void or_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void xor_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shl_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shr_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override {}

        void cmp_i(const LIROperand &in1, const LIROperand &in2) override {}

        void neg_i(const LIRVal &out, const LIROperand &in) override {}

        void not_i(const LIRVal &out, const LIROperand &in) override {}

        void mov_i(const LIRVal &in1, const LIRVal &in2) override {}

        void copy_i(const LIRVal &out, const LIROperand &in) override {}

        void jmp(const LIRBlock *bb) override {}

        void je(const LIRBlock *on_true, const LIRBlock *on_false) override {}

        void jne(const LIRBlock *on_true, const LIRBlock *on_false) override {}

        void jl(const LIRBlock *on_true, const LIRBlock *on_false) override {}

        void jle(const LIRBlock *on_true, const LIRBlock *on_false) override {}

        void jg(const LIRBlock *on_true, const LIRBlock *on_false) override {}

        void jge(const LIRBlock *on_true, const LIRBlock *on_false) override {}

        void call(const LIRVal &out, std::span<LIRVal const> args) override {}

        void vcall(std::span<LIRVal const> args) override {}

        void icall(const LIRVal &out, const LIRVal &pointer, std::span<LIRVal const> args) override {}

        void ivcall(const LIRVal &pointer, std::span<LIRVal const> args) override {}

        void ret(const std::span<const LIRVal> ret_values) override {
            if (ret_values.size() == 1) {
                m_reg_map.emplace(ret_values[0], aasm::rax);
            }
        }

        LIRValMap<GPVReg>& m_reg_map;
    };

    const LIRFuncData& m_obj_func_data;
    LIRValMap<GPVReg> m_reg_map{};
};

