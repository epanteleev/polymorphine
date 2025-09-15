#include "LIRCall.h"

void LIRCall::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRCallKind::Call: {
            visitor.call(def(0), m_name, to_lir_vals_only(inputs()), m_bind);
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
        default: std::unreachable();
    }
}