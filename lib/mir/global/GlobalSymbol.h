#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <iosfwd>

#include "mir/types/NonTrivialType.h"

class GlobalSymbol {
public:
    explicit GlobalSymbol(std::string&& name, const NonTrivialType* type) noexcept:
        m_name(std::move(name)),
        m_type(type) {}

    [[nodiscard]]
    std::string_view name() const noexcept { return m_name; }

    [[nodiscard]]
    const NonTrivialType* content_type() const noexcept { return m_type; }

protected:
    std::string m_name;
    const NonTrivialType* m_type;
};

template<typename T>
concept GlobalConstantVarians = std::convertible_to<T, std::int64_t> ||
    std::convertible_to<T, double> ||
    std::convertible_to<T, std::string_view>;

class GlobalConstant final: public GlobalSymbol {
public:
    template<typename T>
    explicit GlobalConstant(std::string&& name, const NonTrivialType* type, T&& value) noexcept:
        GlobalSymbol(std::move(name), type),
        m_value(std::forward<T>(value)) {}

    template<typename Fn>
    decltype(auto) visit(Fn&& vis) const {
        return std::visit(std::forward<Fn>(vis), m_value);
    }

    friend std::ostream& operator<<(std::ostream& os, const GlobalConstant& sym);

    void print_description(std::ostream& os) const;

private:
    std::variant<std::int64_t, double, std::string> m_value;
};