#include "LIRProducerInstruction.h"

void LIRProducerInstruction::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRProdInstKind::Gen: visitor.gen(def(0)); break;
        case LIRProdInstKind::Add: visitor.add_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Sub: visitor.sub_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Mul: visitor.mul_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Div: visitor.div_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::And: visitor.and_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Or:  visitor.or_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Xor: visitor.xor_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Shl: visitor.shl_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Shr: visitor.shr_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Neg: visitor.neg_i(def(0), in(0)); break;
        case LIRProdInstKind::Not: visitor.not_i(def(0), in(0)); break;
        case LIRProdInstKind::Copy: visitor.copy_i(def(0), in(0)); break;
        case LIRProdInstKind::Load: {
            const auto in0 = LIRVal::try_from(in(0));
            assertion(in0.has_value(), "invariant");
            visitor.load_i(def(0), in0.value());
            break;
        }
        case LIRProdInstKind::LoadByIdx: {
            const auto pointer = LIRVal::try_from(in(0));
            assertion(pointer.has_value(), "invariant");
            visitor.load_by_idx_i(def(0), pointer.value(), in(1));
            break;
        }
        case LIRProdInstKind::LoadFromStack: {
            const auto pointer = LIRVal::try_from(in(0));
            assertion(pointer.has_value(), "invariant");
            visitor.load_from_stack_i(def(0), pointer.value(), in(1));
            break;
        }
        case LIRProdInstKind::Lea: {
            const auto in0 = LIRVal::try_from(in(0));
            assertion(in0.has_value(), "invariant");
            visitor.lea_i(def(0), in0.value(), in(1));
            break;
        }
        case LIRProdInstKind::Movz: visitor.movzx_i(def(0), in(0)); break;
        case LIRProdInstKind::Movs: visitor.movsx_i(def(0), in(0)); break;
        case LIRProdInstKind::Trunc: visitor.trunc_i(def(0), in(0)); break;
        default: die("Unsupported LIRProducerInstruction kind: {}", static_cast<int>(m_kind));
    }
}