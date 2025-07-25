#include "LIRInstruction.h"

#include <utility>

void LIRInstruction::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRInstKind::Mov: {
            const auto in0 = LIRVal::try_from(in(0));
            assertion(in0.has_value(), "invariant");
            visitor.mov_i(in0.value(), in(1));
            break;
        }
        default: die("Unsupported LIR instruction kind: {}", static_cast<int>(m_kind));
    }
}
