#pragma once

#include "Instruction.h"

class Store final: public Instruction {
public:
    Store(const Value& ptr, const Value& value) :
        Instruction({ptr, value}) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    const Value& pointer() const {
        return m_values.at(0);
    }

    [[nodiscard]]
    const Value& value() const {
        return m_values.at(1);
    }

    static std::unique_ptr<Store> store(const Value& ptr, const Value& value) {
        return std::make_unique<Store>(ptr, value);
    }
};