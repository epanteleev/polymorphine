
#include "platform/lir/x64/AnyLIRInstruction.h"

#include <ostream>

#include "utility/Error.h"

void AnyLIRInstruction::print(std::ostream &os) const {
    os << m_id << ":" << std::endl;
}

void LIRInstruction::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRInstKind::Add: visitor.add_i(out(0), in(0), in(1)); break;
        case LIRInstKind::Sub: visitor.sub_i(out(0), in(0), in(1)); break;
        case LIRInstKind::Mul: visitor.mul_i(out(0), in(0), in(1)); break;
        case LIRInstKind::Div: visitor.div_i(out(0), in(0), in(1)); break;
        case LIRInstKind::And: visitor.and_i(out(0), in(0), in(1)); break;
        case LIRInstKind::Or:  visitor.or_i(out(0), in(0), in(1)); break;
        case LIRInstKind::Xor: visitor.xor_i(out(0), in(0), in(1)); break;
        case LIRInstKind::Shl: visitor.shl_i(out(0), in(0), in(1)); break;
        case LIRInstKind::Shr: visitor.shr_i(out(0), in(0), in(1)); break;
        case LIRInstKind::Neg: visitor.neg_i(out(0), in(0)); break;
        case LIRInstKind::Not: visitor.not_i(out(0), in(0)); break;
        case LIRInstKind::Mov: {
            const auto in0 = VReg::from(in(0));
            assertion(in0.has_value(), "invariant");

            const auto in1 = VReg::from(in(1));
            assertion(in1.has_value(), "invariant");

            visitor.mov_i(in0.value(), in1.value());
            break;
        }
        default: die("Unsupported instruction kind");
    }
}

void LIRControlInstruction::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRControlKind::Je: visitor.je(
    }
}
