#include "Instruction.h"

#include <ostream>
#include <ranges>

#include "IntDiv.h"
#include "Projection.h"
#include "mir/module/BasicBlock.h"
#include "mir/types/Type.h"
#include "mir/types/StructType.h"
#include "mir/value/Value.h"
#include "mir/instruction/ValueInstruction.h"
#include "mir/instruction/Binary.h"
#include "mir/instruction/Unary.h"
#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/TerminateValueInstruction.h"
#include "mir/instruction/Compare.h"
#include "mir/instruction/GetElementPtr.h"
#include "mir/instruction/GetFieldPtr.h"
#include "mir/instruction/Phi.h"
#include "mir/instruction/Select.h"
#include "mir/instruction/Alloc.h"
#include "mir/instruction/Store.h"

#include "utility/Error.h"

namespace {
    class Printer final: public Visitor {
    public:
        explicit Printer(std::ostream& os) noexcept: os(os) {}

        void do_print(Instruction* inst) {
            inst->visit(*this);
        }

    private:
        void print_val(const Instruction* inst) const {
            os << '%' << inst->owner()->id() << 'x' << inst->id() << " = ";
        }

        static std::string_view binaryOpToString(const BinaryOp op) {
            switch (op) {
                case BinaryOp::Add: return "add";
                case BinaryOp::Subtract: return "sub";
                case BinaryOp::Multiply: return "multiply";
                case BinaryOp::Divide: return "divide";
                case BinaryOp::BitwiseAnd: return "and";
                case BinaryOp::BitwiseOr: return "or";
                case BinaryOp::BitwiseXor: return "xor";
                default: die("wrong type");
            }
        }

        void accept(Binary *inst) override {
            print_val(inst);
            os << binaryOpToString(inst->op());
            os << ' ';
            os << *inst->type();
            os << ' ' << inst->lhs() << ", " << inst->rhs();
        }

        static std::string_view unaryOpToString(const UnaryOp op) {
            switch (op) {
                case UnaryOp::Negate: return "neg";
                case UnaryOp::LogicalNot: return "not";
                case UnaryOp::Trunk: return "trunk";
                case UnaryOp::SignExtend: return "sext";
                case UnaryOp::ZeroExtend: return "zext";
                case UnaryOp::Ptr2Int: return "ptr2int";
                case UnaryOp::Flag2Int: return "flag2int";
                case UnaryOp::Int2Ptr: return "int2ptr";
                case UnaryOp::Int2Float: return "int2fp";
                case UnaryOp::Float2Int: return "fp2int";
                case UnaryOp::Load: return "load";
                case UnaryOp::Bitcast: return "bitcast";
                default: die("wrong type");
            }
        }

        void accept(Unary *inst) override {
            print_val(inst);
            os << unaryOpToString(inst->op());
            os << ' ' << *inst->type();
            os << ' ' << inst->operand();
        }

        void accept(Branch* branch) override {
            os << "br label %" << branch->target()->id();
        }

        void accept(CondBranch *cond_branch) override {
            os << "br_cond " << cond_branch->condition();
            os << ", label %" << cond_branch->on_true()->id();
            os << ", label %" << cond_branch->on_false()->id();
        }

        void accept(Phi *inst) override {
            print_val(inst);
            os << "phi " << *inst->type() << ' ' << '[';
            for (auto [idx, incoming, target] : std::views::zip(std::views::iota(0), inst->incoming(), inst->operands())) {
                if (idx++ != 0) {
                    os << ", ";
                }

                os << target  << ": ";
                incoming->print_short_name(os);
            }
            os << ']';
        }

        void accept(Return *inst) override {
            os << "ret void";
        }

        void accept(ReturnValue *inst) override {
            const auto ty = inst->ret_type();
            os << "ret " << *ty << " [" << inst->first();
            if (const auto second = inst->second()) {
                os << ", " << *second;
            }
            os << ']';
        }

        void accept(Call *inst) override {
            print_val(inst);
            os << "call " << *inst->prototype()->ret_type() << ' ';
            inst->prototype()->print(os, inst->operands());
            os << ' ';
            inst->cont()->print_short_name(os);
        }

        void accept(Switch *inst) override {

        }

        void accept(VCall *call) override {

        }

        void accept(IVCall *call) override {

        }

        void accept(Store *store) override {
            os << "store ptr " << store->pointer() << ", ";
            os << *store->value().type();
            os << ": " << store->value();
        }

        void accept(Alloc *alloc) override {
            print_val(alloc);
            os << "alloc ";
            os << *alloc->type();
        }

        static std::string_view icmpOpToString(const IcmpPredicate op) noexcept {
            switch (op) {
                case IcmpPredicate::Eq: return "eq";
                case IcmpPredicate::Ne: return "ne";
                case IcmpPredicate::Gt: return "gt";
                case IcmpPredicate::Ge: return "ge";
                case IcmpPredicate::Lt: return "lt";
                case IcmpPredicate::Le: return "le";
                default: die("wrong type");
            }
        }

        void accept(IcmpInstruction *icmp) override {
            print_val(icmp);
            os << "icmp " << *icmp->type();
            os << ' ' << icmpOpToString(icmp->predicate()) << ' ';
            os << icmp->lhs() << ", " << icmp->rhs();
        }

        void accept(GetElementPtr *gep) override {
            print_val(gep);
            os << "gep" << ' ';
            os << *gep->access_type();
            os << ' ' << gep->pointer() << ", " << gep->index();
        }

        void accept(GetFieldPtr *gfp) override {
            print_val(gfp);
            os << "gep" << ' ';
            os << *gfp->basic_type();
            os << ' ' << gfp->pointer() << ", " << gfp->index();
        }

        void accept(Select *select) override {
            print_val(select);
            os << "select ";
            os << *select->type();
            os << ' ' << select->condition() << ", ";
            os << select->on_true() << ", " << select->on_false();
        }

        void accept(IntDiv *div) override {
            print_val(div);
            os << "idiv ";
            os << *div->type();
            os << ' ' << div->lhs() << ", " << div->rhs();
        }

        void accept(Projection *proj) override {
            print_val(proj);
            os << "proj";
            os << ' ' << *proj->type();
            os << ' ' << proj->operand() << ": " << proj->idx();
        }

        std::ostream& os;
    };
}


void Instruction::print(std::ostream& os) const {
    Printer p(os);
    p.do_print(const_cast<Instruction *>(this));
}