#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace aasm {
    enum class Linkage: std::uint8_t {
        DEFAULT,
        EXTERNAL,
        INTERNAL,
    };

    class Symbol final {
    public:
        Symbol(std::string&& name, const Linkage linkage) noexcept:
            m_name(std::move(name)),
            m_linkage(linkage) {}

        [[nodiscard]]
        std::string_view name() const noexcept { return m_name; }

        [[nodiscard]]
        Linkage linkage() const noexcept { return m_linkage; }

        friend std::ostream &operator<<(std::ostream &os, const Symbol &symbol);

    private:
        std::string m_name;
        Linkage m_linkage;
    };

    inline std::ostream & operator<<(std::ostream &os, const Symbol &symbol) {
        return os << symbol.m_name;
    }
}
