#pragma once
#include "Instruction.h"


class Store final: public Instruction {
public:
    Store(const std::size_t id, BasicBlock *bb, const Value& ptr, const Value& value) :
        Instruction(id, bb, {ptr, value}) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    const Value& pointer() const {
        return m_values.at(0);
    }

    [[nodiscard]]
    const Value& value() const {
        return m_values.at(1);
    }

    static InstructionBuilder<Store> store(const Value& ptr, const Value& value) {
        return [=](std::size_t id, BasicBlock *bb) {
            return std::make_unique<Store>(id, bb, ptr, value);
        };
    }
};