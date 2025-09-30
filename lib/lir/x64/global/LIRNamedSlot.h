#pragma once

#include "LIRSlot.h"


class LIRNamedSlot final {
public:
    explicit LIRNamedSlot(std::string&& name, LIRSlot&& value) noexcept:
        m_name(std::move(name)),
        m_value(std::move(value)) {}

    [[nodiscard]]
    const LIRSlot& root() const noexcept {
        return m_value;
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_value.size();
    }

    [[nodiscard]]
    std::string_view name() const noexcept { return m_name; }

    friend std::ostream &operator<<(std::ostream &os, const LIRNamedSlot &slot);

    void print_description(std::ostream &os) const;

private:
    std::string m_name;
    LIRSlot m_value;
};