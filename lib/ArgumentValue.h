#pragma once

#include <cstddef>
#include <vector>
#include <iosfwd>

#include "ir_frwd.h"

class ArgumentValue final {
public:
    explicit ArgumentValue(std::size_t index, Type* type) noexcept
        : m_index(index), m_type(type) {}

    void print(std::ostream& os) const;

    [[nodiscard]]
    Type* type() const noexcept {
        return m_type;
    }

private:
    std::size_t m_index;
    Type* m_type;
    std::vector<Instruction*> m_used_in;
};
