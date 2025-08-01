#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace aasm {
    enum class Linkage: std::uint8_t {
        EXTERNAL,
        INTERNAL,
    };

    class Symbol final {
    public:
        Symbol(const std::string_view name, const Linkage linkage) noexcept:
            m_name(name),
            m_linkage(linkage) {}

    private:
        std::string m_name;
        Linkage m_linkage;
    };
}