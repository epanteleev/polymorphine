#pragma once

#include "lir/x64/instruction/LIRProducerInstructionBase.h"

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
};

class LIRProducerInstruction final: public LIRProducerInstructionBase {
public:
    LIRProducerInstruction(const LIRProdInstKind kind, std::vector<LIROperand>&& uses) :
        LIRProducerInstructionBase(std::move(uses)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRProducerInstruction> copy(const std::uint8_t size, const LIROperand &op)  {
        return create(LIRProdInstKind::Copy, size, op);
    }

    static std::unique_ptr<LIRProducerInstruction> add(const LIROperand &lhs, const LIROperand &rhs) {
        return create(LIRProdInstKind::Add, lhs.size(), lhs, rhs);
    }

    static std::unique_ptr<LIRProducerInstruction> sub(const LIROperand &lhs, const LIROperand &rhs) {
        return create(LIRProdInstKind::Sub, lhs.size(), lhs, rhs);
    }

    static std::unique_ptr<LIRProducerInstruction> gen(const std::uint8_t size) {
        auto gen = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::Gen, std::vector<LIROperand>{});
        gen->add_def(LIRVal::reg(size, 0, gen.get()));
        return gen;
    }

    static std::unique_ptr<LIRProducerInstruction> load(const std::uint8_t loaded_ty_size, const LIROperand &op) {
        auto load = std::make_unique<LIRProducerInstruction>(LIRProdInstKind::Load, std::vector{op});
        load->add_def(LIRVal::reg(loaded_ty_size, 0, load.get()));
        return load;
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
