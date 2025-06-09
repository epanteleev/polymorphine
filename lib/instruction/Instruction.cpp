#include "Instruction.h"

#include <iostream>

#include "../utility/Error.h"
#include "../types/Type.h"
#include "../value/Value.h"


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

        static std::string_view terminateOpToString(const TermInstType op) {
            switch (op) {
                case TermInstType::Return: return "ret";
                case TermInstType::Branch: return "br";
                case TermInstType::ConditionalBranch: return "br_cond";
                case TermInstType::Switch: return "switch";
                default: die("wrong type");
            }
        }

        void accept(TerminateInstruction *inst) override {

        }

        void accept(PhiInstruction *inst) override {

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
