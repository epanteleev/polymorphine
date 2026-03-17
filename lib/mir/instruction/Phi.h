#pragma once

#include "ValueInstruction.h"
#include "mir/value/UsedValue.h"
#include "utility/StdExtensions.h"

class Phi final: public ValueInstruction {
public:
    explicit Phi(const PrimitiveType *ty, std::vector<Value>&& values, std::vector<const BasicBlock *>&& targets) noexcept:
        ValueInstruction(ty, std::move(values)), m_entries(std::move(targets)) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    std::span<const BasicBlock* const> incoming() const noexcept {
        return m_entries;
    }

    void remove_incoming_from(const BasicBlock* block) {
        for (std::size_t i = 0; i < m_entries.size();) {
            if (m_entries[i] != block) {
                ++i;
                continue;
            }

            if (auto local = UsedValue::try_from(m_values[i]); local.has_value()) {
                local->remove_user(this);
            }
            remove_fast(m_entries, m_entries.begin() + i);
            remove_fast(m_values, m_values.begin() + i);
        }
    }

    [[nodiscard]]
    static std::unique_ptr<Phi> phi(const PrimitiveType* type, std::vector<Value>&& values, std::vector<const BasicBlock*>&& targets) {
        return std::make_unique<Phi>(type, std::move(values), std::move(targets));
    }

    [[nodiscard]]
    static std::unique_ptr<Phi> undef(const PrimitiveType* type, std::vector<const BasicBlock*>&& targets) {
        std::vector<Value> values(targets.size(), Value::undefined());
        return std::make_unique<Phi>(type, std::move(values), std::move(targets));
    }

    [[nodiscard]]
    static const Phi* cast(const Instruction* inst) noexcept {
        return dynamic_cast<const Phi *>(inst);
    }

    [[nodiscard]]
    static Phi* cast(Instruction* inst) noexcept {
        return dynamic_cast<Phi *>(inst);
    }

private:
    std::vector<const BasicBlock*> m_entries;
};

namespace impls {
    inline bool phi(const Instruction* inst) noexcept {
        return Phi::cast(inst) != nullptr;
    }
}

consteval auto phi() {
    return impls::phi;
}