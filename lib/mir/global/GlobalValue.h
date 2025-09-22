#pragma once

#include "mir/global/GlobalSymbol.h"

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