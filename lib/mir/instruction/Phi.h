#pragma once

#include "ValueInstruction.h"

class Phi final: public ValueInstruction {
public:
    explicit Phi(const PrimitiveType *ty, std::vector<Value>&& values, std::vector<BasicBlock *>&& targets) noexcept:
        ValueInstruction(ty, std::move(values)), m_entries(std::move(targets)) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    std::span<BasicBlock* const> incoming() const noexcept {
        return m_entries;
    }

    static std::unique_ptr<Phi> phi(const PrimitiveType* type, std::vector<Value>&& values, std::vector<BasicBlock*>&& targets) {
        return std::make_unique<Phi>(type, std::move(values), std::move(targets));
    }

private:
    std::vector<BasicBlock*> m_entries;
};