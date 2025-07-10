#include "LIRInstruction.h"

void LIRInstruction::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRInstKind::Add: visitor.add_i(def(0), in(0), in(1)); break;
        case LIRInstKind::Sub: visitor.sub_i(def(0), in(0), in(1)); break;
        case LIRInstKind::Mul: visitor.mul_i(def(0), in(0), in(1)); break;
        case LIRInstKind::Div: visitor.div_i(def(0), in(0), in(1)); break;
        case LIRInstKind::And: visitor.and_i(def(0), in(0), in(1)); break;
        case LIRInstKind::Or:  visitor.or_i(def(0), in(0), in(1)); break;
        case LIRInstKind::Xor: visitor.xor_i(def(0), in(0), in(1)); break;
        case LIRInstKind::Shl: visitor.shl_i(def(0), in(0), in(1)); break;
        case LIRInstKind::Shr: visitor.shr_i(def(0), in(0), in(1)); break;
        case LIRInstKind::Neg: visitor.neg_i(def(0), in(0)); break;
        case LIRInstKind::Not: visitor.not_i(def(0), in(0)); break;
        case LIRInstKind::Mov: {
            const auto in0 = LIRVReg::try_from(in(0));
            assertion(in0.has_value(), "invariant");

            const auto in1 = LIRVReg::try_from(in(1));
            assertion(in1.has_value(), "invariant");

            visitor.mov_i(in0.value(), in1.value());
            break;
        }
        case LIRInstKind::Copy: {
            const auto out1 = LIRVReg::try_from(def(0));
            assertion(out1.has_value(), "invariant");

            visitor.copy_i(out1.value(), in(0));
            break;
        }
        case LIRInstKind::Cmp: visitor.cmp_i(in(0), in(1)); break;
        case LIRInstKind::ParallelCopy: {
            const auto out0 = LIRVReg::try_from(def(0));
            assertion(out0.has_value(), "invariant");

            const auto vregs = to_vregs_only(inputs());
            visitor.parallel_copy(out0.value(), vregs);
            break;
        }
    }
}

LIRInstBuilder<LIRInstruction> LIRInstruction::copy(const LIROperand &op)  {
    return [=](std::size_t id, MachBlock *bb) {
        auto copy = std::make_unique<LIRInstruction>(id, bb, LIRInstKind::Copy, std::vector{op}, std::vector<LIRVReg>{});
        copy->add_def(LIRVReg(op.size(), 0, copy.get()));
        return copy;
    };
}