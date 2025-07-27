#pragma once

#include "LIRProducerInstructionBase.h"

enum class LIRProdInstKind: std::uint8_t {
    Gen,
    Add,
    Sub,
    Mul,
    Div,
    And,
    Or,
    Xor,
    Shl,
    Shr,
    Neg,
    Not,
    ParallelCopy,
    Copy,
    Load,
    Cmp,
};

class LIRProducerInstruction final: public LIRProducerInstructionBase {
public:
    LIRProducerInstruction(const std::size_t id, LIRBlock *bb, const LIRProdInstKind kind, std::vector<LIROperand>&& uses) :
        LIRProducerInstructionBase(id, bb, std::move(uses)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

    static LIRInstBuilder<LIRProducerInstruction> copy(const std::uint8_t size, const LIROperand &op)  {
        return [=](std::size_t id, LIRBlock *bb) {
            auto copy = std::make_unique<LIRProducerInstruction>(id, bb, LIRProdInstKind::Copy, std::vector{op});
            copy->add_def(LIRVal::reg(size, 0, copy.get()));
            return copy;
        };
    }

    static LIRInstBuilder<LIRProducerInstruction> add(const LIROperand &lhs, const LIROperand &rhs) {
        return [=](std::size_t id, LIRBlock *bb) {
            auto add = std::make_unique<LIRProducerInstruction>(id, bb, LIRProdInstKind::Add, std::vector{lhs, rhs});
            add->add_def(LIRVal::reg(lhs.size(), 0, add.get()));
            return add;
        };
    }

    static LIRInstBuilder<LIRProducerInstruction> cmp(const LIROperand &lhs, const LIROperand &rhs) {
        return [=](std::size_t id, LIRBlock *bb) {
            auto cmp = std::make_unique<LIRProducerInstruction>(id, bb, LIRProdInstKind::Cmp, std::vector{lhs, rhs});
            cmp->add_def(LIRVal::reg(1, 0, cmp.get()));
            return cmp;
        };
    }

    static LIRInstBuilder<LIRProducerInstruction> gen(const std::uint8_t size) {
        return [=](std::size_t id, LIRBlock *bb) {
            auto gen = std::make_unique<LIRProducerInstruction>(id, bb, LIRProdInstKind::Gen, std::vector<LIROperand>{});
            gen->add_def(LIRVal::reg(size, 0, gen.get()));
            return gen;
        };
    }

    static LIRInstBuilder<LIRProducerInstruction> load(const std::uint8_t loaded_ty_size, const LIROperand &op) {
        return [=](std::size_t id, LIRBlock *bb) {
            auto load = std::make_unique<LIRProducerInstruction>(id, bb, LIRProdInstKind::Load, std::vector{op});
            load->add_def(LIRVal::reg(loaded_ty_size, 0, load.get()));
            return load;
        };
    }

    [[nodiscard]]
    LIRProdInstKind op() const noexcept {
        return m_kind;
    }

private:
    const LIRProdInstKind m_kind;
};
