#include <ranges>
#include <algorithm>

#include "Verifier.h"
#include "mir/types/FlagType.h"
#include "mir/instruction/Instruction.h"
#include "mir/instruction/Icmp.h"
#include "mir/instruction/Fcmp.h"
#include "mir/instruction/Binary.h"
#include "mir/instruction/Alloc.h"
#include "mir/instruction/Phi.h"
#include "mir/instruction/Store.h"
#include "mir/instruction/IntDiv.h"
#include "mir/value/UsedValue.h"

class InstructionVerifier final: public Visitor {
public:
    [[nodiscard]]
    static std::optional<VerifierResult> apply(const FunctionPrototype* proto, const Instruction* inst) {
        InstructionVerifier instVer(proto, inst);
        if (auto valid_du = instVer.verify_def_use_chain(); valid_du.has_value()) {
            return std::move(valid_du.value());
        }
        const_cast<Instruction*>(inst)->visit(instVer);
        return std::move(instVer.m_correct);
    }

private:
    friend class Visitor;

    explicit InstructionVerifier(const FunctionPrototype* proto, const Instruction* inst) noexcept:
        m_inst(inst),
        m_prototype(proto) {}

    void raise_type_error(const Type *a_type, const Type *b_type) {
        m_correct.emplace(VerifierResult::wrong_type(m_prototype, m_inst->location(), a_type, b_type));
    }

    void raise_type_error(const Type *a_type) {
        m_correct.emplace(VerifierResult::wrong_type(m_prototype, m_inst->location(), a_type));
    }

    void raise_cfg_error(const BasicBlock* bb, const BasicBlock* pred) {
        m_correct.emplace(VerifierResult::invalid_cfg(m_prototype, m_inst->location(), bb->id(), pred->id()));
    }

    void raise_terminator_error(const BasicBlock* bb) {
        m_correct.emplace(VerifierResult::invalid_terminator(m_prototype, m_inst->location(), bb->id()));
    }

    [[nodiscard]]
    std::optional<VerifierResult> verify_def_use_chain() const {
        for (const auto& operand: m_inst->operands()) {
            const auto local = UsedValue::try_from(operand);
            if (!local.has_value()) {
                continue;
            }

            if (const auto& users = local.value().users(); std::ranges::contains(users, m_inst)) {
                continue;
            }

            return VerifierResult::invalid_du(m_prototype, m_inst->location());
        }
        return std::nullopt;
    }

    bool verify_cfg() {
        const auto bb = m_inst->owner();
        for (const auto pred: bb->successors()) {
            if (!std::ranges::contains(pred->predecessors(), bb)) {
                raise_cfg_error(bb, pred);
                return true;
            }
        }
        return false;
    }

    bool verify_return() {
        if (const auto bb = m_inst->owner(); !bb->successors().empty()) {
            raise_terminator_error(bb);
            return true;
        }

        return false;
    }

