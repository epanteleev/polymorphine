#pragma once

#include <cstddef>
#include <vector>
#include <iosfwd>
#include <span>

#include "base/Attribute.h"
#include "mir/mir_frwd.h"

class ArgumentValue final {
public:
    explicit ArgumentValue(const std::size_t index, const NonTrivialType* type, AttributeSet attributes) noexcept:
        m_index(index),
        m_type(type),
        m_attributes(std::move(attributes)) {}

    friend std::ostream& operator<<(std::ostream &os, const ArgumentValue &args);

    [[nodiscard]]
    const NonTrivialType* type() const noexcept {
        return m_type;
    }

    [[nodiscard]]
    AttributeSet attributes() const noexcept {
        return m_attributes;
    }

    void add_user(const Instruction* user);

    [[nodiscard]]
    std::span<const Instruction* const> users() const noexcept {
        return m_users;
    }

    [[nodiscard]]
    std::size_t index() const noexcept {
        return m_index;
    }

private:
    const std::size_t m_index;
    const NonTrivialType* m_type;
    std::vector<const Instruction*> m_users;
    const AttributeSet m_attributes;
};