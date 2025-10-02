#pragma once

#include "lir/x64/operand/LIROperand.h"
#include "operand/GPOp.h"
#include "operand/XOp.h"


namespace details {
    class LIROperandMapping {
    public:
        explicit LIROperandMapping(aasm::SymbolTable& symbol_tab) noexcept:
            m_symbol_tab(symbol_tab) {}

        [[nodiscard]]
        GPOp convert_to_gp_op(const LIROperand &val) const {
            if (const auto vreg = val.as_vreg(); vreg.has_value()) {
                return vreg.value().assigned_reg().to_gp_op().value();
            }

            return convert_internal<GPOp>(val);
        }

        [[nodiscard]]
        XOp convert_to_x_op(const LIROperand &val) const {
            if (const auto vreg = val.as_vreg(); vreg.has_value()) {
                return vreg.value().assigned_reg().to_xmm_op().value();
            }

            return convert_internal<XOp>(val);
        }

    private:
        template<typename T>
        [[nodiscard]]
        T convert_internal(const LIROperand &val) const {
            if (const auto cst = val.as_cst(); cst.has_value()) {
                return cst.value().value();
            }
            if (const auto slot = val.as_slot(); slot.has_value()) {
                const auto [symbol, _] = m_symbol_tab.add(slot.value()->name(), aasm::BindAttribute::INTERNAL);
                return aasm::Address(symbol);
            }

            die("Invalid LIROperand");
        }

    protected:
        aasm::SymbolTable& m_symbol_tab;
    };
}