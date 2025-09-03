#pragma once

#include <cstdint>
#include <initializer_list>
#include <iosfwd>


enum class Attribute: std::uint8_t {
    ByValue = 0,
};

std::ostream& operator<<(std::ostream& os, const Attribute& attr);

class AttributeSet final {
    static constexpr std::size_t BITS = 8;

public:
    AttributeSet(const std::initializer_list<Attribute> list) noexcept {
        for (const auto &attr: list) {
            m_bits |= 1 << static_cast<std::size_t>(attr);
        }
    }

    template<Attribute ATTR>
    void set() noexcept {
        m_bits |= 1 << static_cast<std::size_t>(ATTR);
    }

    [[nodiscard]]
    bool has(Attribute attr) const noexcept {
        return test(static_cast<std::size_t>(attr));
    }

    friend std::ostream& operator<<(std::ostream& os, const AttributeSet& attr);

private:
    [[nodiscard]]
    bool test(const std::size_t idx) const noexcept {
        return m_bits & 1 << idx;
    }

    unsigned char m_bits{};
};

std::ostream& operator<<(std::ostream& os, const AttributeSet& attr);