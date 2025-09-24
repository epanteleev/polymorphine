#pragma once

#include <cstddef>

class ZeroInit final {
public:
    explicit ZeroInit(const std::size_t length) noexcept:
        m_length(length) {}

    [[nodiscard]]
    std::size_t length() const noexcept {
        return m_length;
    }

private:
    std::size_t m_length{};
};