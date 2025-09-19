#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace aasm {
    enum class BindAttribute: std::uint8_t {
        DEFAULT,
        EXTERNAL,
        INTERNAL,
    };

    class Symbol final {
    public:
        Symbol(std::string&& name, const BindAttribute bind) noexcept:
            m_name(std::move(name)),
            m_bind(bind) {}

        [[nodiscard]]
        std::string_view name() const noexcept { return m_name; }

        [[nodiscard]]
        BindAttribute bind() const noexcept { return m_bind; }

        friend std::ostream &operator<<(std::ostream &os, const Symbol &symbol);

    private:
        std::string m_name;
        BindAttribute m_bind;
    };

    inline std::ostream & operator<<(std::ostream &os, const Symbol &symbol) {
        return os << symbol.m_name;
    }
}