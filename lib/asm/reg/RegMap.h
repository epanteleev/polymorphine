#pragma once

#include <array>
#include <bitset>

#include "Register.h"
#include "utility/BitUtils.h"

namespace aasm {
    template<typename V>
    class GPRegMap final {
    public:
        class Iterator final {
            explicit Iterator(GPRegMap& reg_map, const std::size_t idx) noexcept: m_reg_map(reg_map), m_idx(idx) {}
            friend class GPRegMap;
        public:
            using pointer = std::pair<GPReg, V>*;
            using reference = std::pair<GPReg, V>&;

            reference operator*() noexcept {
                return m_reg_map.m_regs[m_idx];
            }

            pointer operator->() noexcept {
                return &m_reg_map.m_regs[m_idx];
            }

            Iterator& operator++() noexcept {
                m_idx++;
                m_idx = bitutils::find_next_set_bit(m_reg_map.m_has_values, m_idx);
                return *this;
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
            GPRegMap& m_reg_map;
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

        /**
         * Inserts a new value into the map if the register is not already present.
         * Returns an iterator to the inserted or existing element and a boolean indicating if it was inserted.
         */
        std::pair<const_iterator, bool> try_emplace(const GPReg reg, const V& value) noexcept {
            if (contains(reg)) {
                return {Iterator(*this, reg.code()), false};
            }

            m_has_values.set(reg.code());
            m_regs[reg.code()] = std::make_pair(reg, value);
            return {Iterator(*this, reg.code()), true};
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
        const_iterator begin() noexcept {
            return Iterator(*this, bitutils::find_next_set_bit(m_has_values, 0));
        }

        [[nodiscard]]
        const_iterator end() noexcept {
            return Iterator(*this, GPReg::NUMBER_OF_GP_REGS);
        }

    private:
        std::bitset<GPReg::NUMBER_OF_GP_REGS> m_has_values{};
        std::array<std::pair<GPReg, V>, GPReg::NUMBER_OF_GP_REGS> m_regs; //TODO not to call default constructor
    };
}