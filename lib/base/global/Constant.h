#pragma once

#include "SlotType.h"

class Constant final {
public:
    explicit Constant(const SlotType type, const std::int64_t value) noexcept:
        m_kind(type),
        m_value(value) {}

    [[nodiscard]]
    SlotType type() const noexcept {
        return m_kind;
    }

    [[nodiscard]]
    std::int64_t value() const noexcept {
        return m_value;
    }

private:
    SlotType m_kind;
    std::int64_t m_value;
};
