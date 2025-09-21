#pragma once

#include <cstdint>
#include <string>
#include <iosfwd>
#include <utility>

#include "Initializer.h"
#include "mir/mir_frwd.h"

class GlobalSymbol {
public:
    explicit GlobalSymbol(std::string&& name, const NonTrivialType* type, Initializer&& value) noexcept:
        m_name(std::move(name)),
        m_content_type(type),
        m_value(std::move(value)) {}

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_name;
    }

    [[nodiscard]]
    const NonTrivialType* content_type() const noexcept {
        return m_content_type;
    }

    [[nodiscard]]
    const Initializer& initializer() const noexcept {
        return m_value;
    }

    friend std::ostream& operator<<(std::ostream& os, const GlobalSymbol& sym);

protected:
    std::string m_name;
    const NonTrivialType* m_content_type;
    Initializer m_value;
};