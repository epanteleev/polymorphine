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
        m_obj_func_data(obj_func_data) {}

public:
    void run() {
        handle_argument_values();
        handle_basic_blocks();
    }

    std::unique_ptr<result_type> result() noexcept {
        return std::make_unique<FixedRegisters>(std::move(m_reg_map), std::move(m_args));
    }

    static FixedRegistersEvalBase create(AnalysisPassManagerBase<LIRFuncData>*, const LIRFuncData *data) {
        return FixedRegistersEvalBase(*data);
    }

private:
    aasm::Address arg_stack_alloc(const std::size_t size) noexcept {
        m_arg_area_size = align_up(m_arg_area_size, size) + size;
        return aasm::Address(aasm::rbp, 8+m_arg_area_size);
    }

    void handle_argument_values() {
        for (const auto idx: std::ranges::iota_view{0U, CC::GP_ARGUMENT_REGISTERS.size()}) {
            if (idx >= m_obj_func_data.args().size()) {
                break; // No more arguments to process
            }

            const auto& arg = m_obj_func_data.arg(idx);
            const auto arg_reg = CC::GP_ARGUMENT_REGISTERS[idx];
            m_reg_map.emplace(arg, arg_reg);
            m_args.emplace_back(arg_reg);
        }

        if (m_obj_func_data.args().size() <= CC::GP_ARGUMENT_REGISTERS.size()) {
            return; // No need to allocate overflow area for arguments.
        }

        const auto overflow_args = m_obj_func_data.args().size() - CC::GP_ARGUMENT_REGISTERS.size();
        for (std::size_t i{}; i < overflow_args; ++i) {
            const auto arg = m_obj_func_data.arg(CC::GP_ARGUMENT_REGISTERS.size() + i);
            m_reg_map.emplace(arg, arg_stack_alloc(8));
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
        explicit LIRInstTraverse(LIRValMap<GPVReg>& fixed_reg) noexcept:
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
        void setcc_i(const LIRVal &out, aasm::CondType cond_type) override {}
        void cmov_i(aasm::CondType cond_type, const LIRVal& out, const LIROperand& in1, const LIROperand& in2) override {}
        void parallel_copy(const LIRVal &out, std::span<LIRVal const> inputs) override {}
        void movzx_i(const LIRVal &out, const LIROperand &in) override {}
        void movsx_i(const LIRVal &out, const LIROperand &in) override {}
        void trunc_i(const LIRVal &out, const LIROperand &in) override {}
        void cmp_i(const LIROperand &in1, const LIROperand &in2) override {}
        void neg_i(const LIRVal &out, const LIROperand &in) override {}
        void not_i(const LIRVal &out, const LIROperand &in) override {}
        void mov_i(const LIRVal &in1, const LIROperand &in2) override {}
        void mov_by_idx_i(const LIRVal &out, const LIROperand &index, const LIROperand &in2) override {}
        void store_on_stack_i(const LIRVal &pointer, const LIROperand &index, const LIROperand &value) override {}
        void store_i(const LIRVal &pointer, const LIROperand &value) override {}
        void up_stack(const aasm::GPRegSet& reg_set, std::size_t caller_overflow_area_size) override {}
        void down_stack(const aasm::GPRegSet& reg_set, std::size_t caller_overflow_area_size) override {}
        void prologue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size) override {}
        void epilogue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size) override {}
        void copy_i(const LIRVal &out, const LIROperand &in) override {}
        void load_i(const LIRVal &out, const LIRVal &pointer) override {}
        void load_by_idx_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) override {}
        void load_from_stack_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) override {}
        void lea_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) override {}
        void jmp(const LIRBlock *bb) override {}

        void jcc(aasm::CondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) override {}

        aasm::Address arg_stack_alloc() noexcept {
            const auto addr = aasm::Address(aasm::rsp, m_arg_area_size);
            m_arg_area_size += 8;
            return addr;
        }

        void call(const LIRVal &out, std::string_view name, const std::span<LIRVal const> args, LIRLinkage linkage) override {
            m_fixed_reg.emplace(out, aasm::rax);
            for (const auto idx: std::ranges::iota_view{0U, CC::GP_ARGUMENT_REGISTERS.size()}) {
                if (idx >= args.size()) {
                    break; // No more arguments to process
                }

                const auto& arg = args[idx];
                const auto arg_reg = CC::GP_ARGUMENT_REGISTERS[idx];
                m_fixed_reg.emplace(arg, arg_reg);
            }

            if (args.size() <= CC::GP_ARGUMENT_REGISTERS.size()) {
                return; // No need to allocate overflow area for arguments.
            }

            const auto overflow_args = args.size() - CC::GP_ARGUMENT_REGISTERS.size();
            for (std::size_t i{}; i < overflow_args; ++i) {
                const auto arg = args[CC::GP_ARGUMENT_REGISTERS.size() + i];
                m_fixed_reg.emplace(arg, arg_stack_alloc());
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

        LIRValMap<GPVReg>& m_fixed_reg;
        std::uint32_t m_arg_area_size{};
    };

    const LIRFuncData& m_obj_func_data;

    std::vector<aasm::GPReg> m_args{};
    LIRValMap<GPVReg> m_reg_map{};
    std::uint32_t m_arg_area_size{};
};

