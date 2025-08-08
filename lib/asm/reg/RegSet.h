#pragma once

#include <bitset>

#include "Register.h"
#include "utility/BitUtils.h"

namespace aasm {
    class GPRegSet final {
    public:
        class Iterator final {
            explicit Iterator(const GPRegSet& reg_map, const std::size_t idx) noexcept: m_reg_map(reg_map), m_idx(idx) {}
            friend class GPRegSet;
        public:
            using pointer = const GPReg*;
            using reference = const GPReg&;

            reference operator*() const noexcept {
                return m_reg_map.m_regs[m_idx];
            }

            pointer operator->() const noexcept {
                return &m_reg_map.m_regs[m_idx];
            }

            Iterator& operator--() noexcept {
                m_idx--;
                m_idx = bitutils::find_prev_set_bit(m_reg_map.m_has_values, m_idx);
                return *this;
            }

            Iterator& operator++() noexcept {
                m_idx++;
                m_idx = bitutils::find_next_set_bit(m_reg_map.m_has_values, m_idx);
                return *this;
            }

            Iterator operator--(int) noexcept {
                const Iterator temp = *this;
                --*this;
                return temp;
            }

            Iterator operator++(int) noexcept {
                const Iterator temp = *this;
                ++*this;
                return temp;
            }

            bool operator==(const Iterator& other) const noexcept {
                return &m_reg_map == &other.m_reg_map && m_idx == other.m_idx;
            }

        private:
            const GPRegSet& m_reg_map;
            std::size_t m_idx;
        };

        using const_iterator = Iterator;

        [[nodiscard]]
        constexpr auto size() const noexcept {
            return m_has_values.count();
        };

        [[nodiscard]]
        constexpr bool empty() const noexcept {
            return m_has_values.none();
        }

        GPReg& emplace(const GPReg reg) noexcept {
            m_has_values.set(reg.code());
            m_regs[reg.code()] = reg;
            return m_regs[reg.code()];
        }

        [[nodiscard]]
        bool contains(const GPReg reg) const noexcept {
            return m_has_values.test(reg.code());
        }

        [[nodiscard]]
        const_iterator find(const GPReg reg) const noexcept {
            if (!contains(reg)) {
                return end();
            }
            return Iterator(*this, reg.code());
        }

        [[nodiscard]]
        const_iterator begin() const noexcept {
            return Iterator(*this, bitutils::find_next_set_bit(m_has_values, 0));
        }

        [[nodiscard]]
        const_iterator end() const noexcept {
            return Iterator(*this, GPReg::NUMBER_OF_GP_REGS);
        }

        [[nodiscard]]
        const_iterator rbegin() const noexcept {
            return Iterator(*this, bitutils::find_prev_set_bit(m_has_values, GPReg::NUMBER_OF_GP_REGS-1));
        }

        [[nodiscard]]
        const_iterator rend() const noexcept {
            return Iterator(*this, GPReg::NUMBER_OF_GP_REGS);
        }

    private:
        std::bitset<GPReg::NUMBER_OF_GP_REGS> m_has_values{};
        GPReg m_regs[GPReg::NUMBER_OF_GP_REGS];
    };
}
