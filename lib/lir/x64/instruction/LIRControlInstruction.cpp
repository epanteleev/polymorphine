#include "LIRControlInstruction.h"


void LIRBranch::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRBranchKind::Je: visitor.je(succ(0), succ(1)); break;
        case LIRBranchKind::Jg: visitor.jg(succ(0), succ(1)); break;
        case LIRBranchKind::Jl: visitor.jl(succ(0), succ(1)); break;
        case LIRBranchKind::Jmp: visitor.jmp(succ(0)); break;
        case LIRBranchKind::Jne: visitor.jne(succ(0), succ(1)); break;
        case LIRBranchKind::Jge: visitor.jge(succ(0), succ(1)); break;
        case LIRBranchKind::Jle: visitor.jle(succ(0), succ(1)); break;
    }
}

void LIRReturn::visit(LIRVisitor &visitor) {
    visitor.ret(inputs());
}

void LIRCall::visit(LIRVisitor &visitor) {
    switch (m_kind) {
        case LIRCallKind::Call: {
            visitor.call(def(0), to_vregs_only(inputs()));
            break;
        }
        case LIRCallKind::ICall: {
            const auto pointer = LIRVal::try_from(in(0));
            assertion(pointer.has_value(), "invariant");
            const auto vregs = to_vregs_only(inputs().subspan(1));
            visitor.icall(def(0), pointer.value(), vregs);
            break;
        }
        case LIRCallKind::IVCall: {
            const auto pointer = LIRVal::try_from(in(0));
            assertion(pointer.has_value(), "invariant");
            const auto vregs = to_vregs_only(inputs().subspan(1));
            visitor.ivcall(pointer.value(), vregs);
            break;
        }
        case LIRCallKind::VCall: visitor.vcall(to_vregs_only(inputs())); break;
    }
}