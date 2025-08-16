#include "LIRControlInstruction.h"

void LIRBranch::visit(LIRVisitor &visitor) {
    visitor.jmp(succ(0));
}

void LIRCondBranch::visit(LIRVisitor &visitor) {
    visitor.jcc(m_kind, succ(0), succ(1));
}

void LIRReturn::visit(LIRVisitor &visitor) {
    const auto ret_values = to_lir_vals_only(inputs());
    visitor.ret(ret_values);
}

void LIRCall::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRCallKind::Call: {
            visitor.call(def(0), m_name, to_lir_vals_only(inputs()), m_linkage);
            break;
        }
        case LIRCallKind::ICall: {
            const auto pointer = LIRVal::try_from(in(0));
            assertion(pointer.has_value(), "invariant");
            const auto lir_vals = to_lir_vals_only(inputs().subspan(1));
            visitor.icall(def(0), pointer.value(), lir_vals);
            break;
        }
        case LIRCallKind::IVCall: {
            const auto pointer = LIRVal::try_from(in(0));
            assertion(pointer.has_value(), "invariant");
            const auto lir_vals = to_lir_vals_only(inputs().subspan(1));
            visitor.ivcall(pointer.value(), lir_vals);
            break;
        }
        case LIRCallKind::VCall: visitor.vcall(to_lir_vals_only(inputs())); break;
    }
}