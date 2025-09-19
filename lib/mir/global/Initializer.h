#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <iosfwd>
#include "mir/mir_frwd.h"

template<typename T>
concept InitializerVarians = std::convertible_to<T, std::int64_t> ||
    std::convertible_to<T, double> ||
    std::convertible_to<T, const GlobalSymbol*> ||
    std::convertible_to<T, std::string> ||
    std::convertible_to<T, std::vector<Initializer>>;

class Initializer final {
public:
    template<InitializerVarians T>
    Initializer(T&& value) noexcept:
        m_value(std::forward<T>(value)) {}

    Initializer(const std::initializer_list<Initializer> list) noexcept:
        m_value(std::vector(list)) {}

    template<typename Fn>
    decltype(auto) visit(Fn&& vis) const {
        return std::visit(std::forward<Fn>(vis), m_value);
    }

    friend std::ostream& operator<<(std::ostream& os, const Initializer& sym);

private:
    std::variant<std::int64_t, double, const GlobalConstant*, std::string, std::vector<Initializer>> m_value;
};