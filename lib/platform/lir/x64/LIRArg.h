#pragma once

#include <cstdint>
#include <iosfwd>

class LIRArg final {
public:
    explicit LIRArg(std::uint32_t index, std::uint8_t size) :
        m_index(index), m_size(size) {}

    [[nodiscard]]
    std::uint8_t size() const noexcept { return m_size; }

    [[nodiscard]]
    std::uint32_t index() const noexcept { return m_index; }

    friend std::ostream& operator<<(std::ostream& os, const LIRArg& op) noexcept;

private:
    const std::uint32_t m_index;
    const std::uint8_t m_size;
};

std::ostream& operator<<(std::ostream& os, const LIRArg& op) noexcept;