#pragma once

#include "base/analysis/AnalysisPassManagerBase.h"

#include "lir/x64/asm/cc/CallConv.h"
#include "lir/x64/analysis/fixedregs/FixedRegisters.h"
#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/module/LIRFuncData.h"
#include "lir/x64/operand/OperandMatcher.h"

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

    aasm::Address caller_arg_stack_alloc(const std::size_t size, const std::size_t align) noexcept {
        const auto offset = m_caller_overflow_area_size;
        m_caller_overflow_area_size = align_up(m_caller_overflow_area_size, align) + size;
        return aasm::Address(aasm::rsp, 8 + offset);
    }

    void handle_argument_values() {
        std::size_t idx{};
        for (const auto& lir_val_arg: m_obj_func_data.args()) {
            if (const auto arg = LIRArg::try_from(lir_val_arg).value(); arg.attributes().has(Attribute::ByValue)) {
                // Struct type argument in overflow area.
                m_reg_map.emplace(lir_val_arg, caller_arg_stack_alloc(arg.size(), 8));
                continue;
            }

            if (idx >= CC::GP_ARGUMENT_REGISTERS.size()) {
                // No more arguments to process.
                // Put argument in overflow area.
                m_reg_map.emplace(lir_val_arg, arg_stack_alloc(8));
                continue;
            }

            const auto arg_reg = CC::GP_ARGUMENT_REGISTERS[idx];
            m_reg_map.emplace(lir_val_arg, arg_reg);
            m_args.emplace_back(arg_reg);
            idx += 1;
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

        void call(const LIRVal &out, std::string_view name, const std::span<LIRVal const> args, FunctionLinkage linkage) override {
            m_fixed_reg.emplace(out, aasm::rax);

            std::int32_t caller_arg_area_size{};
            std::size_t idx{};
            for (const auto& lir_val_arg: args) {
                if (lir_val_arg.isa(gen_v())) {
                    m_fixed_reg.emplace(lir_val_arg, aasm::Address(aasm::rsp, caller_arg_area_size));
                    caller_arg_area_size += align_up(lir_val_arg.size(), 8);
                    continue;
                }

                if (idx >= CC::GP_ARGUMENT_REGISTERS.size()) {
                    m_fixed_reg.emplace(lir_val_arg, arg_stack_alloc());
                    continue;
                }

                m_fixed_reg.emplace(lir_val_arg, CC::GP_ARGUMENT_REGISTERS[idx]);
                idx += 1;
            }
        }

        void vcall(std::span<LIRVal const> args) override {}

        void icall(const LIRVal &out, const LIRVal &pointer, std::span<LIRVal const> args) override {}

        void ivcall(const LIRVal &pointer, std::span<LIRVal const> args) override {}

        void ret(const std::span<const LIRVal> ret_values) override {
            switch (ret_values.size()) {
                case 0: return;
                case 1: {
                    m_fixed_reg.emplace(ret_values[0], aasm::rax);
                    return;
                }
                case 2: {
                    m_fixed_reg.emplace(ret_values[0], aasm::rax);
                    m_fixed_reg.emplace(ret_values[1], aasm::rdx);
                    return;
                }
                default: die("Unsupported number of return values: {}", ret_values.size());
            }
        }

        LIRValMap<GPVReg>& m_fixed_reg;
        std::uint32_t m_arg_area_size{};
    };

    const LIRFuncData& m_obj_func_data;

    std::vector<aasm::GPReg> m_args{};
    LIRValMap<GPVReg> m_reg_map{};
    std::uint32_t m_arg_area_size{};
    std::uint32_t m_caller_overflow_area_size{};
};

