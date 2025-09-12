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

protected:
    std::string m_name;
    const NonTrivialType* m_type;
};

template<typename T>
concept GlobalConstantVarians = std::is_same_v<T, std::int64_t> ||
    std::is_same_v<T, double> ||
    std::is_same_v<T, std::string>;

class GlobalConstant final: public GlobalSymbol {
public:
    template<GlobalConstantVarians T>
    explicit GlobalConstant(std::string&& name, const NonTrivialType* type, T value) noexcept:
        GlobalSymbol(std::move(name), type),
        m_value(value) {}

    friend std::ostream& operator<<(std::ostream& os, const GlobalSymbol& sym);

    void print_description(std::ostream& os) const;

private:
    std::variant<std::int64_t, double, std::string> m_value;
};