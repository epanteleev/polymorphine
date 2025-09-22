#pragma once

#include <expected>
#include <unordered_map>

#include "GlobalValue.h"
#include "utility/Error.h"


class GValuePool final {
public:
    using iterator = std::unordered_map<std::string, GlobalValue>::iterator;
    using const_iterator = std::unordered_map<std::string, GlobalValue>::const_iterator;

    explicit GValuePool() = default;

    [[nodiscard]]
    std::expected<const GlobalValue*, Error> lookup(const std::string& name) const noexcept {
        if (const auto it = m_gvalues.find(name); it != m_gvalues.end()) {
            return &it->second;
        }

        return std::unexpected(Error::NotFoundError);
    }

    [[nodiscard]]
    std::expected<const GlobalValue*, Error> add_constant(const std::string_view name, const NonTrivialType* type, Initializer&& value) {
        return add_constant_internal(name, type, std::move(value), GValueKind::CONSTANT);
    }

    [[nodiscard]]
    std::expected<const GlobalValue*, Error> add_variable(const std::string_view name, const NonTrivialType* type, Initializer&& value) {
        return add_constant_internal(name, type, std::move(value), GValueKind::VARIABLE);
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return m_gvalues.empty();
    }

    iterator begin() noexcept {
        return m_gvalues.begin();
    }

    iterator end() noexcept {
        return m_gvalues.end();
    }

    const_iterator begin() const noexcept {
        return m_gvalues.begin();
    }

    const_iterator end() const noexcept {
        return m_gvalues.end();
    }

private:
    [[nodiscard]]
    std::expected<const GlobalValue*, Error> add_constant_internal(const std::string_view name, const NonTrivialType* type, Initializer&& value, GValueKind kind) {
        std::string key(name);
        const auto [it, inserted] = m_gvalues.emplace(std::move(key), GlobalValue(std::string(name), type, std::move(value), kind));
        if (!inserted) {
            return std::unexpected(Error::AlreadyExists);
        }

        return &it->second;
    }

    std::unordered_map<std::string, GlobalValue> m_gvalues;
};