#pragma once

#include <cstdint>
#include <ostream>
#include <utility>

#include "asm/Common.h"
#include "asm/Register.h"

#include "AddressBaseDisp.h"
#include "AddressIndexScale.h"

namespace aasm {
    class Address final {
    public:
        explicit constexpr Address(const GPReg base, const GPReg index, const std::uint8_t scale = 1, const std::int32_t displacement = 0) noexcept:
            m_address(AddressIndexScale(base, index, scale, displacement)) {}

        explicit constexpr Address(const GPReg base, const std::int32_t displacement = 0) noexcept:
                    m_address(AddressBaseDisp(base, displacement)) {}

        friend std::ostream& operator<<(std::ostream & os, const Address & addr);

        bool operator==(const Address & other) const noexcept {
            if (this == &other) {
                return true;
            }

            return m_address == other.m_address;
        }

        template<CodeBuffer C>
        void encode(C& c, unsigned int modrm_pattern) const {
            const auto visit = [&](const auto& addr) {
                addr.encode(c, modrm_pattern);
            };
            std::visit(visit, m_address);
        }

        template<typename Addr>
        requires std::same_as<Addr, AddressBaseDisp> || std::same_as<Addr, AddressIndexScale>
        [[nodiscard]]
        const Addr* as() const noexcept {
            return std::get_if<Addr>(&m_address);
        }

        [[nodiscard]]
        GPReg base() const noexcept {
            const auto visit = [&](const auto& addr) {
                return addr.m_base;
            };
            return std::visit(visit, m_address);
        }

        std::variant<AddressBaseDisp, AddressIndexScale> m_address;
    };

    inline std::ostream& operator<<(std::ostream & os, const Address & addr) {
        const auto visit = [&](const auto& addr) {
            os << addr;
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