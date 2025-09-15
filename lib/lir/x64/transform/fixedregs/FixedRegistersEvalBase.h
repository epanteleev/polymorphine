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
    void handle_argument_values() {
        std::size_t idx{};
        for (const auto& lir_val_arg: m_obj_func_data.args()) {
            if (const auto arg = LIRArg::try_from(lir_val_arg).value(); arg.attributes().has(Attribute::ByValue)) {
                // Struct type argument in overflow area.
                lir_val_arg.assign_reg(arg_stack_alloc(arg.size(), 8));
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

    aasm::Address arg_stack_alloc(const std::size_t size) noexcept {
        m_arg_area_size = align_up(m_arg_area_size, size) + size;
        return aasm::Address(aasm::rbp, 8+m_arg_area_size);
    }

    aasm::Address arg_stack_alloc(const std::size_t size, const std::size_t align) noexcept {
        const auto offset = m_callee_overflow_area_size;
        m_callee_overflow_area_size = align_up(m_callee_overflow_area_size, align) + size;
        return aasm::Address(aasm::rsp, 8 + offset);
    }

    void handle_basic_blocks() {
        for (const auto& bb: m_obj_func_data.basic_blocks()) {
            const auto last = bb.last();
            const auto call = dynamic_cast<const LIRCall*>(last);
            if (call == nullptr) {
                continue;
            }

            allocate_arguments_for_call(call->def(0), call->inputs());
        }
    }

    void allocate_arguments_for_call(const LIRVal &out, const std::span<LIROperand const> args) {
        out.assign_reg(aasm::rax);
        std::int32_t caller_arg_area_size{};
        std::size_t idx{};
        for (const auto& lir_val: args) {
            const auto lir_val_arg = LIRVal::try_from(lir_val).value();
            if (lir_val_arg.isa(gen_v()) || idx >= CC::GP_ARGUMENT_REGISTERS.size()) {
                lir_val_arg.assign_reg(aasm::Address(aasm::rsp, caller_arg_area_size));
                caller_arg_area_size += align_up(lir_val_arg.size(), 8);
                continue;
            }

            lir_val_arg.assign_reg(CC::GP_ARGUMENT_REGISTERS[idx]);
            idx += 1;
        }
    }

    LIRFuncData& m_obj_func_data;

    aasm::GPRegSet m_args{};
    std::uint32_t m_arg_area_size{};
    std::uint32_t m_callee_overflow_area_size{};
};