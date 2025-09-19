#pragma once

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
    LoadFromStack,
    Lea,
    Movz,
    Movs,
    Trunc,
};

class LIRProducerInstruction final: public LIRProducerInstructionBase {
public:
    explicit LIRProducerInstruction(const LIRProdInstKind kind, std::vector<LIROperand>&& uses) noexcept:
        LIRProducerInstructionBase(std::move(uses)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRProducerInstruction> copy(const std::uint8_t size, const LIROperand &op)  {
        return create(LIRProdInstKind::Copy, size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> copy(const std::uint8_t size, const LIROperand &op, const aasm::GPReg fixed_reg)  {
        auto prod = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::Copy, std::vector{op});
        prod->add_def(LIRVal::reg(size, 0, prod.get()));
        prod->assign_reg(0, fixed_reg);
        return prod;
    }

    static std::unique_ptr<LIRProducerInstruction> add(const LIROperand &lhs, const LIROperand &rhs) {
        return create(LIRProdInstKind::Add, lhs.size(), lhs, rhs);
    }

    static std::unique_ptr<LIRProducerInstruction> sub(const LIROperand &lhs, const LIROperand &rhs) {
        return create(LIRProdInstKind::Sub, lhs.size(), lhs, rhs);
    }

    static std::unique_ptr<LIRProducerInstruction> xxor(const LIROperand &lhs, const LIROperand &rhs) {
        return create(LIRProdInstKind::Xor, lhs.size(), lhs, rhs);
    }

    static std::unique_ptr<LIRProducerInstruction> idiv(const LIROperand &lhs, const LIROperand &rhs) {
        auto idiv = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::DivI, std::vector{lhs, rhs});
        idiv->add_def(LIRVal::reg(lhs.size(), 0, idiv.get()));
        idiv->add_def(LIRVal::reg(lhs.size(), 1, idiv.get()));
        return idiv;
    }

    static std::unique_ptr<LIRProducerInstruction> udiv(const LIROperand &lhs, const LIROperand &rhs) {
        auto udiv = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::DivU, std::vector{lhs, rhs});
        udiv->add_def(LIRVal::reg(lhs.size(), 0, udiv.get()));
        udiv->add_def(LIRVal::reg(lhs.size(), 1, udiv.get()));
        return udiv;
    }

    static std::unique_ptr<LIRProducerInstruction> gen(const std::uint8_t size) {
        auto gen = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::Gen, std::vector<LIROperand>{});
        gen->add_def(LIRVal::reg(size, 0, gen.get()));
        return gen;
    }

    static std::unique_ptr<LIRProducerInstruction> load(const std::uint8_t loaded_ty_size, const LIROperand &op) {
        return create(LIRProdInstKind::Load, loaded_ty_size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> load_by_idx(const std::uint8_t loaded_ty_size, const LIROperand &pointer, const LIROperand &index) {
        return create(LIRProdInstKind::LoadByIdx, loaded_ty_size, pointer, index);
    }

    static std::unique_ptr<LIRProducerInstruction> load_from_stack(const std::uint8_t loaded_ty_size, const LIROperand &pointer, const LIROperand &index) {
        return create(LIRProdInstKind::LoadFromStack, loaded_ty_size, pointer, index);
    }

    static std::unique_ptr<LIRProducerInstruction> lea(const std::uint8_t size, const LIROperand &pointer, const LIROperand &index) {
        auto lea = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::Lea, std::vector{pointer, index});
        lea->add_def(LIRVal::reg(size, 0, lea.get()));
        return lea;
    }

    static std::unique_ptr<LIRProducerInstruction> lea(const std::uint8_t size, const LIROperand &pointer, const LIROperand &index, const aasm::GPReg fixed_reg) {
        auto lea = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::Lea, std::vector{pointer, index});
        lea->add_def(LIRVal::reg(size, 0, lea.get()));
        lea->assign_reg(0, fixed_reg);
        return lea;
    }

    static std::unique_ptr<LIRProducerInstruction> movzx(const std::uint8_t to_size, const LIROperand &op) {
        return create(LIRProdInstKind::Movz, to_size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> movsx(const std::uint8_t to_size, const LIROperand &op) {
        return create(LIRProdInstKind::Movs, to_size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> trunc(const std::uint8_t to_size, const LIROperand &op) {
        return create(LIRProdInstKind::Trunc, to_size, op);
    }

    [[nodiscard]]
    LIRProdInstKind op() const noexcept {
        return m_kind;
    }

private:
    template<typename ... Args>
    static std::unique_ptr<LIRProducerInstruction> create(LIRProdInstKind kind, const std::uint8_t size, Args&&... args) {
        auto prod = std::make_unique<LIRProducerInstruction>(kind, std::vector{std::forward<Args>(args)...});
        prod->add_def(LIRVal::reg(size, 0, prod.get()));
        return prod;
    }

    const LIRProdInstKind m_kind;
};