#pragma once

#include <memory>

#include "Slot.h"

class NamedSlot final {
public:
    explicit NamedSlot(std::string&& name, std::shared_ptr<Slot>&& value) noexcept:
        m_name(std::move(name)),
        m_value(std::move(value)) {}

    [[nodiscard]]
    SlotType type() const noexcept {
        return m_value->type();
    }

    [[nodiscard]]
    const Slot& root() const noexcept {
        return *m_value;
    }

    [[nodiscard]]
    std::uint8_t size() const noexcept {
        return m_value->size();
    }

    [[nodiscard]]
    std::string_view name() const noexcept { return m_name; }

    friend std::ostream &operator<<(std::ostream &os, const NamedSlot &slot);

    void print_description(std::ostream &os) const;

private:
    std::string m_name;
    std::shared_ptr<Slot> m_value;
};