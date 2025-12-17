#pragma once

#include "LIRProducerInstruction.h"

class ParallelCopy final: public LIRProducerInstructionBase {
public:
    explicit ParallelCopy(const LIRValType ty, std::vector<LIROperand> &&uses, std::vector<LIRBlock*>&& blocks) noexcept:
        LIRProducerInstructionBase({ty}, std::move(uses)),
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

    static std::unique_ptr<ParallelCopy> copy(const LIRValType ty, std::vector<LIROperand> &&uses, std::vector<LIRBlock*>&& blocks) {
        const auto size = uses.front().size();
        auto copy = std::make_unique<ParallelCopy>(ty, std::move(uses), std::move(blocks));
        copy->add_def(LIRVal::reg(size, size, 0, copy.get()));
        return copy;
    }

private:
    std::vector<LIRBlock*> m_blocks;
};