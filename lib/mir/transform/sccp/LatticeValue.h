#pragma once

#include <cstdint>

#include "mir/value/VConstant.h"

namespace details {
    enum class LatticeKind : std::uint8_t {
        Unknown,
        Constant,
        Overdefined
    };

    class LatticeValue final {
        explicit constexpr LatticeValue(const LatticeKind& _kind, const VConstant& _value) noexcept:
            m_kind(_kind),
            m_value(_value) {}

    public:
        consteval LatticeValue() noexcept:
            m_kind(LatticeKind::Unknown),
            m_value(VConstant::undefined()) {}

        [[nodiscard]]
        static consteval LatticeValue unknown() noexcept {
            return LatticeValue(LatticeKind::Unknown, VConstant::undefined());
        }

        [[nodiscard]]
        static consteval LatticeValue overdefined() noexcept {
            return LatticeValue(LatticeKind::Overdefined, VConstant::undefined());
        }

        [[nodiscard]]
        static LatticeValue constant(const VConstant& val) noexcept {
            return LatticeValue(LatticeKind::Constant, val);
        }

        [[nodiscard]]
        constexpr LatticeKind kind() const noexcept {
            return m_kind;
        }

        [[nodiscard]]
        constexpr const VConstant& cst() const noexcept {
            return m_value;
        }

    private:
        LatticeKind m_kind{};
        VConstant m_value;
    };
}
