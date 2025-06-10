#include "Instruction.h"

#include <iostream>

#include "BasicBlock.h"
#include "../utility/Error.h"
#include "types/Type.h"
#include "value/Value.h"
#include "instruction/ValueInstruction.h"
#include "instruction/Binary.h"
#include "instruction/Unary.h"
#include "instruction/TerminateInstruction.h"
#include "instruction/TerminateValueInstruction.h"


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
            inst->type()->print(os);
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
                case UnaryOp::Int2Ptr: return "int2ptr";
                case UnaryOp::Int2Float: return "int2fp";
                case UnaryOp::Float2Int: return "fp2int";
                default: die("wrong type");
            }
        }

        void accept(Unary *inst) override {
            os << '%' << inst->id() << " = ";
            os << unaryOpToString(inst->op());
            os << ' ';
            inst->type()->print(os);
            os << ' ' << inst->operand();
        }

        void accept(Branch* branch) override {
            os << "br label %" << branch->target()->id();
        }

        void accept(CondBranch *cond_branch) override {
            os << "br " << cond_branch->condition();
            os << ", label %" << cond_branch->onTrue()->id();
            os << ", label %" << cond_branch->onFalse()->id();
        }

        void accept(PhiInstruction *inst) override {

        }

        void accept(Return *inst) override {
            os << "ret void";
        }

        void accept(ReturnValue *inst) override {
            os << "ret ";
            inst->ret_value().type()->print(os);
            os << ' ' << inst->ret_value();
        }

        void accept(TerminateValueInstruction *inst) override {

        }

        void accept(Switch *inst) override {

        }

        void accept(VCall *call) override {

        }

        void accept(IVCall *call) override {

        }

        std::ostream& os;
    };
}


void Instruction::print(std::ostream& os) const {
    Printer p(os);
    p.do_print(const_cast<Instruction *>(this));
}

PhiInstruction::PhiInstruction(const std::size_t id, BasicBlock *bb, NonTrivialType *ty,
    const std::initializer_list<Value> &values, std::vector<BasicBlock *> targets)
        : ValueInstruction(id, bb, ty, values), m_entries(std::move(targets)) {}

TerminateValueInstruction::TerminateValueInstruction(const std::size_t id, BasicBlock *bb, NonTrivialType *ty,
                                                     const TermValueInstType type,
                                                     std::vector<BasicBlock *> &&successors)
    : ValueInstruction(id, bb, ty, {}),
    m_type(type),
    m_successors(std::move(successors)) {}
