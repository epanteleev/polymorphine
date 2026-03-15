#pragma once

#include "Instruction.h"

class Store final: public Instruction {
public:
    explicit Store(const Value& ptr, const Value& value) noexcept:
        Instruction({ptr, value}) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    const Value& pointer() const {
        return m_values[0];
    }

    [[nodiscard]]
    const Value& value() const {
        return m_values[1];
    }

    [[nodiscard]]
    static std::unique_ptr<Store> store(const Value& ptr, const Value& value) {
        return std::make_unique<Store>(ptr, value);
    }

    [[nodiscard]]
    static const Store* cast(const Instruction* inst) noexcept {
        return dynamic_cast<const Store *>(inst);
    }

    [[nodiscard]]
    static Store* cast(Instruction* inst) noexcept {
        return dynamic_cast<Store *>(inst);
    }
};

namespace impl {
    inline bool store(const Instruction* inst) noexcept {
        return dynamic_cast<const Store*>(inst) != nullptr;
    }
}

consteval auto store() {
    return impl::store;
}