#include "LIRInstruction.h"


void LIRInstruction::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRInstKind::Mov: {
            const auto in0 = LIRVal::try_from(in(0));
            assertion(in0.has_value(), "invariant");
            visitor.mov_i(in0.value(), in(1));
            break;
        }
        case LIRInstKind::MovByIdx: {
            const auto in0 = LIRVal::try_from(in(0));
            assertion(in0.has_value(), "invariant");
            visitor.mov_by_idx_i(in0.value(), in(1), in(2));
            break;
        }
        case LIRInstKind::Store: {
            const auto pointer = LIRVal::try_from(in(0));
            assertion(pointer.has_value(), "invariant");
            visitor.store_i(pointer.value(), in(1));
            break;
        }
        case LIRInstKind::Cmp: visitor.cmp_i(in(0), in(1)); break;
        default: die("Unsupported LIR instruction kind: {}", static_cast<int>(m_kind));
    }
}
