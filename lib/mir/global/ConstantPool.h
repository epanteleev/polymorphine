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

    template<std::convertible_to<std::int64_t> T>
    [[nodiscard]]
    std::expected<const GlobalConstant*, Error> add_constant(const std::string_view name, const IntegerType* type, T value) {
        return add_constant_internal(name, type, std::forward<T>(value));
    }

    [[nodiscard]]
    std::expected<const GlobalConstant*, Error> add_constant(std::string_view name, const ArrayType* type, std::string_view value);

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
    template <typename T>
    [[nodiscard]]
    std::expected<const GlobalConstant*, Error> add_constant_internal(const std::string_view name, const NonTrivialType* type, T&& value) {
        std::string key(name);
        const auto [it, inserted] = m_constants.emplace(std::move(key), GlobalConstant(std::string(name), type, std::forward<T>(value)));
        if (!inserted) {
            return std::unexpected(Error::AlreadyExists);
        }

        return &it->second;
    }

    std::unordered_map<std::string, GlobalConstant> m_constants;
};