#pragma once

#include <cstdint>
#include <ostream>

#include "asm/symbol/Symbol.h"
#include "asm/Common.h"
#include "asm/Relocation.h"
#include "asm/reg/Register.h"

#include "AddressBaseDisp.h"
#include "AddressIndexScale.h"
#include "AddressLiteral.h"

namespace aasm {
    class Address final {
    public:
        explicit constexpr Address(const GPReg base, const GPReg index, const std::uint8_t scale = 1, const std::int32_t displacement = 0) noexcept:
            m_address(AddressIndexScale(base, index, scale, displacement)) {}

        explicit constexpr Address(const GPReg base, const std::int32_t displacement = 0) noexcept:
                    m_address(AddressBaseDisp(base, displacement)) {}

        explicit constexpr Address(const Symbol* symbol, const std::int32_t displacement = 0) noexcept:
            m_address(AddressLiteral(symbol, displacement)) {}

        friend std::ostream& operator<<(std::ostream & os, const Address & addr);

        bool operator==(const Address & other) const noexcept {
            if (this == &other) {
                return true;
            }

            return m_address == other.m_address;
        }

        template<CodeBuffer C>
        std::optional<Relocation> encode(C& c, std::uint32_t modrm_pattern) const {
            const auto visit = [&](const auto& addr) {
                return addr.encode(c, modrm_pattern);
            };

            return std::visit<std::optional<Relocation>>(visit, m_address);
        }

        template<typename Addr>
        requires std::same_as<Addr, AddressBaseDisp> || std::same_as<Addr, AddressIndexScale> || std::same_as<Addr, AddressLiteral>
        [[nodiscard]]
        const Addr* as() const noexcept {
            return std::get_if<Addr>(&m_address);
        }

        [[nodiscard]]
        std::optional<GPReg> base() const noexcept {
            if (const auto as_base_disp = as<AddressBaseDisp>()) {
                return as_base_disp->m_base;
            }
            if (const auto as_index_scale = as<AddressIndexScale>()) {
                return as_index_scale->m_base;
            }
            return std::nullopt;
        }

        [[nodiscard]]
        std::size_t hash() const noexcept {
            const auto hash_fn = []<typename T0>(const T0& val) {
                return val.hash();
            };

            return std::visit(hash_fn, m_address);
        }

        std::variant<AddressBaseDisp, AddressIndexScale, AddressLiteral> m_address;
    };

    inline std::ostream& operator<<(std::ostream & os, const Address & addr) {
        const auto visit = [&](const auto& address) {
            os << address;
        };
        std::visit(visit, addr.m_address);
        return os;
    }

    static constexpr std::uint8_t X(const Address& arg) {
        const auto as_index_addr = arg.as<AddressIndexScale>();
        if (as_index_addr == nullptr) {
            return 0;
        }

        return as_index_addr->index().is_64_bit_reg() << 1;
    }
}
