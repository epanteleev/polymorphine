#pragma once

#include <memory>

#include "base/Constant.h"
#include "lir/x64/instruction/LIRProducerInstructionBase.h"

enum class LIRProdInstKind: std::uint8_t {
    Gen,
    Add,
    Sub,
    Mul,
    DivI,
    DivU,
    And,
    Or,
    Xor,
    Shl,
    Shr,
    Neg,
    Not,
    Copy,
    Load,
    LoadByIdx,
    ReadByOffset,
    Lea,
    Movz,
    Movs,
    Trunc,
    CvtFp2Int,
    CvtInt2Fp,
    CvtUInt2Fp,
};

class LIRProducerInstruction final: public LIRProducerInstructionBase {
public:
    explicit LIRProducerInstruction(const LIRProdInstKind kind, const LIRValType type, std::vector<LIROperand>&& uses) noexcept:
        LIRProducerInstructionBase(type, std::move(uses)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRProducerInstruction> copy(const std::uint8_t size, const LIROperand &op)  {
        return create(LIRProdInstKind::Copy, LIRValType::GP, size, size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> copy_f(const std::uint8_t size, const LIROperand &op)  {
        return create(LIRProdInstKind::Copy, LIRValType::FP, size, size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> copy(const std::uint8_t size, const LIROperand &op, const aasm::GPReg fixed_reg)  {
        auto prod = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::Copy, LIRValType::GP, std::vector{op});
        prod->add_def(LIRVal::reg(size, size, 0, prod.get()));
        prod->assign_reg(0, fixed_reg);
        return prod;
    }

    static std::unique_ptr<LIRProducerInstruction> copy_f(const std::uint8_t size, const LIROperand &op, const aasm::XmmReg fixed_reg)  {
        auto prod = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::Copy, LIRValType::FP, std::vector{op});
        prod->add_def(LIRVal::reg(size, size, 0, prod.get()));
        prod->assign_reg(0, fixed_reg);
        return prod;
    }

    static std::unique_ptr<LIRProducerInstruction> add(const LIRValType type, const LIROperand &lhs, const LIROperand &rhs) {
        return create(LIRProdInstKind::Add, type, lhs.size(), lhs.size(), lhs, rhs);
    }

    static std::unique_ptr<LIRProducerInstruction> sub(const LIRValType type, const LIROperand &lhs, const LIROperand &rhs) {
        return create(LIRProdInstKind::Sub, type, lhs.size(), lhs.size(), lhs, rhs);
    }

    static std::unique_ptr<LIRProducerInstruction> xxor(const LIROperand &lhs, const LIROperand &rhs) {
        return create(LIRProdInstKind::Xor, LIRValType::GP, lhs.size(), lhs.size(), lhs, rhs);
    }

    static std::unique_ptr<LIRProducerInstruction> idiv(const LIROperand &lhs, const LIROperand &rhs) {
        auto idiv = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::DivI, LIRValType::GP, std::vector{lhs, rhs});
        idiv->add_def(LIRVal::reg(lhs.size(), lhs.align(), 0, idiv.get()));
        idiv->add_def(LIRVal::reg(lhs.size(), lhs.align(), 1, idiv.get()));
        return idiv;
    }

    static std::unique_ptr<LIRProducerInstruction> udiv(const LIROperand &lhs, const LIROperand &rhs) {
        auto udiv = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::DivU, LIRValType::GP, std::vector{lhs, rhs});
        udiv->add_def(LIRVal::reg(lhs.size(), lhs.align(), 0, udiv.get()));
        udiv->add_def(LIRVal::reg(lhs.size(), lhs.align(), 1, udiv.get()));
        return udiv;
    }

    static std::unique_ptr<LIRProducerInstruction> gen(const std::uint8_t size, const std::uint8_t align) {
        auto gen = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::Gen, LIRValType::GP, std::vector<LIROperand>{});
        gen->add_def(LIRVal::reg(size, align, 0, gen.get()));
        return gen;
    }

    static std::unique_ptr<LIRProducerInstruction> load(const LIRValType type, const std::uint8_t loaded_ty_size, const LIROperand &op) {
        return create(LIRProdInstKind::Load, type, loaded_ty_size, loaded_ty_size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> load_by_idx(const LIRValType type, const std::uint8_t loaded_ty_size, const LIROperand &pointer, const LIROperand &index) {
        return create(LIRProdInstKind::LoadByIdx, type, loaded_ty_size, loaded_ty_size, pointer, index);
    }

    static std::unique_ptr<LIRProducerInstruction> read_by_offset(const LIRValType type, const std::uint8_t loaded_ty_size, const LIROperand &pointer, const LIROperand &index) {
        return create(LIRProdInstKind::ReadByOffset, type, loaded_ty_size, loaded_ty_size, pointer, index);
    }

    static std::unique_ptr<LIRProducerInstruction> lea(const std::uint8_t size, const LIROperand &pointer, const LIROperand &index) {
        auto lea = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::Lea, LIRValType::GP, std::vector{pointer, index});
        lea->add_def(LIRVal::reg(size, cst::POINTER_SIZE, 0, lea.get()));
        return lea;
    }

    static std::unique_ptr<LIRProducerInstruction> lea(const std::uint8_t size, const LIROperand &pointer, const LIROperand &index, const aasm::GPReg fixed_reg) {
        auto lea = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::Lea, LIRValType::GP, std::vector{pointer, index});
        lea->add_def(LIRVal::reg(size, cst::POINTER_SIZE, 0, lea.get()));
        lea->assign_reg(0, fixed_reg);
        return lea;
    }

    static std::unique_ptr<LIRProducerInstruction> movzx(const std::uint8_t to_size, const LIROperand &op) {
        return create(LIRProdInstKind::Movz, LIRValType::GP, to_size, to_size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> movsx(const std::uint8_t to_size, const LIROperand &op) {
        return create(LIRProdInstKind::Movs, LIRValType::GP, to_size, to_size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> trunc(const std::uint8_t to_size, const LIROperand &op) {
        return create(LIRProdInstKind::Trunc, LIRValType::GP, to_size, to_size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> cvtfp2int(const std::uint8_t to_size, const LIROperand &op) {
        return create(LIRProdInstKind::CvtFp2Int, LIRValType::FP, to_size, to_size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> cvtint2fp(const std::uint8_t to_size, const LIROperand &op) {
        return create(LIRProdInstKind::CvtInt2Fp, LIRValType::GP, to_size, to_size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> cvtuint2fp(const std::uint8_t to_size, const LIROperand &op) {
        return create(LIRProdInstKind::CvtUInt2Fp, LIRValType::FP, to_size, to_size, op);
    }

    [[nodiscard]]
    LIRProdInstKind op() const noexcept {
        return m_kind;
    }

private:
    template<typename... Args>
    static std::unique_ptr<LIRProducerInstruction> create(LIRProdInstKind kind, const LIRValType type, const std::size_t size, const std::size_t align, Args&&... args) {
        auto prod = std::make_unique<LIRProducerInstruction>(kind, type, std::vector{std::forward<Args>(args)...});
        prod->add_def(LIRVal::reg(size, align, 0, prod.get()));
        return prod;
    }

    const LIRProdInstKind m_kind;
};