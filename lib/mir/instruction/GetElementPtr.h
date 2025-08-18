#pragma once

#include "mir/instruction/ValueInstruction.h"

class GetElementPtr final: public ValueInstruction {
public:
    GetElementPtr(const NonTrivialType* basic_type, const Value &pointer, const Value &index):
        ValueInstruction(PointerType::ptr(), {pointer, index}),
        m_basic_type(basic_type) {}

    [[nodiscard]]
    const Value &pointer() const {
        return m_values.at(0);
    }

    [[nodiscard]]
    const Value &index() const {
        return m_values.at(1);
    }

    [[nodiscard]]
    const NonTrivialType* access_type() const {
        return m_basic_type;
    }

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<GetElementPtr> gep(const NonTrivialType* basic_type, const Value &pointer, const Value &index) {
        return std::make_unique<GetElementPtr>(basic_type, pointer, index);
    }

private:
    const NonTrivialType* m_basic_type;
};
