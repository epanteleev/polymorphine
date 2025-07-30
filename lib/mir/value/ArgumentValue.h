#pragma once

#include <cstddef>
#include <vector>
#include <iosfwd>

#include "mir/mir_frwd.h"

class ArgumentValue final {
public:
    explicit ArgumentValue(const std::size_t index, const NonTrivialType* type) noexcept
        : m_index(index), m_type(type) {}

    friend std::ostream& operator<<(std::ostream &os, const ArgumentValue &args);

    [[nodiscard]]
    const NonTrivialType* type() const noexcept {
        return m_type;
    }

    void add_user(const Instruction* user);

    [[nodiscard]]
    std::size_t index() const noexcept {
        return m_index;
    }

private:
    const std::size_t m_index;
    const NonTrivialType* m_type;
    std::vector<const Instruction*> m_used_in;
};