#include "LIRInstruction.h"

#include <utility>

void LIRInstruction::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRInstKind::Mov: {
            const auto in0 = LIRVal::try_from(in(0));
            assertion(in0.has_value(), "invariant");

            const auto in1 = LIRVal::try_from(in(1));
            assertion(in1.has_value(), "invariant");

            visitor.mov_i(in0.value(), in1.value());
            break;
        }
        default: die("Unsupported LIR instruction kind: {}", static_cast<int>(m_kind));
    }
}
