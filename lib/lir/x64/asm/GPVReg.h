#pragma once
#include <variant>
#include <expected>

#include "asm/x64/asm.h"

template<typename T>
concept GPVRegVariant = std::is_same_v<T, aasm::GPReg> ||
    std::is_same_v<T, aasm::Address>;

class GPVReg final {
public:
    template<GPVRegVariant T>
    GPVReg(const T& reg) noexcept:
        m_reg(reg) {}

    template<typename T>
    void visit(const T& visitor) const {
        std::visit(visitor, m_reg);
    }

    [[nodiscard]]
    std::expected<aasm::GPReg, Error> as_gp_reg() const noexcept {
        if (const auto reg = std::get_if<aasm::GPReg>(&m_reg)) {
            return *reg;
        }

        return std::unexpected(Error::CastError);
    }

    [[nodiscard]]
    std::expected<aasm::Address, Error> as_address() const noexcept {
        if (const auto addr = std::get_if<aasm::Address>(&m_reg)) {
            return *addr;
        }

        return std::unexpected(Error::CastError);
    }

    bool operator==(const GPVReg &other) const noexcept {
        if (this == &other) {
            return true;
        }

        return m_reg == other.m_reg;
    }

    friend std::ostream& operator<<(std::ostream& os, const GPVReg& reg) noexcept;

private:
    std::variant<aasm::GPReg, aasm::Address> m_reg;
};

inline std::ostream & operator<<(std::ostream &os, const GPVReg &reg) noexcept {
    const auto visitor = [&]<typename T>(const T &val) {
        if constexpr (std::is_same_v<T, aasm::GPReg>) {
            os << val.name(8);
        } else if constexpr (std::is_same_v<T, aasm::Address>) {
            os << val;
        } else {
            static_assert(false, "Unsupported type in GPVReg variant");
        }
    };

    std::visit(visitor, reg.m_reg);
    return os;
}

class OptionalGPVReg final {
public:
    template<GPVRegVariant T>
    OptionalGPVReg(const T& reg) noexcept:
        m_reg(reg) {}

    OptionalGPVReg(const GPVReg& reg) noexcept {
        const auto visitor = [&]<typename T>(const T &val) { m_reg = val; };
        reg.visit(visitor);
    }

    explicit OptionalGPVReg() noexcept: m_reg(std::monostate{}) {}

    [[nodiscard]]
    std::optional<GPVReg> to_gp_op() const noexcept {
        const auto visitor = [&]<typename T>(const T &val) -> std::optional<GPVReg> {
            if constexpr (GPVRegVariant<T>) {
                return GPVReg(val);
            }

            return std::nullopt;
        };

        return std::visit(visitor, m_reg);
    }

private:
    std::variant<std::monostate, aasm::GPReg, aasm::Address> m_reg;
};