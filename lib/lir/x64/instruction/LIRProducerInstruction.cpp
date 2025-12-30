#include "LIRProducerInstruction.h"

void LIRProducerInstruction::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRProdInstKind::Gen: visitor.gen(def(0)); break;
        case LIRProdInstKind::Add: {
            switch (type(0)) {
                case LIRValType::GP: visitor.add_i(def(0), in(0), in(1)); break;
                case LIRValType::FP: visitor.add_f(def(0), in(0), in(1)); break;
                default: std::unreachable();
            }
            break;
        }
        case LIRProdInstKind::Sub: visitor.sub_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Mul: visitor.mul_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::DivI: visitor.div_i(defs(), in(0), in(1)); break;
        case LIRProdInstKind::DivU: visitor.div_u(defs(), in(0), in(1)); break;
        case LIRProdInstKind::DivF: visitor.div_f(def(0), in(0), in(1)); break;
        case LIRProdInstKind::And: visitor.and_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Or:  visitor.or_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Xor: visitor.xor_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Sal: visitor.sal_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Sar: visitor.sar_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Shr: visitor.shr_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Neg: visitor.neg_i(def(0), in(0)); break;
        case LIRProdInstKind::Not: visitor.not_i(def(0), in(0)); break;
        case LIRProdInstKind::Copy: {
            switch (type(0)) {
                case LIRValType::GP: visitor.copy_i(def(0), in(0)); break;
                case LIRValType::FP: visitor.copy_f(def(0), in(0)); break;
                default: std::unreachable();
            }
            break;
        }
        case LIRProdInstKind::Load: {
            const auto in0 = LIRVal::try_from(in(0));
            assertion(in0.has_value(), "invariant");
            switch (type(0)) {
                case LIRValType::GP: visitor.load_i(def(0), in0.value()); break;
                case LIRValType::FP: visitor.load_f(def(0), in0.value()); break;
                default: std::unreachable();
            }
            break;
        }
        case LIRProdInstKind::LoadByIdx: {
            switch (type(0)) {
                case LIRValType::GP: visitor.load_by_idx_i(def(0), in(0), in(1)); break;
                case LIRValType::FP: visitor.load_by_idx_f(def(0), in(0), in(1)); break;
                default: std::unreachable();
            }
            break;
        }
        case LIRProdInstKind::ReadByOffset: {
            switch (type(0)) {
                case LIRValType::GP: visitor.read_by_offset_i(def(0), in(0), in(1)); break;
                case LIRValType::FP: visitor.read_by_offset_f(def(0), in(0), in(1)); break;
                default: std::unreachable();
            }
            break;
        }
        case LIRProdInstKind::Lea: visitor.lea_i(def(0), in(0), in(1)); break;
        case LIRProdInstKind::Movz: visitor.movzx_i(def(0), in(0)); break;
        case LIRProdInstKind::Movs: visitor.movsx_i(def(0), in(0)); break;
        case LIRProdInstKind::Trunc: visitor.trunc_i(def(0), in(0)); break;
        case LIRProdInstKind::CvtFp2Int: visitor.cvtfp2int(def(0), in(0)); break;
        case LIRProdInstKind::CvtInt2Fp: visitor.cvtint2fp(def(0), in(0)); break;
        case LIRProdInstKind::CvtUInt2Fp: visitor.cvtuint2fp(def(0), in(0)); break;
        default: std::unreachable();
    }
}