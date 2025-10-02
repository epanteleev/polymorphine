#pragma once

#include "lir/x64/asm/cc/CallConv.h"
#include "lir/x64/operand/LIRVal.h"

namespace details {
    class ArgumentRegistersAllocator final {
    public:
        explicit ArgumentRegistersAllocator(const call_conv::CallConvProvider* call_conv, const std::span<LIRVal const> args, const std::span<const ArgumentValue> arg_values) noexcept:
            m_call_conv(call_conv),
            args(args),
            m_arg_values(arg_values) {}

        void run() {
            for (const auto& [lir_val_arg, v_arg]: std::ranges::views::zip(args, m_arg_values)) {
                if (const auto arg = LIRArg::try_from(lir_val_arg).value(); arg.attributes().has(Attribute::ByValue)) {
                    // Struct type argument in the overflow area.
                    lir_val_arg.assign_reg(overflow_arg_stack_alloc(lir_val_arg.size()));
                    continue;
                }

                const auto type = dynamic_cast<const NonTrivialType*>(v_arg.type());
                assertion(type != nullptr, "Expected NonTrivialType for argument type");

                if (type->isa(gp_type())) {
                    assign_gp_reg(lir_val_arg);
                    continue;
                }

                if (type->isa(float_type())) {
                    assign_xmm_reg(lir_val_arg);
                    continue;
                }

                die("unknown type");
            }
        }

    private:
        void assign_gp_reg(const LIRVal& lir_val_arg) {
            if (m_gp_reg_idx >= m_call_conv->GP_ARGUMENT_REGISTERS().size()) {
                // No more arguments to process.
                // Put an argument in the overflow area.
                lir_val_arg.assign_reg(arg_stack_alloc(8));
                return;
            }

            const auto arg_reg = m_call_conv->GP_ARGUMENT_REGISTERS(m_gp_reg_idx);
            lir_val_arg.assign_reg(arg_reg);
            m_gp_reg_idx += 1;
        }

        void assign_xmm_reg(const LIRVal& lir_val_arg) {
            if (m_gp_reg_idx >= m_call_conv->XMM_ARGUMENT_REGISTERS().size()) {
                // No more arguments to process.
                // Put an argument in the overflow area.
                lir_val_arg.assign_reg(arg_stack_alloc(8));
                return;
            }

            const auto arg_reg = m_call_conv->XMM_ARGUMENT_REGISTERS(m_gp_reg_idx);
            lir_val_arg.assign_reg(arg_reg);
            m_gp_reg_idx += 1;
        }

        aasm::Address arg_stack_alloc(const std::size_t size) noexcept {
            m_arg_area_size = align_up(m_arg_area_size, size) + size;
            return aasm::Address(aasm::rbp, checked_cast<std::int32_t>(8+m_arg_area_size));
        };

        aasm::Address overflow_arg_stack_alloc(const std::size_t size) noexcept {
            const auto offset = m_callee_overflow_area_size;
            m_callee_overflow_area_size = align_up(m_callee_overflow_area_size, 8) + size;
            return aasm::Address(aasm::rsp, 8 + offset);
        };

        const call_conv::CallConvProvider* m_call_conv;
        std::span<LIRVal const> args;
        std::span<const ArgumentValue> m_arg_values;
        std::size_t m_arg_area_size{};
        std::size_t m_callee_overflow_area_size{};
        std::size_t m_gp_reg_idx{};
    };
}