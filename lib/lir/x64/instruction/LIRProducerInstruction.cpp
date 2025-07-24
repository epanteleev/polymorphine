#include "LIRProducerInstruction.h"

void LIRProducerInstruction::visit(LIRVisitor &visitor) {
    switch (m_kind) {
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
        case LIRProdInstKind::Copy: {
            visitor.copy_i(def(0), in(0));
            break;
        }
        case LIRProdInstKind::Cmp: visitor.cmp_i(in(0), in(1)); break;
        case LIRProdInstKind::ParallelCopy: {
            const auto out0 = def(0);
            const auto vregs = to_vregs_only(inputs());
            visitor.parallel_copy(out0, vregs);
            break;
        }
    }
}