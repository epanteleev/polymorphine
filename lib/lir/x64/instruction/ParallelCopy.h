#pragma once
#include "LIRProducerInstruction.h"
#include "mir/instruction/Instruction.h"


class ParallelCopy final: public LIRProducerInstructionBase {
public:
    explicit ParallelCopy(std::vector<LIROperand> &&uses, std::vector<LIRBlock*>&& blocks) noexcept:
        LIRProducerInstructionBase(std::move(uses)),
        m_blocks(std::move(blocks)) {}

    void visit(LIRVisitor &visitor) override;

    [[nodiscard]]
    std::span<LIRBlock*> targets() noexcept {
        return m_blocks;
    }

    [[nodiscard]]
    std::span<LIRBlock* const> targets() const noexcept {
        return m_blocks;
    }

    static std::unique_ptr<ParallelCopy> copy(std::vector<LIROperand> &&uses, std::vector<LIRBlock*>&& blocks) {
        const auto size = uses.front().size();
        auto copy = std::make_unique<ParallelCopy>(std::move(uses), std::move(blocks));
        copy->add_def(LIRVal::reg(size, 0, copy.get()));
        return copy;
    }

private:
    std::vector<LIRBlock*> m_blocks;
};