#include "FoldInstruction.h"

#include "mir/instruction/Binary.h"
#include "mir/instruction/Icmp.h"
#include "mir/instruction/InstructionVisitor.h"
#include "mir/instruction/Unary.h"

namespace {
    class FoldInstructionImpl final: public Visitor {
    public:
        [[nodiscard]]
        std::optional<VConstant> try_fold(const Instruction &inst) noexcept {
            inst.visit(*this);
            return m_constant;
        }

        void accept(Binary *inst) override {
            const auto lhr = VConstant::try_from(inst->lhs());
            if (!lhr.has_value()) {
                return;
            }

            const auto rhr = VConstant::try_from(inst->rhs());
            if (!rhr.has_value()) {
                return;
            }

            switch (inst->op()) {
                case BinaryOp::Add:      m_constant = VConstant::sum(lhr.value(), rhr.value()); break;
                case BinaryOp::Subtract: m_constant = VConstant::sub(lhr.value(), rhr.value()); break;
                default: break;
            }
        }

        void accept(Unary *inst) override {
            const auto op = VConstant::try_from(inst->operand());
            if (!op.has_value()) {
                return;
            }

            // TODO
        }

        void accept(Branch *branch) override {

        }

        void accept(CondBranch *cond_branch) override {

        }

        void accept(Call *inst) override {

        }

        void accept(TupleCall *inst) override {

        }

        void accept(Return *inst) override {

        }

        void accept(ReturnValue *inst) override {

        }

        void accept(Switch *inst) override {

        }

        void accept(VCall *call) override {

        }

        void accept(IVCall *call) override {

        }

        void accept(Phi *inst) override {

        }

        void accept(Store *store) override {

        }

        void accept(Alloc *alloc) override {

        }

        void accept(IcmpInstruction *icmp) override {
            const auto lhr = VConstant::try_from(icmp->lhs());
            if (!lhr.has_value()) {
                return;
            }

            const auto rhr = VConstant::try_from(icmp->rhs());
            if (!rhr.has_value()) {
                return;
            }

            switch (icmp->predicate()) {
                case IcmpPredicate::Eq: m_constant = VConstant::eq(lhr.value(), rhr.value()); break;
                case IcmpPredicate::Ne: m_constant = VConstant::ne(lhr.value(), rhr.value()); break;
                case IcmpPredicate::Lt: m_constant = VConstant::lt(lhr.value(), rhr.value()); break;
                case IcmpPredicate::Le: m_constant = VConstant::le(lhr.value(), rhr.value()); break;
                case IcmpPredicate::Gt: m_constant = VConstant::gt(lhr.value(), rhr.value()); break;
                case IcmpPredicate::Ge: m_constant = VConstant::ge(lhr.value(), rhr.value()); break;
                default: std::unreachable();
            }
        }

        void accept(FcmpInstruction *fcmp) override {

        }

        void accept(GetElementPtr *gep) override {

        }

        void accept(GetFieldPtr *gfp) override {

        }

        void accept(Select *select) override {

        }

        void accept(IntDiv *div) override {

        }

        void accept(Projection *proj) override {

        }

    private:
        std::optional<VConstant> m_constant;
    };
}

std::optional<VConstant> FoldInstruction::try_fold(const Instruction &inst) noexcept {
    FoldInstructionImpl fold_inst;
    return fold_inst.try_fold(inst);
}