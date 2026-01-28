#pragma once

#include "mir/global/Initializer.h"
#include "mir/global/GlobalSymbol.h"
#include "mir/value/Value.h"

enum class GValueKind : uint8_t {
    CONSTANT,
    VARIABLE,
};

inline std::string_view to_string(const GValueKind kind) {
    switch (kind) {
        case GValueKind::CONSTANT: return "constant";
        case GValueKind::VARIABLE: return "variable";
        default: std::unreachable();
    }
}

class GlobalValue final: public GlobalSymbol {
public:
    explicit GlobalValue(std::string&& name, const NonTrivialType* type, Initializer&& value, const GValueKind kind) noexcept:
        GlobalSymbol(std::move(name), type),
        m_kind(kind),
        m_value(std::move(value)) {}

    void print_description(std::ostream& os) const;

    [[nodiscard]]
    const Initializer& initializer() const noexcept {
        return m_value;
    }

    [[nodiscard]]
    GValueKind kind() const noexcept {
        return m_kind;
    }

private:
    GValueKind m_kind;
    Initializer m_value;
};


namespace impls {
    inline bool g_variable(const Value& value) noexcept {
        if (!value.is<GlobalValue*>()) {
            return false;
        }

        const auto gvalue = value.get<GlobalValue*>();
        return gvalue->kind() == GValueKind::VARIABLE;
    }

    inline bool g_value(const Value& value) noexcept {
        return value.is<GlobalValue*>();
    }
}

consteval auto g_variable() noexcept {
    return impls::g_variable;
}

consteval auto g_value() noexcept {
    return impls::g_value;
}