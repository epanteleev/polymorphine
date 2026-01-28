#pragma once

#include "ValueInstruction.h"
#include "mir/value/ValueMatcher.h"

class FieldAccess: public ValueInstruction {
public:
    explicit FieldAccess(std::vector<Value>&& values) noexcept:
        ValueInstruction(PointerType::ptr(), std::move(values)) {}

    [[nodiscard]]
    const Value &pointer() const {
        return m_values[0];
    }

    [[nodiscard]]
    virtual const NonTrivialType* access_type() const noexcept = 0;
};

consteval auto field_access() noexcept {
    return impls::value_inst<FieldAccess>;
}