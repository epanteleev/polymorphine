#pragma once

#include "base/analysis/AnalysisPassManagerBase.h"

#include "lir/x64/asm/cc/CallConv.h"
#include "lir/x64/module/LIRBlock.h"
#include "lir/x64/module/LIRFuncData.h"
#include "lir/x64/operand/OperandMatcher.h"
#include "utility/ArithmeticUtils.h"

template<call_conv::CallConv CC>
class FixedRegistersEvalBase final {
    explicit FixedRegistersEvalBase(LIRFuncData &obj_func_data) noexcept:
        m_obj_func_data(obj_func_data) {}

public:
    void run() {
        handle_argument_values();
        handle_basic_blocks();
    }

    static FixedRegistersEvalBase create(AnalysisPassManagerBase<LIRFuncData>*, LIRFuncData *data) {
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
                lir_val_arg.assign_reg(caller_arg_stack_alloc(arg.size(), 8));
                continue;
            }

            if (idx >= CC::GP_ARGUMENT_REGISTERS.size()) {
                // No more arguments to process.
                // Put argument in overflow area.
                lir_val_arg.assign_reg(arg_stack_alloc(8));
                continue;
            }

            const auto arg_reg = CC::GP_ARGUMENT_REGISTERS[idx];
            lir_val_arg.assign_reg(arg_reg);
            m_args.emplace(arg_reg);
            idx += 1;
        }
    }

    void handle_basic_blocks() {
        for (const auto& bb: m_obj_func_data.basic_blocks()) {
            for (const auto& inst: bb.instructions()) {
                LIRInstTraverse traverser;
                traverser.run(inst);
            }
        }
    }

    class LIRInstTraverse final: public LIRVisitor {
    public:
        explicit LIRInstTraverse() noexcept = default;

        void run(const LIRInstructionBase& inst) {
            const_cast<LIRInstructionBase&>(inst).visit(*this);
        }

    private:
        void gen(const LIRVal &out) override {}
        void add_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}
        void sub_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}
        void mul_i(const LIRVal &out, const LIROperand &in1, const LIROperand &in2) override {}
        void div_i(const std::span<LIRVal const> outs, const LIROperand &in1, const LIROperand &in2) override {
            outs[0].assign_reg(aasm::rax);
            outs[1].assign_reg(aasm::rdx);
            const auto in_vreg = in1.as_vreg().value();
            in_vreg.assign_reg(aasm::rax);
        }

        void div_u(const std::span<LIRVal const> outs, const LIROperand &in1, const LIROperand &in2) override {
            outs[0].assign_reg(aasm::rax);
            outs[1].assign_reg(aasm::rdx);
            const auto in_vreg = in1.as_vreg().value();
            in_vreg.assign_reg(aasm::rax);
        }

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
        void up_stack(const aasm::GPRegSet& reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {}
        void down_stack(const aasm::GPRegSet& reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {}
        void prologue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {}
        void epilogue(const aasm::GPRegSet &reg_set, std::size_t caller_overflow_area_size, std::size_t local_area_size) override {}
        void copy_i(const LIRVal &out, const LIROperand &in) override {}
        void load_i(const LIRVal &out, const LIRVal &pointer) override {}
        void load_by_idx_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) override {}
        void load_from_stack_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) override {}
        void load_stack_addr_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) override {}
        void lea_i(const LIRVal &out, const LIRVal &pointer, const LIROperand &index) override {}
        void jmp(const LIRBlock *bb) override {}

        void jcc(aasm::CondType cond_type, const LIRBlock *on_true, const LIRBlock *on_false) override {}

        aasm::Address arg_stack_alloc() noexcept {
            const auto addr = aasm::Address(aasm::rsp, m_arg_area_size);
            m_arg_area_size += 8;
            return addr;
        }

        void call(const LIRVal &out, std::string_view name, const std::span<LIRVal const> args, FunctionLinkage linkage) override {
            out.assign_reg(aasm::rax);
            std::int32_t caller_arg_area_size{};
            std::size_t idx{};
            for (const auto& lir_val_arg: args) {
                if (lir_val_arg.isa(gen_v())) {
                    lir_val_arg.assign_reg(aasm::Address(aasm::rsp, caller_arg_area_size));
                    caller_arg_area_size += align_up(lir_val_arg.size(), 8);
                    continue;
                }

                if (idx >= CC::GP_ARGUMENT_REGISTERS.size()) {
                    lir_val_arg.assign_reg(arg_stack_alloc());
                    continue;
                }

                lir_val_arg.assign_reg(CC::GP_ARGUMENT_REGISTERS[idx]);
                idx += 1;
            }
        }

        void vcall(std::span<LIRVal const> args) override {}

        void icall(const LIRVal &out, const LIRVal &pointer, std::span<LIRVal const> args) override {}

        void ivcall(const LIRVal &pointer, std::span<LIRVal const> args) override {}

        void ret(const std::span<const LIRVal> ret_values) override {
            switch (ret_values.size()) {
                case 0: return;
                case 2: {
                    ret_values[1].assign_reg(aasm::rdx);
                    [[fallthrough]];
                }
                case 1: {
                    ret_values[0].assign_reg(aasm::rax);
                    return;
                }
                default: die("Unsupported number of return values: {}", ret_values.size());
            }
        }

        std::uint32_t m_arg_area_size{};
    };

    LIRFuncData& m_obj_func_data;

    aasm::GPRegSet m_args{};
    std::uint32_t m_arg_area_size{};
    std::uint32_t m_caller_overflow_area_size{};
};