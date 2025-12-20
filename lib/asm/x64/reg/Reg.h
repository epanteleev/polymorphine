#pragma once

#include <expected>
#include <variant>
#include <utility>

#include "GPReg.h"
#include "XmmReg.h"
#include "utility/Error.h"


namespace aasm {
    template<typename R>
    concept RegVariant = std::same_as<R, GPReg> || std::same_as<R, XmmReg>;

    class Reg final {
    public:
        explicit Reg() = default;

        Reg(GPReg r) noexcept: m_reg(r) {}
        Reg(XmmReg r) noexcept: m_reg(r) {}

        [[nodiscard]]
        std::expected<GPReg, Error> as_gp_reg() const noexcept {
            if (const auto reg = std::get_if<GPReg>(&m_reg)) {
                return *reg;
            }

            return std::unexpected(Error::CastError);
        }

        [[nodiscard]]
        std::expected<XmmReg, Error> as_xmm_reg() const noexcept {
            if (const auto reg = std::get_if<XmmReg>(&m_reg)) {
                return *reg;
            }

            return std::unexpected(Error::CastError);
        }

        template<typename Fn>
        decltype(auto) visit(Fn&& fn) const {
            return std::visit(std::forward<Fn>(fn), m_reg);
        }

        bool operator==(const Reg& other) const noexcept {
            return m_reg == other.m_reg;
        }

        [[nodiscard]]
        std::uint8_t code() const noexcept {
            const auto vis = [&]<typename T>(const T& r) {
                return r.code();
            };

            return std::visit(vis, m_reg);
        }

        friend std::ostream& operator<<(std::ostream& os, const Reg& reg);

    private:
        std::variant<GPReg, XmmReg> m_reg;
    };
}