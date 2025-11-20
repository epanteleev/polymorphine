#include "LIRInstruction.h"

void LIRInstruction::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRInstKind::Mov: {
            switch (m_val_type) {
                case LIRValType::GP: visitor.mov_i(in(0), in(1)); break;
                case LIRValType::FP: visitor.mov_f(in(0), in(1)); break;
                default: std::unreachable();
            }
            break;
        }
        case LIRInstKind::MovByIdx: {
            const auto inout = LIRVal::try_from(in(0));
            assertion(inout.has_value(), "invariant");
            visitor.mov_by_idx_i(inout.value(), in(1), in(2));
            break;
        }
        case LIRInstKind::StoreByOffset: visitor.store_by_offset_i(in(0), in(1), in(2)); break;
        case LIRInstKind::Store: {
            const auto pointer = LIRVal::try_from(in(0));
            assertion(pointer.has_value(), "invariant");
            visitor.store_i(pointer.value(), in(1));
            break;
        }
        default: std::unreachable();
    }
}
