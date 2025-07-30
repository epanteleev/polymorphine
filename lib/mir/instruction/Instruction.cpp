#include "Instruction.h"

#include <iostream>

#include "mir/instruction/Alloc.h"
#include "mir/module/BasicBlock.h"
#include "mir/instruction/Store.h"
#include "utility/Error.h"
#include "mir/types/Type.h"
#include "mir/value/Value.h"
#include "mir/instruction/ValueInstruction.h"
#include "mir/instruction/Binary.h"
#include "mir/instruction/Unary.h"
#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/TerminateValueInstruction.h"
#include "mir/instruction/Compare.h"
#include "mir/instruction/GetElementPtr.h"


namespace {
    class Printer final: public Visitor {
    public:
        explicit Printer(std::ostream& os) : os(os) {}

        void do_print(Instruction* inst) {
            inst->visit(*this);
        }

    private:
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
            os << '%' << inst->id() << " = ";
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
                default: die("wrong type");
            }
        }

        void accept(Unary *inst) override {
            os << '%' << inst->id() << " = ";
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

        void accept(PhiInstruction *inst) override {

        }

        void accept(Return *inst) override {
            os << "ret void";
        }

        void accept(ReturnValue *inst) override {
            os << "ret ";
            os << *inst->ret_value().type();
            os << ' ' << inst->ret_value();
        }

        void accept(Call *inst) override {
            os << "call ";
            inst->prototype().print(os, inst->operands());


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
            os << '%' << alloc->id() << " = alloc ";
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
            os << '%' << icmp->id() << " = icmp ";
            os << *icmp->type();
            os << ' ' << icmpOpToString(icmp->predicate()) << ' ';
            os << icmp->lhs() << ", " << icmp->rhs();
        }

        void accept(GetElementPtr *gep) override {
            os << '%' << gep->id() << " = gep ";
            os << *gep->access_type();
            os << ' ' << gep->pointer() << ", " << gep->index();
        }

        std::ostream& os;
    };
}


void Instruction::print(std::ostream& os) const {
    Printer p(os);
    p.do_print(const_cast<Instruction *>(this));
}

PhiInstruction::PhiInstruction(NonTrivialType *ty,
                               const std::initializer_list<Value> &values, std::vector<BasicBlock *> targets)
        : ValueInstruction(ty, values), m_entries(std::move(targets)) {}