    template<typename InTy, typename B>
    void validate_binary(const B* inst) {
        const auto a_type = inst->lhs().type();
        const auto b_type = inst->rhs().type();
        if (InTy::cast(a_type) == nullptr) {
            raise_type_error(a_type, b_type);
            return;
        }
        if (a_type != b_type) {
            raise_type_error(a_type, b_type);
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
        if (const auto ty = inst->type(); OutTy::cast(ty) == nullptr) {
            raise_type_error(ty);
            return true;
        }

        return false;
    }

    void validate_neg_not(const Unary* inst) {
        if (check_out_type<IntegerType>(inst)) return;

        if (const auto ty = inst->type(); ty != inst->operand().type()) {
            raise_type_error(ty);
        }
    }

    void check_operand_size(const Unary* inst ) {
        const auto ty = inst->type();
        if (SignedIntegerType::cast(ty) == nullptr && SignedIntegerType::cast(ty) != nullptr) {
            raise_type_error(ty);
            return;
        }

        if (UnsignedIntegerType::cast(ty) == nullptr && UnsignedIntegerType::cast(ty) != nullptr) {
            raise_type_error(ty);
        }
    }

    void validate_trunk(const Unary* inst) {
        if (check_out_type<IntegerType>(inst)) return;
        const auto ty = IntegerType::cast(inst->type());

        const auto op_type = IntegerType::cast(inst->operand().type());
        if (op_type == nullptr || op_type->size_of() <= ty->size_of()) {
            raise_type_error(ty);
            return;
        }

        check_operand_size(inst);
    }

    void validate_bitcast(const Unary* inst) {
        if (check_out_type<IntegerType>(inst)) return;
        const auto ty = IntegerType::cast(inst->type());

        const auto op_type = IntegerType::cast(inst->operand().type());
        if (op_type == nullptr || op_type->size_of() != ty->size_of()) {
            raise_type_error(ty);
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
            raise_type_error(ty);
        }
    }

    template<typename To, typename From>
    void validate_unary_from_to(const Unary* inst) {
        if (check_out_type<To>(inst)) return;
        const auto ty = To::cast(inst->type());

        const auto op_type = From::cast(inst->operand().type());
        if (op_type == nullptr || op_type->size_of() != ty->size_of()) {
            raise_type_error(ty);
        }
    }

    template<typename To, typename From>
    void validate_unary_from_to0(const Unary* inst) {
        if (check_out_type<To>(inst)) return;
        const auto ty = To::cast(inst->type());

        const auto op_type = From::cast(inst->operand().type());
        if (op_type == nullptr) {
            raise_type_error(ty);
        }
    }

    void validate_flag2int(const Unary *inst) {
        if (check_out_type<IntegerType>(inst)) return;
        const auto op_type = FlagType::cast(inst->operand().type());
        if (op_type == nullptr) {
            raise_type_error(inst->type());
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
        verify_cfg();
    }

    void accept(CondBranch *cond_branch) override {
        const auto cond = cond_branch->condition().type();
        if (FlagType::cast(cond) == nullptr) {
            raise_type_error(cond);
        }
    }

    void accept(Call *inst) override {
        verify_cfg();
    }

    void accept(TupleCall *inst) override {
        verify_cfg();
    }

    void accept(Return *inst) override {
        verify_return();
    }

    void accept(ReturnValue *inst) override {
        verify_return();
    }

    void accept(Switch *inst) override {
        verify_cfg();
    }

    void accept(VCall *call) override {
        verify_cfg();
    }

    void accept(IVCall *call) override {
        verify_cfg();
    }

    void accept(Phi *inst) override {
        const auto out_type = inst->type();
        for (const auto& incoming: inst->operands()) {
            if (const auto incoming_ty = incoming.type(); incoming_ty != out_type) {
                raise_type_error(out_type, incoming_ty);
                return;
            }
        }
    }

    void accept(Store *store) override {
        const auto value_ty = store->value().type();
        if (PrimitiveType::cast(value_ty) == nullptr) {
            raise_type_error(value_ty);
            return;
        }

        if (const auto ptr_type = PointerType::cast(store->pointer().type()); ptr_type == nullptr) {
            raise_type_error(ptr_type);
        }
    }

    void accept(Alloc *alloc) override {
        const auto out_ty = alloc->type();
        if (VoidType::cast(out_ty) != nullptr || FlagType::cast(out_ty) != nullptr) {
            raise_type_error(out_ty);
        }
    }

    void accept(IcmpInstruction *icmp) override {
        const auto a_type = icmp->lhs().type();
        const auto b_type = icmp->rhs().type();
        if (a_type != b_type && a_type == PointerType::ptr()) {
            raise_type_error(a_type, b_type);
            return;
        }

        validate_binary<IntegerType>(icmp);
    }

    void accept(FcmpInstruction *fcmp) override {
        validate_binary<FloatingPointType>(fcmp);
    }

    void accept(GetElementPtr *gep) override {
    }

    void accept(GetFieldPtr *gfp) override {
    }

    void accept(Select *select) override {
    }

    void accept(IntDiv *div) override {
        validate_binary<IntegerType>(div);
    }

    void accept(Projection *proj) override {
    }

    std::optional<VerifierResult> m_correct{};
    const Instruction* m_inst;
    const FunctionPrototype* m_prototype;
};

std::optional<VerifierResult> Verifier::apply(const Module &module) {
    for (const auto& fn: std::ranges::views::values(module.functions())) {
        for (const auto& bb: fn.basic_blocks()) {
            for (const auto& inst: bb.instructions()) {
                if (auto res = InstructionVerifier::apply(fn.prototype(), &inst); res.has_value()) {
                    return res;
                }
            }
        }
    }

    return std::nullopt;
}