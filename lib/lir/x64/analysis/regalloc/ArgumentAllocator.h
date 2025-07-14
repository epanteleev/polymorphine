#pragma once
#include <array>

#include "utility/Error.h"

template<typename Reg, std::size_t N>
class ArgumentAllocator final {
public:
    explicit ArgumentAllocator(const std::array<Reg, N>& regs):
        m_regs(regs) {}

    Reg get_reg() noexcept {
        if (m_gp_reg_pos < N) {
            return m_regs[m_gp_reg_pos++];
        }

        unimplemented();
    }

private:
    std::size_t m_gp_reg_pos{};
    const std::array<Reg, N>& m_regs;
};
