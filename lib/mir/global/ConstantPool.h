#pragma once

#include <expected>
#include <unordered_map>

#include "GlobalSymbol.h"
#include "utility/Error.h"


class ConstantPool final {
public:
    using iterator = std::unordered_map<std::string, GlobalConstant>::iterator;
    using const_iterator = std::unordered_map<std::string, GlobalConstant>::const_iterator;

    explicit ConstantPool() = default;

    [[nodiscard]]
    std::expected<const GlobalConstant*, Error> lookup(const std::string& name) const noexcept {
        if (const auto it = m_constants.find(name); it != m_constants.end()) {
            return &it->second;
        }

        return std::unexpected(Error::NotFoundError);
    }

    template <typename T>
    [[nodiscard]]
    std::expected<const GlobalConstant*, Error> add_constant(std::string&& name, const NonTrivialType* type, T value) {
        std::string key(name);
        const auto [it, inserted] = m_constants.emplace(std::move(key), GlobalConstant(std::move(name), type, value));
        if (!inserted) {
            return std::unexpected(Error::AlreadyExists);
        }

        return &it->second;
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return m_constants.empty();
    }

    iterator begin() noexcept {
        return m_constants.begin();
    }

    iterator end() noexcept {
        return m_constants.end();
    }

    const_iterator begin() const noexcept {
        return m_constants.begin();
    }

    const_iterator end() const noexcept {
        return m_constants.end();
    }

private:
    std::unordered_map<std::string, GlobalConstant> m_constants;
};