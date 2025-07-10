#pragma once
#include "FixedRegisters.h"
#include "base/analysis/AnalysisPassCacheBase.h"
#include "lir/x64/module/MachBlock.h"
#include "lir/x64/module/ObjFuncData.h"

class FixedRegistersEval final {
public:
    using result_type = FixedRegisters;
    using basic_block = MachBlock;
    static constexpr auto analysis_kind = AnalysisType::FixedRegisters;

private:
    explicit FixedRegistersEval(const ObjFuncData &obj_func_data) noexcept:
        m_obj_func_data(obj_func_data){}

public:
    void run() {
        for (const auto& bb: m_obj_func_data.basic_blocks()) {
            for (const auto& inst: bb.instructions()) {
                LIRInstTraverse traverser(m_rax_set);
                traverser.run(inst);
            }
        }
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<FixedRegisters>(std::move(m_rax_set));
    }

    static FixedRegistersEval create(AnalysisPassCacheBase<ObjFuncData> *cache, const ObjFuncData *data) {
        return FixedRegistersEval(*data);
    }

private:
    class LIRInstTraverse final: public LIRVisitor {
    public:
        explicit LIRInstTraverse(LIRVRegSet& vreg_rax) noexcept:
            m_rax_set(vreg_rax) {}

        void run(const LIRInstructionBase& inst) {
            const_cast<LIRInstructionBase&>(inst).visit(*this);
        }

        ~LIRInstTraverse() override;
    private:
        void add_i(const LIRVReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void sub_i(const LIRVReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void mul_i(const LIRVReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void div_i(const LIRVReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void and_i(const LIRVReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void or_i(const LIRVReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void xor_i(const LIRVReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shl_i(const LIRVReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shr_i(const LIRVReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void parallel_copy(const LIRVReg &out, std::span<LIRVReg const> inputs) override {}

        void cmp_i(const LIROperand &in1, const LIROperand &in2) override {}

        void neg_i(const LIRVReg &out, const LIROperand &in) override {}

        void not_i(const LIRVReg &out, const LIROperand &in) override {}

        void mov_i(const LIRVReg &in1, const LIRVReg &in2) override {}

        void copy_i(const LIRVReg &out, const LIROperand &in) override {}

        void jmp(const MachBlock *bb) override {}

        void je(const MachBlock *on_true, const MachBlock *on_false) override {}

        void jne(const MachBlock *on_true, const MachBlock *on_false) override {}

        void jl(const MachBlock *on_true, const MachBlock *on_false) override {}

        void jle(const MachBlock *on_true, const MachBlock *on_false) override {}

        void jg(const MachBlock *on_true, const MachBlock *on_false) override {}

        void jge(const MachBlock *on_true, const MachBlock *on_false) override {}

        void call(const LIRVReg &out, std::span<LIRVReg const> args) override {}

        void vcall(std::span<LIRVReg const> args) override {}

        void icall(const LIRVReg &out, const LIRVReg &pointer, std::span<LIRVReg const> args) override {}

        void ivcall(const LIRVReg &pointer, std::span<LIRVReg const> args) override {}

        void ret(std::span<const LIROperand> ret_values) override {
            const auto& op = ret_values[0];
            const auto vreg = LIRVReg::try_from(op);
           // assertion(vreg.has_value(), "should be op={}", op);
            m_rax_set.insert(vreg.value());
        }

        LIRVRegSet& m_rax_set;
    };

    const ObjFuncData& m_obj_func_data;
    LIRVRegSet m_rax_set{};
};

