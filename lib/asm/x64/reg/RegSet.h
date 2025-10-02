#pragma once

#include <bitset>

#include "asm/asm_frwd.h"
#include "GPReg.h"
#include "utility/BitUtils.h"

namespace aasm {
    class RegSetIterator final {
        explicit RegSetIterator(const GPRegSet* reg_map, const std::size_t idx) noexcept:
            m_reg_set(reg_map),
            m_idx(idx) {}

        friend class GPRegSet;

    public:
        using value_type      = GPReg;
        using reference       = const GPReg;
        using difference_type = std::ptrdiff_t;
        using const_reference = reference;

        RegSetIterator() noexcept = default;

        reference operator*() const noexcept;

        RegSetIterator& operator++() noexcept;
        RegSetIterator operator++(int) noexcept {
            const RegSetIterator temp = *this;
            ++*this;
            return temp;
        }

        RegSetIterator& operator--() noexcept;
        RegSetIterator operator--(int) noexcept {
            const RegSetIterator temp = *this;
            --*this;
            return temp;
        }

        bool operator==(const RegSetIterator& other) const noexcept {
            if (this == &other) {
                return true;
            }

            return m_reg_set == other.m_reg_set && m_idx == other.m_idx;
        }

        bool operator!=(const RegSetIterator& other) const noexcept {
            return !(*this == other);
        }

    private:
        const GPRegSet* m_reg_set{};
        std::size_t m_idx{};
    };

    class GPRegSet final {
    public:
        using value_type = GPReg;
        using iterator   = RegSetIterator;
        using reference  = GPReg&;

        GPRegSet() noexcept = default;
        constexpr GPRegSet(const std::initializer_list<GPReg> list) noexcept {
            for (const auto& reg : list) {
                m_has_values.set(reg.code());
            }
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return m_has_values.count();
        };

        [[nodiscard]]
        bool empty() const noexcept {
            return m_has_values.none();
        }

        GPReg emplace(const GPReg& reg) noexcept {
            m_has_values.set(reg.code());
            return reg;
        }

        [[nodiscard]]
        bool contains(const GPReg reg) const noexcept {
            return m_has_values.test(reg.code());
        }

        [[nodiscard]]
        iterator find(const GPReg reg) const noexcept {
            if (!contains(reg)) {
                return end();
            }

            return RegSetIterator(this, reg.code());
        }

        [[nodiscard]]
        iterator begin() const noexcept {
            return RegSetIterator(this, bitutils::find_next_set_bit(m_has_values, 0));
        }

        [[nodiscard]]
        iterator end() const noexcept {
            return RegSetIterator(this, GPReg::NUMBER_OF_GP_REGS);
        }

    private:
        friend class RegSetIterator;
        std::bitset<GPReg::NUMBER_OF_GP_REGS> m_has_values{};
    };

    inline RegSetIterator::reference RegSetIterator::operator*() const noexcept {
        return GPReg(m_idx);
    }

    inline RegSetIterator &RegSetIterator::operator++() noexcept {
        m_idx++;
        m_idx = bitutils::find_next_set_bit(m_reg_set->m_has_values, m_idx);
        return *this;
    }

    inline RegSetIterator &RegSetIterator::operator--() noexcept {
        m_idx--;
        m_idx = bitutils::find_prev_set_bit(m_reg_set->m_has_values, m_idx);
        return *this;
    }
}

static_assert(std::ranges::range<aasm::GPRegSet>, "should be");
static_assert(std::ranges::bidirectional_range<aasm::GPRegSet>, "should be");