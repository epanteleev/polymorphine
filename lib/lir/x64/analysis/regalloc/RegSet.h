#pragma once

#include "lir/x64/asm/cc/LinuxX64.h"

namespace details {
    template<call_conv::CallConv CC>
    class RegSet final {
    public:
        using stack = std::vector<aasm::GPReg>;

        explicit RegSet(stack&& free_regs) noexcept:
            m_free_regs(std::move(free_regs)) {}

        template<std::ranges::range Range>
        static RegSet create(Range&& arg_regs) {
            stack regs{};
            regs.reserve(CC::GP_CALLER_SAVE_REGISTERS.size());

            for (const auto reg: CC::GP_CALLER_SAVE_REGISTERS) {
                if (std::ranges::contains(arg_regs, reg)) {
                    continue;
                }

                regs.push_back(reg);
            }

            return RegSet(std::move(regs));
        }

        [[nodiscard]]
        aasm::GPReg top() const noexcept {
            assertion(!m_free_regs.empty(), "Attempted to access top of an empty register set");
            return m_free_regs.back();
        }

        void pop() noexcept {
            assertion(!m_free_regs.empty(), "Attempted to pop from an empty register set");
            m_free_regs.pop_back();
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

    private:
        stack m_free_regs{};
    };
}
