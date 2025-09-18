#pragma once

#include <expected>
#include <string>
#include <unordered_map>

#include "NamedSlot.h"
#include "utility/Error.h"


class GlobalData final {
public:
    using iterator = std::unordered_map<std::string, NamedSlot>::iterator;
    using const_iterator = std::unordered_map<std::string, NamedSlot>::const_iterator;

    explicit GlobalData() = default;

    [[nodiscard]]
    std::expected<const NamedSlot*, Error> lookup(const std::string &name) const noexcept {
        const auto it = m_slots.find(name);
        if (it == m_slots.end()) {
            return std::unexpected(Error::NotFoundError);
        }

        return &it->second;
    }

    [[nodiscard]]
    std::expected<NamedSlot*, Error> add_slot(std::string_view name, NamedSlot&& value) {
        const auto [it, inserted] = m_slots.emplace(name, std::move(value));
        if (!inserted) {
            return std::unexpected(Error::AlreadyExists);
        }

        return &it->second;
    }

    bool empty() const noexcept {
        return m_slots.empty();
    }

    iterator begin() noexcept {
        return m_slots.begin();
    }

    iterator end() noexcept {
        return m_slots.end();
    }

    const_iterator begin() const noexcept {
        return m_slots.begin();
    }

    const_iterator end() const noexcept {
        return m_slots.end();
    }

private:
    std::unordered_map<std::string, NamedSlot> m_slots;
};