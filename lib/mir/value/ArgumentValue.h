#pragma once

#include <cstddef>
#include <vector>
#include <iosfwd>

#include "mir/value/Use.h"
#include "base/Attribute.h"
#include "mir/mir_frwd.h"
#include "mir/instruction/Unary.h"

class ArgumentValue final: public Use {
public:
    explicit ArgumentValue(const std::size_t index, const NonTrivialType* type, const AttributeSet attributes) noexcept:
        Use(type),
        m_index(index),
        m_attributes(attributes) {}

    friend std::ostream& operator<<(std::ostream &os, const ArgumentValue &args);

    [[nodiscard]]
    AttributeSet attributes() const noexcept {
        return m_attributes;
    }

    [[nodiscard]]
    std::size_t index() const noexcept {
        return m_index;
    }

private:
    const std::size_t m_index;
    const AttributeSet m_attributes;
};
