
#include "platform/lir/x64/AnyLIRInstruction.h"

#include <ostream>

#include "utility/Error.h"

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
        case LIRInstKind::Cmp: visitor.cmp_i(in(0), in(1)); break;
    }
    die("unreachable");
}

void LIRBranch::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRBranchKind::Je: visitor.je(succ(0), succ(1)); break;
        case LIRBranchKind::Jg: visitor.jg(succ(0), succ(1)); break;
        case LIRBranchKind::Jl: visitor.jl(succ(0), succ(1)); break;
        case LIRBranchKind::Jmp: visitor.jmp(succ(0)); break;
        case LIRBranchKind::Ret: visitor.ret(); break;
        case LIRBranchKind::Jne: visitor.jne(succ(0), succ(1)); break;
        case LIRBranchKind::Jge: visitor.jge(succ(0), succ(1)); break;
        case LIRBranchKind::Jle: visitor.jle(succ(0), succ(1)); break;
    }
}

static std::vector<VReg> to_vregs_only(std::span<LIROperand const> inputs) {
    std::vector<VReg> vregs;
    for (const auto& in: inputs) {
        const auto vreg = VReg::from(in);
        assertion(vreg.has_value(), "invariant");
        vregs.push_back(vreg.value());
    }
    return vregs;
}

void LIRCall::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRCallKind::Call: {
            visitor.call(out(0), to_vregs_only(inputs()));
            break;
        }
        case LIRCallKind::ICall: {
            const auto pointer = VReg::from(in(0));
            assertion(pointer.has_value(), "invariant");
            visitor.icall(out(0), pointer.value(), to_vregs_only(inputs().subspan(1)));
            break;
        }
        case LIRCallKind::IVCall: {
            const auto pointer = VReg::from(in(0));
            assertion(pointer.has_value(), "invariant");
            visitor.ivcall(pointer.value(), to_vregs_only(inputs().subspan(1)));
            break;
        }
        case LIRCallKind::VCall: visitor.vcall(to_vregs_only(inputs())); break;
    }
}


void AnyLIRInstruction::print(std::ostream &os) const {
    os << m_id << ":" << std::endl;
}