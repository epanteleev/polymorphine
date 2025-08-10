#pragma once

#include <array>
#include <bitset>

#include "asm/asm_frwd.h"
#include "Register.h"
#include "utility/BitUtils.h"

namespace aasm {
    template<typename V>
    class RegMapIterator final {
        explicit RegMapIterator(GPRegMap<V>* reg_map, const std::size_t idx) noexcept:
            m_reg_map(reg_map),
            m_idx(idx) {}

        friend class GPRegMap<V>;

    public:
        using value_type      = std::pair<GPReg, V>;

        using reference       = std::pair<GPReg, V>&;
        using const_reference = reference;

        using pointer         = std::pair<GPReg, V>*;
        using const_pointer   = const std::pair<GPReg, V>*;

        using difference_type = std::ptrdiff_t;

        RegMapIterator() noexcept = default;

        const_reference operator*() const noexcept;
        reference operator*() noexcept;

        const_pointer operator->() const noexcept;
        pointer operator->() noexcept;

        RegMapIterator& operator++() noexcept;
        RegMapIterator operator++(int) noexcept {
            const RegMapIterator temp = *this;
            ++*this;
            return temp;
        }

        RegMapIterator& operator--() noexcept;
        RegMapIterator operator--(int) noexcept {
            const RegMapIterator temp = *this;
            --*this;
            return temp;
        }

        bool operator==(const RegMapIterator& other) const noexcept {
            if (this == &other) {
                return true;
            }

            return m_reg_map == other.m_reg_map && m_idx == other.m_idx;
        }

        bool operator!=(const RegMapIterator& other) const noexcept {
            return !(*this == other);
        }

    private:
        GPRegMap<V>* m_reg_map{};
        std::size_t m_idx{};
    };

    template<typename V>
    class GPRegMap final {
    public:
        using value_type = V;
        using iterator   = RegMapIterator<V>;
        using const_iterator = RegMapIterator<V>;

        using reference = std::pair<GPReg, V>&;
        using const_reference = const std::pair<GPReg, V>&;

        [[nodiscard]]
        constexpr std::size_t size() const noexcept {
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
        std::pair<iterator, bool> try_emplace(const GPReg reg, const V& value) noexcept {
            if (contains(reg)) {
                return {RegMapIterator(this, reg.code()), false};
            }

            m_has_values.set(reg.code());
            m_regs[reg.code()] = std::make_pair(reg, value);
            return {RegMapIterator(this, reg.code()), true};
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
            return RegMapIterator(this, reg.code());
        }

        [[nodiscard]]
        iterator begin() noexcept {
            return RegMapIterator(this, bitutils::find_next_set_bit(m_has_values, 0));
        }

        [[nodiscard]]
        iterator end() noexcept {
            return RegMapIterator(this, GPReg::NUMBER_OF_GP_REGS);
        }

        [[nodiscard]]
        const_iterator begin() const noexcept {
            return RegMapIterator(this, bitutils::find_next_set_bit(m_has_values, 0));
        }

        [[nodiscard]]
        const_iterator end() const noexcept {
            return RegMapIterator(this, GPReg::NUMBER_OF_GP_REGS);
        }

    private:
        friend class RegMapIterator<V>;
        std::bitset<GPReg::NUMBER_OF_GP_REGS> m_has_values{};
        std::array<std::pair<GPReg, V>, GPReg::NUMBER_OF_GP_REGS> m_regs; //TODO not to call default constructor
    };

    template<typename V>
    RegMapIterator<V>::pointer RegMapIterator<V>::operator->() noexcept {
        return &m_reg_map->m_regs[m_idx];
    }

    template<typename V>
    RegMapIterator<V>::const_pointer RegMapIterator<V>::operator->() const noexcept {
        return &m_reg_map->m_regs[m_idx];
    }

    template<typename V>
    RegMapIterator<V>::reference RegMapIterator<V>::operator*() noexcept {
        return m_reg_map->m_regs[m_idx];
    }

    template<typename V>
    RegMapIterator<V>::const_reference RegMapIterator<V>::operator*() const noexcept {
        return m_reg_map->m_regs[m_idx];
    }

    template<typename V>
    RegMapIterator<V>& RegMapIterator<V>::operator++() noexcept {
        m_idx++;
        m_idx = bitutils::find_next_set_bit(m_reg_map->m_has_values, m_idx);
        return *this;
    }

    template<typename V>
    RegMapIterator<V>& RegMapIterator<V>::operator--() noexcept {
        m_idx--;
        m_idx = bitutils::find_prev_set_bit(m_reg_map->m_has_values, m_idx);
        return *this;
    }
}

static_assert(std::ranges::range<aasm::GPRegMap<int>>, "should be");
static_assert(std::ranges::bidirectional_range<aasm::GPRegMap<int>>, "should be");