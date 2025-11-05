#pragma once

#include "GPVReg.h"
#include "Constrains.h"
#include "asm/x64/asm.h"

class GPOp final {
public:
    template<GPOpVariant T>
    GPOp(const T& reg) noexcept:
        m_reg(reg) {}

    GPOp(const GPVReg& vreg) noexcept: m_reg(aasm::rax) {
        vreg.visit([&](const auto& reg) { m_reg = reg; });
    }

    [[nodiscard]]
    std::expected<aasm::Address, Error> as_address() const noexcept {
        if (const auto addr = std::get_if<aasm::Address>(&m_reg)) {
            return *addr;
        }

        return std::unexpected(Error::CastError);
    }

    friend std::ostream& operator<<(std::ostream& os, const GPOp& reg) noexcept;

    template<typename T>
    void visit(const T& visitor) const {
        std::visit(visitor, m_reg);
    }

private:
    std::variant<aasm::GPReg, aasm::Address, std::int64_t> m_reg;
};