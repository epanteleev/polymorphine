#pragma once
#include <bitset>

#include "Register.h"

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
                m_idx = m_reg_map.find_prev_set_bit(m_idx);
                return *this;
            }

            Iterator& operator++() noexcept {
                m_idx++;
                m_idx = m_reg_map.find_next_set_bit(m_idx);
                return *this;
            }

            Iterator operator--(int) noexcept {
                Iterator temp = *this;
                ++*this;
                return temp;
            }

            Iterator operator++(int) noexcept {
                Iterator temp = *this;
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
        const_iterator find(const GPReg reg) noexcept {
            if (!contains(reg)) {
                return end();
            }
            return Iterator(*this, reg.code());
        }

        [[nodiscard]]
        const_iterator begin() const noexcept {
            return Iterator(*this, find_next_set_bit(0));
        }

        [[nodiscard]]
        const_iterator end() const noexcept {
            return Iterator(*this, GPReg::NUMBER_OF_GP_REGS);
        }

        [[nodiscard]]
        const_iterator rbegin() const noexcept {
            return Iterator(*this, find_prev_set_bit(GPReg::NUMBER_OF_GP_REGS));
        }

        [[nodiscard]]
        const_iterator rend() const noexcept {
            return Iterator(*this, GPReg::NUMBER_OF_GP_REGS);
        }

    private:
        [[nodiscard]]
        std::size_t find_next_set_bit(const std::size_t start) const noexcept {
            for (std::size_t i = start; i < GPReg::NUMBER_OF_GP_REGS; ++i) {
                if (m_has_values.test(i)) return i;
            }

            return GPReg::NUMBER_OF_GP_REGS; // No set bit found
        }

        [[nodiscard]]
        std::size_t find_prev_set_bit(const std::size_t start) const noexcept {
            for (std::size_t i = start; i > 0; --i) {
                if (m_has_values.test(i - 1)) return i - 1;
            }
            return -1; // No set bit found
        }

        std::bitset<GPReg::NUMBER_OF_GP_REGS> m_has_values{};
        std::array<GPReg, GPReg::NUMBER_OF_GP_REGS> m_regs{}; //TODO not to call default constructor
    };
}
