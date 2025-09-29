#pragma once

#include "lir/x64/analysis/intervals/IntervalHint.h"
#include "lir/x64/asm/cc/LinuxX64.h"
#include "utility/ArithmeticUtils.h"

namespace details {
    class VRegSelection final {
    public:
        using stack = std::vector<aasm::GPReg>;

        explicit VRegSelection(stack&& free_regs, const call_conv::CallConvProvider* call_conv) noexcept:
            m_free_regs(std::move(free_regs)),
            m_call_conv(call_conv) {}

        aasm::Address stack_alloc(const std::size_t size, const std::size_t align) noexcept {
            m_local_area_size = align_up(m_local_area_size, align) + size;
            return aasm::Address(aasm::rbp, -m_local_area_size);
        }

        [[nodiscard]]
        aasm::GPReg top(const IntervalHint hint) noexcept {
            switch (hint) {
                case IntervalHint::NOTHING: {
                    for (const auto reg: std::ranges::reverse_view(m_free_regs)) {
                        if (!std::ranges::contains(m_call_conv->GP_CALLER_SAVE_REGISTERS(), reg)) {
                            continue;
                        }

                        remove(reg);
                        return reg;
                    }

                    [[fallthrough]];
                }
                case IntervalHint::CALL_LIVE_OUT: {
                    assertion(!m_free_regs.empty(), "Attempted to access top of an empty register set");
                    const auto reg = m_free_regs.back();
                    m_free_regs.pop_back();
                    return reg;
                }
                default: std::unreachable();
            }
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
        static VRegSelection create(const call_conv::CallConvProvider* call_conv, Range&& arg_regs) {
            stack regs{};
            regs.reserve(call_conv->ALL_GP_REGISTERS().size());

            for (const auto reg: call_conv->ALL_GP_REGISTERS()) {
                if (std::ranges::contains(arg_regs, reg)) {
                    continue;
                }

                regs.push_back(reg);
            }

            return VRegSelection(std::move(regs), call_conv);
        }

    private:
        stack m_free_regs{};
        std::int32_t m_local_area_size{};
        const call_conv::CallConvProvider* m_call_conv;
    };
}
