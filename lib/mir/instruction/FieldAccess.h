#pragma once

#include "ValueInstruction.h"
#include "mir/types/PointerType.h"


class FieldAccess: public ValueInstruction {
public:
    explicit FieldAccess(std::vector<Value>&& values) noexcept:
        ValueInstruction(PointerType::ptr(), std::move(values)) {}

    [[nodiscard]]
    const Value &pointer() const {
        return m_values.at(0);
    }
};
