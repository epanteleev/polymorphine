#include <ostream>
#include <ranges>

#include "LIRInstructionBase.h"
#include "../module/MachBlock.h"

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
            const auto in0 = VReg::try_from(in(0));
            assertion(in0.has_value(), "invariant");

            const auto in1 = VReg::try_from(in(1));
            assertion(in1.has_value(), "invariant");

            visitor.mov_i(in0.value(), in1.value());
            break;
        }
        case LIRInstKind::Copy: {
            const auto out1 = VReg::try_from(out(0));
            assertion(out1.has_value(), "invariant");

            visitor.copy_i(out1.value(), in(0));
            break;
        }
        case LIRInstKind::Cmp: visitor.cmp_i(in(0), in(1)); break;
        case LIRInstKind::ParallelCopy: {
            const auto out0 = VReg::try_from(out(0));
            assertion(out0.has_value(), "invariant");

            std::vector<VReg> inputs;
            inputs.reserve(m_uses.size());
            for (auto& use: m_uses) {
                auto vreg = VReg::try_from(use);
                if (!vreg.has_value()) {
                    continue;
                }

                inputs.push_back(vreg.value());
            }

            visitor.parallel_copy(out0.value(), inputs);
        }
    }
}

LIRInstBuilder<LIRInstruction> LIRInstruction::copy(const LIROperand &op)  {
    return [=](std::size_t id, MachBlock *bb) {
        auto copy = std::make_unique<LIRInstruction>(id, bb, LIRInstKind::Copy, std::vector{op}, std::vector<VReg>{});
        copy->add_def(VReg(op.size(), 0, copy.get()));
        return copy;
    };
}

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

static std::vector<VReg> to_vregs_only(std::span<LIROperand const> inputs) {
    std::vector<VReg> vregs;
    for (const auto& in: inputs) {
        const auto vreg = VReg::try_from(in);
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
            const auto pointer = VReg::try_from(in(0));
            assertion(pointer.has_value(), "invariant");
            visitor.icall(out(0), pointer.value(), to_vregs_only(inputs().subspan(1)));
            break;
        }
        case LIRCallKind::IVCall: {
            const auto pointer = VReg::try_from(in(0));
            assertion(pointer.has_value(), "invariant");
            visitor.ivcall(pointer.value(), to_vregs_only(inputs().subspan(1)));
            break;
        }
        case LIRCallKind::VCall: visitor.vcall(to_vregs_only(inputs())); break;
    }
}


namespace {
    class LIRInsructionPrinter final: public LIRVisitor {
    public:
        explicit LIRInsructionPrinter(std::ostream& os): m_os(os) {}

    private:
        void add_i(const VReg &out, const LIROperand &in1, const LIROperand &in2) override {
            m_os << "add_i out(" << out << ") in(" << in1 << ", " << in2 << ')';
        }

        void sub_i(const VReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void mul_i(const VReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void div_i(const VReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void and_i(const VReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void or_i(const VReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void xor_i(const VReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shl_i(const VReg &out, const LIROperand &in1, const LIROperand &in2) override {}

        void shr_i(const VReg &out, const LIROperand &in1, const LIROperand &in2) override {

        }

        void parallel_copy(const VReg &out, std::span<VReg const> inputs) override {

        }

        void cmp_i(const LIROperand &in1, const LIROperand &in2) override {

        }

        void neg_i(const VReg &out, const LIROperand &in) override {

        }

        void not_i(const VReg &out, const LIROperand &in) override {

        }

        void mov_i(const VReg &out, const VReg &in) override {

        }

        void copy_i(const VReg &out, const LIROperand &in) override {
            m_os << "copy_i out(" << out << ") in(" << in << ')';
        }

        void jmp(const MachBlock *bb) override {

        }

        void je(const MachBlock *on_true, const MachBlock *on_false) override {

        }

        void jne(const MachBlock *on_true, const MachBlock *on_false) override {

        }

        void jl(const MachBlock *on_true, const MachBlock *on_false) override {

        }

        void jle(const MachBlock *on_true, const MachBlock *on_false) override {

        }

        void jg(const MachBlock *on_true, const MachBlock *on_false) override {

        }

        void jge(const MachBlock *on_true, const MachBlock *on_false) override {

        }

        void call(const VReg &out, std::span<VReg const> args) override {

        }

        void vcall(std::span<VReg const> args) override {

        }

        void icall(const VReg &out, const VReg &pointer, std::span<VReg const> args) override {

        }

        void ivcall(const VReg &pointer, std::span<VReg const> args) override {

        }

        void ret(std::span<LIROperand const> ret_values) override {
            m_os << "ret " << "in[";
            for (auto [idx, v_ret]: std::ranges::views::enumerate(ret_values)) {
                if (idx != 0) m_os << ", ";
                m_os << v_ret;
            }
            m_os << "]";
        }

        std::ostream& m_os;
    };
}

void LIRInstructionBase::print(std::ostream &os) const {
    LIRInsructionPrinter printer(os);
    const auto me = const_cast<LIRInstructionBase*>(this);
    me->visit(printer);
}