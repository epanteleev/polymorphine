#pragma once

#include <bitset>
#include "asm/asm_frwd.h"
#include "Reg.h"
#include "utility/BitUtils.h"

namespace aasm {
    template<typename Reg, std::size_t MAX_NOF_REGS>
    class AnyRegSet;

    template<typename RegT, std::size_t MAX_NOF_REGS>
    class RegSetIterator final {
        template<typename R, std::size_t M>
        friend class AnyRegSet;

        explicit constexpr RegSetIterator(const AnyRegSet<RegT, MAX_NOF_REGS>* reg_map, const std::size_t idx) noexcept:
            m_reg_set(reg_map),
            m_idx(idx) {}

    public:
        using value_type      = RegT;
        using reference       = const RegT;
        using difference_type = std::ptrdiff_t;
        using const_reference = reference;

        constexpr RegSetIterator() noexcept = default;

        constexpr reference operator*() const noexcept;

        constexpr RegSetIterator& operator++() noexcept;
        constexpr RegSetIterator operator++(int) noexcept {
            const RegSetIterator temp = *this;
            ++*this;
            return temp;
        }

        constexpr RegSetIterator& operator--() noexcept;
        constexpr RegSetIterator operator--(int) noexcept {
            const RegSetIterator temp = *this;
            --*this;
            return temp;
        }

        constexpr bool operator==(const RegSetIterator& other) const noexcept {
            if (this == &other) {
                return true;
            }

            return m_reg_set == other.m_reg_set && m_idx == other.m_idx;
        }

        constexpr bool operator!=(const RegSetIterator& other) const noexcept {
            return !(*this == other);
        }

    private:
        template<typename R>
        static constexpr R from_index(const std::size_t idx) noexcept {
            if constexpr (std::same_as<R, XmmReg>) {
                return RegT(idx+xmm0.code());
            } else if constexpr (std::same_as<R, GPReg>) {
                return RegT(idx);

            } else if constexpr (std::same_as<R, Reg>) {
                if (idx < GPReg::NUMBER_OF_REGISTERS) {
                    return RegT(GPReg(idx));
                }
                return RegT(XmmReg(idx));

            } else {
                static_assert(false, "unexpected type");
                std::unreachable();
            }
        }

        const AnyRegSet<RegT, MAX_NOF_REGS>* m_reg_set{};
        std::size_t m_idx{};
    };

    template<typename RegT, std::size_t MAX_NOF_REGS>
    class AnyRegSet {
    public:
        using value_type = RegT;
        using iterator   = RegSetIterator<RegT, MAX_NOF_REGS>;
        using reference  = RegT&;

        constexpr AnyRegSet() noexcept = default;
        constexpr AnyRegSet(const std::initializer_list<RegT>& list) noexcept {
            for (const auto& reg : list) {
                m_has_values.set(to_index(reg));
            }
        }

        [[nodiscard]]
        constexpr std::size_t size() const noexcept {
            return m_has_values.count();
        };

        [[nodiscard]]
        constexpr bool empty() const noexcept {
            return m_has_values.none();
        }

        constexpr RegT emplace(const RegT& reg) noexcept {
            m_has_values.set(to_index(reg));
            return reg;
        }

        [[nodiscard]]
        constexpr bool contains(const RegT reg) const noexcept {
            return m_has_values.test(to_index(reg));
        }

        [[nodiscard]]
        constexpr iterator find(const RegT reg) const noexcept {
            if (!contains(reg)) {
                return end();
            }

            return RegSetIterator<RegT, MAX_NOF_REGS>(this, to_index(reg));
        }

        [[nodiscard]]
        constexpr iterator begin() const noexcept {
            return RegSetIterator(this, bitutils::find_next_set_bit(m_has_values, 0));
        }

        [[nodiscard]]
        constexpr iterator end() const noexcept {
            return RegSetIterator(this, MAX_NOF_REGS);
        }

    private:
        template<typename R, std::size_t M>
        friend class RegSetIterator;

        template<typename R>
        static constexpr std::size_t to_index(const R& reg) noexcept {
            if constexpr (std::same_as<R, XmmReg>) {
                return reg.code()-xmm0.code();
            } else if constexpr (std::same_as<R, GPReg> || std::same_as<R, Reg>) {
                return reg.code();
            } else {
                static_assert(false, "unexpected type");
                std::unreachable();
            }
        }

    protected:
        std::bitset<MAX_NOF_REGS> m_has_values{};
    };

    template<typename Reg, std::size_t MAX_NOF_REGS>
    constexpr RegSetIterator<Reg, MAX_NOF_REGS>::reference RegSetIterator<Reg, MAX_NOF_REGS>::operator*() const noexcept {
        return from_index<Reg>(m_idx);
    }

    template<typename Reg, std::size_t MAX_NOF_REGS>
    constexpr RegSetIterator<Reg, MAX_NOF_REGS> &RegSetIterator<Reg, MAX_NOF_REGS>::operator++() noexcept {
        m_idx++;
        m_idx = bitutils::find_next_set_bit(m_reg_set->m_has_values, m_idx);
        return *this;
    }

    template<typename Reg, std::size_t MAX_NOF_REGS>
    constexpr RegSetIterator<Reg, MAX_NOF_REGS> &RegSetIterator<Reg, MAX_NOF_REGS>::operator--() noexcept {
        m_idx--;
        m_idx = bitutils::find_prev_set_bit(m_reg_set->m_has_values, m_idx);
        return *this;
    }

    class GPRegSet final: public AnyRegSet<GPReg, GPReg::NUMBER_OF_REGISTERS> {};
    class XmmRegSet final: public AnyRegSet<XmmReg, XmmReg::NUMBER_OF_REGISTERS> {};
    class RegSet final: public AnyRegSet<Reg, GPReg::NUMBER_OF_REGISTERS+XmmReg::NUMBER_OF_REGISTERS> {
    public:
        [[nodiscard]]
        GPRegSet gp_regs() const noexcept { // TODO make this more efficient
            GPRegSet gp_set;
            for (const auto& reg: *this) {
                if (const auto gp_reg = reg.as_gp_reg(); gp_reg.has_value()) {
                    gp_set.emplace(gp_reg.value());
                }
            }
            return gp_set;
        }

        [[nodiscard]]
        XmmRegSet xmm_regs() const noexcept { // TODO make this more efficient
            XmmRegSet xmm_set;
            for (const auto& reg: *this) {
                if (const auto xmm_reg = reg.as_xmm_reg(); xmm_reg.has_value()) {
                    xmm_set.emplace(xmm_reg.value());
                }
            }
            return xmm_set;
        }
    };

}

static_assert(std::ranges::range<aasm::GPRegSet>, "should be");
static_assert(std::ranges::bidirectional_range<aasm::GPRegSet>, "should be");