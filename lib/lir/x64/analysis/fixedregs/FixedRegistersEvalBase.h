#pragma once

#include "base/analysis/AnalysisPassManagerBase.h"

#include "lir/x64/asm/cc/CallConv.h"
#include "lir/x64/analysis/fixedregs/FixedRegisters.h"
#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/module/LIRFuncData.h"

template<call_conv::CallConv CC>
class FixedRegistersEvalBase final {
public:
    using result_type = FixedRegisters;
    using basic_block = LIRBlock;
    static constexpr auto analysis_kind = AnalysisType::FixedRegisters;

private:
    explicit FixedRegistersEvalBase(const LIRFuncData &obj_func_data) noexcept:
        m_obj_func_data(obj_func_data){}

public:
    void run() {
        handle_argument_values();
        handle_basic_blocks();
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<FixedRegisters>(std::move(m_reg_map), std::move(m_args));
    }

    static FixedRegistersEvalBase create(AnalysisPassManagerBase<LIRFuncData> *, const LIRFuncData *data) {
        return FixedRegistersEvalBase(*data);
    }

private:
    template<typename Reg, std::size_t N>
    class ArgumentAllocator final {
    public:
        explicit ArgumentAllocator(const std::array<Reg, N>& regs):
            m_regs(regs) {}

        Reg get_reg() noexcept {
            if (m_gp_reg_pos < N) {
                return m_regs[m_gp_reg_pos++];
            }

            unimplemented();
        }

    private:
        std::size_t m_gp_reg_pos{};
        const std::array<Reg, N>& m_regs;
    };

    void handle_argument_values() {
        ArgumentAllocator arguments(CC::GP_ARGUMENT_REGISTERS);
        for (const auto& arg: m_obj_func_data.args()) {
            const auto reg = arguments.get_reg();
            m_reg_map.emplace(arg, reg);
            m_args.emplace_back(reg);
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
        explicit LIRInstTraverse(LIRValMap<aasm::GPReg>& fixed_reg) noexcept:
            m_fixed_reg(fixed_reg) {}

        void run(const LIRInstructionBase& inst) {
            const_cast<LIRInstructionBase&>(inst).visit(*this);
        }

    private:
        void gen(const LIRVal &out) override {}

        void add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void mul_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void div_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void and_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void or_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void xor_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shl_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shr_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}

        void setcc_i(const LIRVal &out, LIRCondType cond_type) override {}

        void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override {}

        void cmp_i(const LIROperand &in1, const LIROperand &in2) override {}

        void neg_i(const LIRVal &out, const LIROperand &in) override {}

        void not_i(const LIRVal &out, const LIROperand &in) override {}

        void mov_i(const LIRVal &in1, const LIROperand &in2) override {}

        void store_i(const LIRVal &pointer, const LIROperand &value) override {}

        void copy_i(const LIRVal &out, const LIROperand &in) override {}

        void load_i(const LIRVal &out, const LIRVal &pointer) override {}

        void jmp(const LIRBlock *bb) override {}

        void jcc(LIRCondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) override {}

        void call(const LIRVal &out, std::string_view name, const std::span<LIRVal const> args, LIRLinkage linkage) override {
            m_fixed_reg.emplace(out, aasm::rax);
            ArgumentAllocator arguments(CC::GP_ARGUMENT_REGISTERS);
            for (const auto& arg: args) {
                m_fixed_reg.emplace(arg, arguments.get_reg());
            }
        }

        void vcall(std::span<LIRVal const> args) override {}

        void icall(const LIRVal &out, const LIRVal &pointer, std::span<LIRVal const> args) override {}

        void ivcall(const LIRVal &pointer, std::span<LIRVal const> args) override {}

        void ret(const std::span<const LIRVal> ret_values) override {
            if (ret_values.size() == 1) {
                m_fixed_reg.emplace(ret_values[0], aasm::rax);
            }
        }

        LIRValMap<aasm::GPReg>& m_fixed_reg;
    };

    const LIRFuncData& m_obj_func_data;

    std::vector<aasm::GPReg> m_args{};
    LIRValMap<aasm::GPReg> m_reg_map{};
};

