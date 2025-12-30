#include "Verifier.h"
#include "mir/instruction/Instruction.h"
#include "mir/instruction/Binary.h"
#include "mir/types/FlagType.h"

#include <ranges>

#include "mir/instruction/Alloc.h"
#include "mir/instruction/Phi.h"
#include "mir/instruction/Store.h"

class InstructionVerifier final: public Visitor {
public:
    [[nodiscard]]
    static std::optional<VerifierResult> apply(const Instruction* inst) {
        InstructionVerifier instVer;
        const_cast<Instruction*>(inst)->visit(instVer);
        return std::move(instVer.m_correct);
    }

private:
    friend class Visitor;

    template<typename InTy>
    void validate_binary(const Binary* inst) {
        const auto a_type = inst->lhs().type();
        const auto b_type = inst->rhs().type();
        if (InTy::cast(a_type) == nullptr) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), a_type, b_type));
            return;
        }
        if (a_type != b_type) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), a_type, b_type));
        }
    }

    void accept(Binary *inst) override {
        switch (inst->op()) {
            case BinaryOp::Add:        [[fallthrough]];
            case BinaryOp::Subtract:   [[fallthrough]];
            case BinaryOp::Multiply:   validate_binary<ArithmeticType>(inst); break;
            case BinaryOp::Divide:     validate_binary<FloatingPointType>(inst); break;
            case BinaryOp::BitwiseAnd: [[fallthrough]];
            case BinaryOp::BitwiseOr:  [[fallthrough]];
            case BinaryOp::BitwiseXor: [[fallthrough]];
            case BinaryOp::ShiftLeft:  [[fallthrough]];
            case BinaryOp::ShiftRight: validate_binary<IntegerType>(inst); break;
            default: std::unreachable();
        }
    }

    template<typename OutTy>
    [[nodiscard]]
    bool check_out_type(const Unary* inst) {
        const auto ty = inst->type();
        if (OutTy::cast(ty) == nullptr) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), ty));
            return true;
        }

        return false;
    }

    void validate_neg_not(const Unary* inst) {
        if (check_out_type<IntegerType>(inst)) return;

        if (const auto ty = inst->type(); ty != inst->operand().type()) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), ty));
        }
    }

    void check_operand_size(const Unary* inst ) {
        const auto ty = inst->type();
        if (SignedIntegerType::cast(ty) == nullptr && SignedIntegerType::cast(ty) != nullptr) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), ty));
            return;
        }

        if (UnsignedIntegerType::cast(ty) == nullptr && UnsignedIntegerType::cast(ty) != nullptr) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), ty));
        }
    }

    void validate_trunk(const Unary* inst) {
        if (check_out_type<IntegerType>(inst)) return;
        const auto ty = IntegerType::cast(inst->type());

        const auto op_type = IntegerType::cast(inst->operand().type());
        if (op_type == nullptr || op_type->size_of() <= ty->size_of()) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), ty));
            return;
        }

        check_operand_size(inst);
    }

    void validate_bitcast(const Unary* inst) {
        if (check_out_type<IntegerType>(inst)) return;
        const auto ty = IntegerType::cast(inst->type());

        const auto op_type = IntegerType::cast(inst->operand().type());
        if (op_type == nullptr || op_type->size_of() != ty->size_of()) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), ty));
            return;
        }

        check_operand_size(inst);
    }

    template<typename IntTy>
    void validate_ext(const Unary* inst) {
        if (check_out_type<IntegerType>(inst)) return;
        const auto ty = IntTy::cast(inst->type());

        const auto op_type = IntTy::cast(inst->operand().type());
        if (op_type == nullptr || op_type->size_of() >= ty->size_of()) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), ty));
        }
    }

    template<typename To, typename From>
    void validate_unary_from_to(const Unary* inst) {
        if (check_out_type<To>(inst)) return;
        const auto ty = To::cast(inst->type());

        const auto op_type = From::cast(inst->operand().type());
        if (op_type == nullptr || op_type->size_of() != ty->size_of()) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), ty));
        }
    }

    template<typename To, typename From>
    void validate_unary_from_to0(const Unary* inst) {
        if (check_out_type<To>(inst)) return;
        const auto ty = To::cast(inst->type());

        const auto op_type = From::cast(inst->operand().type());
        if (op_type == nullptr) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), ty));
        }
    }

    void validate_flag2int(const Unary *inst) {
        if (check_out_type<IntegerType>(inst)) return;
        const auto op_type = FlagType::cast(inst->operand().type());
        if (op_type == nullptr) {
            m_correct.emplace(VerifierResult::wrong_type(inst->location(), inst->type()));
        }
    }

    void accept(Unary *inst) override {
        switch (inst->op()) {
            case UnaryOp::Negate:     [[fallthrough]];
            case UnaryOp::LogicalNot: validate_neg_not(inst); break;
            case UnaryOp::Trunk:      validate_trunk(inst); break;
            case UnaryOp::Bitcast:    validate_bitcast(inst); break;
            case UnaryOp::SignExtend: validate_ext<SignedIntegerType>(inst); break;
            case UnaryOp::ZeroExtend: validate_ext<UnsignedIntegerType>(inst); break;
            case UnaryOp::Ptr2Int:    validate_unary_from_to<IntegerType, PointerType>(inst); break;
            case UnaryOp::Flag2Int:   validate_flag2int(inst); break;
            case UnaryOp::Int2Ptr:    validate_unary_from_to<PointerType, IntegerType>(inst); break;
            case UnaryOp::Int2Float:  validate_unary_from_to0<FloatingPointType, IntegerType>(inst); break;
            case UnaryOp::Float2Int:  validate_unary_from_to0<IntegerType, FloatingPointType>(inst); break;
            case UnaryOp::Load:       validate_unary_from_to0<PrimitiveType, PointerType>(inst); break;
            default: std::unreachable();
        }
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
        const auto out_type = inst->type();
        for (const auto& incoming: inst->operands()) {
            if (const auto incoming_ty = incoming.type(); incoming_ty != out_type) {
                m_correct.emplace(VerifierResult::wrong_type(inst->location(), out_type, incoming_ty));
                return;
            }
        }
    }

    void accept(Store *store) override {
        const auto value_ty = store->value().type();
        if (PrimitiveType::cast(value_ty) == nullptr) {
            m_correct.emplace(VerifierResult::wrong_type(store->location(), value_ty));
            return;
        }

        const auto pointer_type = PointerType::cast(store->pointer().type());
        if (pointer_type == nullptr) {
            m_correct.emplace(VerifierResult::wrong_type(store->location(), pointer_type));
        }
    }

    void accept(Alloc *alloc) override {
        const auto out_ty = alloc->type();
        if (VoidType::cast(out_ty) != nullptr || FlagType::cast(out_ty) != nullptr) {
            m_correct.emplace(VerifierResult::wrong_type(alloc->location(), out_ty));
        }
    }

    void accept(IcmpInstruction *icmp) override {
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

    std::optional<VerifierResult> m_correct{};
};

std::optional<VerifierResult> Verifier::apply(const Module &module) {
    for (const auto& fn: std::ranges::views::values(module.functions())) {
        for (const auto& bb: fn.basic_blocks()) {
            for (const auto& inst: bb.instructions()) {
                if (auto res = InstructionVerifier::apply(&inst); res.has_value()) {
                    return res;
                }
            }
        }
    }

    return std::nullopt;
}