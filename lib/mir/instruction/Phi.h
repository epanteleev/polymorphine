#pragma once

#include "ValueInstruction.h"

class Phi final: public ValueInstruction {
public:
    explicit Phi(const PrimitiveType *ty, std::vector<Value>&& values, std::vector<const BasicBlock *>&& targets) noexcept:
        ValueInstruction(ty, std::move(values)), m_entries(std::move(targets)) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    std::span<const BasicBlock* const> incoming() const noexcept {
        return m_entries;
    }

    void set_incoming_value(std::size_t index, const Value& value) noexcept {
        assertion(index < m_values.size(), "index out of range");
        m_values[index] = value;
    }

    [[nodiscard]]
    static std::unique_ptr<Phi> phi(const PrimitiveType* type, std::vector<Value>&& values, std::vector<const BasicBlock*>&& targets) {
        return std::make_unique<Phi>(type, std::move(values), std::move(targets));
    }

    [[nodiscard]]
    static std::unique_ptr<Phi> undef(const PrimitiveType* type, std::vector<const BasicBlock*>&& targets) {
        std::vector values{targets.size(), Value::undefined()};
        return std::make_unique<Phi>(type, std::move(values), std::move(targets));
    }

private:
    std::vector<const BasicBlock*> m_entries;
};