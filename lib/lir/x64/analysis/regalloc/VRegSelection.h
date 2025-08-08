#pragma once

#include "lir/x64/asm/cc/LinuxX64.h"
#include "utility/Align.h"

namespace details {
    template<call_conv::CallConv CC>
    class VRegSelection final {
    public:
        using stack = std::vector<aasm::GPReg>;

        explicit VRegSelection(stack&& free_regs) noexcept:
            m_free_regs(std::move(free_regs)) {}

        aasm::Address stack_alloc(const std::size_t size) noexcept {
            m_local_area_size = align_up(m_local_area_size, size) + size;
            return aasm::Address(aasm::rbp, -m_local_area_size);
        }

        [[nodiscard]]
        aasm::GPReg top() noexcept {
            assertion(!m_free_regs.empty(), "Attempted to access top of an empty register set");
            const auto reg = m_free_regs.back();
            m_free_regs.pop_back();
            return reg;
        }

        void push(const aasm::GPReg reg) noexcept {
            if (std::ranges::contains(m_free_regs, reg)) {
                return;
            }

            m_free_regs.push_back(reg);
        }

        void remove(const aasm::GPReg reg) noexcept {
            std::erase(m_free_regs, reg);
        }

        [[nodiscard]]
        bool empty() const noexcept {
            return m_free_regs.empty();
        }

        [[nodiscard]]
        std::int32_t local_area_size() const noexcept {
            return m_local_area_size;
        }

        template<std::ranges::range Range>
        static VRegSelection create(Range&& arg_regs) {
            stack regs{};
            regs.reserve(CC::ALL_GP_REGISTERS.size());

            for (const auto reg: CC::ALL_GP_REGISTERS) {
                if (std::ranges::contains(arg_regs, reg)) {
                    continue;
                }

                regs.push_back(reg);
            }

            return VRegSelection(std::move(regs));
        }

    private:
        stack m_free_regs{};
        std::int32_t m_local_area_size{0};
    };
}
