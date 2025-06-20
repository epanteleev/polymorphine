#pragma once

#include <cstddef>
#include <vector>
#include <iosfwd>

#include "ir_frwd.h"

class ArgumentValue final {
public:
    explicit ArgumentValue(const std::size_t index, const NonTrivialType* type) noexcept
        : m_index(index), m_type(type) {}

    void print(std::ostream& os) const;

    [[nodiscard]]
    const NonTrivialType* type() const noexcept {
        return m_type;
    }

    void add_user(Instruction* user);

private:
    std::size_t m_index;
    const NonTrivialType* m_type;
    std::vector<Instruction*> m_used_in;
};