#pragma once

#include "ValueInstruction.h"
#include "mir/types/PointerType.h"


class FieldAccess: public ValueInstruction {
public:
    explicit FieldAccess(const NonTrivialType* basic_type, const Value &pointer, const Value &index) noexcept:
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
    const NonTrivialType* access_type() const noexcept {
        return m_basic_type;
    }

private:
    const NonTrivialType* m_basic_type;
};
