#include "LIRProducerInstruction.h"

void LIRProducerInstruction::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRProdInstKind::Gen: visitor.gen(def(0)); break;
        case LIRProdInstKind::Add: visitor.add_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Sub: visitor.sub_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Mul: visitor.mul_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::DivI: visitor.div_i(defs(), in(0), in(1)); break;
        case LIRProdInstKind::DivU: visitor.div_u(defs(), in(0), in(1)); break;
        case LIRProdInstKind::And: visitor.and_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Or:  visitor.or_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Xor: visitor.xor_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Shl: visitor.shl_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Shr: visitor.shr_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Neg: visitor.neg_i(def(0), in(0)); break;
        case LIRProdInstKind::Not: visitor.not_i(def(0), in(0)); break;
        case LIRProdInstKind::CopyI: visitor.copy_i(def(0), in(0)); break;
        case LIRProdInstKind::LoadI: {
            const auto in0 = LIRVal::try_from(in(0));
            assertion(in0.has_value(), "invariant");
            visitor.load_i(def(0), in0.value());
            break;
        }
        case LIRProdInstKind::CopyF: visitor.copy_f(def(0), in(0)); break;
        case LIRProdInstKind::LoadF: {
            const auto in0 = LIRVal::try_from(in(0));
            assertion(in0.has_value(), "invariant");
            visitor.load_f(def(0), in0.value());
            break;
        }
        case LIRProdInstKind::LoadByIdx: visitor.load_by_idx_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::ReadByOffset: visitor.read_by_offset_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Lea: visitor.lea_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Movz: visitor.movzx_i(def(0), in(0)); break;
        case LIRProdInstKind::Movs: visitor.movsx_i(def(0), in(0)); break;
        case LIRProdInstKind::Trunc: visitor.trunc_i(def(0), in(0)); break;
        default: die("Unsupported LIRProducerInstruction kind: {}", static_cast<int>(m_kind));
    }
}