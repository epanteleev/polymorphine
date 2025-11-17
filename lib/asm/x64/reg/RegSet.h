#pragma once

#include <bitset>

#include "asm/asm_frwd.h"
#include "GPReg.h"
#include "XmmReg.h"
#include "utility/BitUtils.h"

namespace aasm {
    template<typename Reg, std::size_t MAX_NOF_REGS>
    class RegSet;

    template<typename Reg, std::size_t MAX_NOF_REGS>
    class RegSetIterator final {
        template<typename R, std::size_t M>
        friend class RegSet;

        explicit RegSetIterator(const RegSet<Reg, MAX_NOF_REGS> * reg_map, const std::size_t idx) noexcept:
            m_reg_set(reg_map),
            m_idx(idx) {}

    public:
        using value_type      = Reg;
        using reference       = const Reg;
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
        template<typename R>
        static constexpr R from_index(const std::size_t idx) noexcept {
            if constexpr (std::same_as<R, XmmReg>) {
                return Reg(idx+xmm0.code());
            } else if constexpr (std::same_as<R, GPReg>) {
                return Reg(idx);
            } else {
                static_assert(false, "unexpected type");
                std::unreachable();
            }
        }

        const RegSet<Reg, MAX_NOF_REGS>* m_reg_set{};
        std::size_t m_idx{};
    };

    template<typename Reg, std::size_t MAX_NOF_REGS>
    class RegSet {
    public:
        using value_type = Reg;
        using iterator   = RegSetIterator<Reg, MAX_NOF_REGS>;
        using reference  = Reg&;

        RegSet() noexcept = default;
        constexpr RegSet(const std::initializer_list<Reg> list) noexcept {
            for (const auto& reg : list) {
                m_has_values.set(to_index(reg));
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

        Reg emplace(const Reg& reg) noexcept {
            m_has_values.set(to_index(reg));
            return reg;
        }

        [[nodiscard]]
        bool contains(const Reg reg) const noexcept {
            return m_has_values.test(to_index(reg));
        }

        [[nodiscard]]
        iterator find(const Reg reg) const noexcept {
            if (!contains(reg)) {
                return end();
            }

            return RegSetIterator<Reg, MAX_NOF_REGS>(this, to_index(reg));
        }

        [[nodiscard]]
        iterator begin() const noexcept {
            return RegSetIterator(this, bitutils::find_next_set_bit(m_has_values, 0));
        }

        [[nodiscard]]
        iterator end() const noexcept {
            return RegSetIterator(this, MAX_NOF_REGS);
        }

    private:
        template<typename R, std::size_t M>
        friend class RegSetIterator;

        template<typename R>
        static constexpr std::size_t to_index(const R& reg) noexcept {
            if constexpr (std::same_as<R, XmmReg>) {
                return reg.code()-xmm0.code();
            } else if constexpr (std::same_as<R, GPReg>) {
                return reg.code();
            } else {
                static_assert(false, "unexpected type");
                std::unreachable();
            }
        }

        std::bitset<MAX_NOF_REGS> m_has_values{};
    };

    template<typename Reg, std::size_t MAX_NOF_REGS>
    RegSetIterator<Reg, MAX_NOF_REGS>::reference RegSetIterator<Reg, MAX_NOF_REGS>::operator*() const noexcept {
        return from_index<Reg>(m_idx);
    }

    template<typename Reg, std::size_t MAX_NOF_REGS>
    RegSetIterator<Reg, MAX_NOF_REGS> &RegSetIterator<Reg, MAX_NOF_REGS>::operator++() noexcept {
        m_idx++;
        m_idx = bitutils::find_next_set_bit(m_reg_set->m_has_values, m_idx);
        return *this;
    }

    template<typename Reg, std::size_t MAX_NOF_REGS>
    RegSetIterator<Reg, MAX_NOF_REGS> &RegSetIterator<Reg, MAX_NOF_REGS>::operator--() noexcept {
        m_idx--;
        m_idx = bitutils::find_prev_set_bit(m_reg_set->m_has_values, m_idx);
        return *this;
    }

    class GPRegSet final: public RegSet<GPReg, GPReg::NUMBER_OF_REGISTERS> {};
    class XmmRegSet final: public RegSet<XmmReg, XmmReg::NUMBER_OF_REGISTERS> {};
}

static_assert(std::ranges::range<aasm::GPRegSet>, "should be");
static_assert(std::ranges::bidirectional_range<aasm::GPRegSet>, "should be");