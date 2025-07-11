#include "MachFunctionCodegen.h"

#include "emitters/CopyGPEmit.h"

GPOp MachFunctionCodegen::convert(const LIROperand &val) const {
    if (const auto vreg = val.vreg(); vreg.has_value()) {
        return m_reg_allocation[vreg.value()];
    }
    if (const auto cst = val.cst(); cst.has_value()) {
        return cst.value().value();
    }

    die("Invalid LIROperand");
}

void MachFunctionCodegen::copy_i(const LIRVal &out, const LIROperand &in) {
    const auto out_reg = m_reg_allocation[out];
    CopyGPEmit::emit(m_as, out.size(), out_reg, convert(in));
}