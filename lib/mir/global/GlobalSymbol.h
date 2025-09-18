#pragma once

#include <cstdint>
#include <string>
#include <iosfwd>
#include <utility>

#include "Initializer.h"
#include "mir/types/NonTrivialType.h"
#include "utility/Error.h"

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


class GlobalConstant final: public GlobalSymbol {
public:
    explicit GlobalConstant(std::string&& name, const NonTrivialType* type, Initializer&& value) noexcept:
        GlobalSymbol(std::move(name), type),
        m_value(std::move(value)) {}

    template<typename Fn>
    decltype(auto) visit(Fn&& vis) const {
        return m_value.visit(std::forward<Fn>(vis));
    }

    friend std::ostream& operator<<(std::ostream& os, const GlobalConstant& sym);

    void print_description(std::ostream& os) const;

    [[nodiscard]]
    const Initializer& initializer() const noexcept { return m_value; }

private:
    Initializer m_value;
};