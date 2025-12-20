#pragma once

#include <bitset>

#include "GPReg.h"
#include "XmmReg.h"
#include "utility/BitUtils.h"
#include "Index.h"

namespace aasm {
    template<typename Reg, typename V, std::size_t MAX_NOF_REGS0>
    class AnyRegMap;

    template<typename Reg, typename V, std::size_t MAX_NOF_REGS>
    class AnyRegMapIterator final {
        explicit AnyRegMapIterator(AnyRegMap<Reg, V, MAX_NOF_REGS>* reg_map, const std::size_t idx) noexcept:
            m_reg_map(reg_map),
            m_idx(idx) {}

    public:
        friend class AnyRegMap<Reg, V, MAX_NOF_REGS>;

        using value_type      = std::pair<Reg, V>;

        using reference       = std::pair<Reg, V>&;
        using const_reference = reference;

        using pointer         = std::pair<Reg, V>*;
        using const_pointer   = const std::pair<Reg, V>*;

        using difference_type = std::ptrdiff_t;

        AnyRegMapIterator() noexcept = default;

        const_reference operator*() const noexcept;
        reference operator*() noexcept;

        const_pointer operator->() const noexcept;
        pointer operator->() noexcept;

        AnyRegMapIterator& operator++() noexcept;
        AnyRegMapIterator operator++(int) noexcept {
            const AnyRegMapIterator temp = *this;
            ++*this;
            return temp;
        }

        AnyRegMapIterator& operator--() noexcept;
        AnyRegMapIterator operator--(int) noexcept {
            const AnyRegMapIterator temp = *this;
            --*this;
            return temp;
        }

        bool operator==(const AnyRegMapIterator& other) const noexcept {
            if (this == &other) {
                return true;
            }

            return m_reg_map == other.m_reg_map && m_idx == other.m_idx;
        }

        bool operator!=(const AnyRegMapIterator& other) const noexcept {
            return !(*this == other);
        }

    private:
        AnyRegMap<Reg, V, MAX_NOF_REGS>* m_reg_map{};
        std::size_t m_idx{};
    };

    template<typename Reg, typename V, std::size_t MAX_NOF_REGS>
    class AnyRegMap {
    public:
        using value_type = V;
        using iterator   = AnyRegMapIterator<Reg, V, MAX_NOF_REGS>;
        using const_iterator = AnyRegMapIterator<Reg, V, MAX_NOF_REGS>;

        using pair = std::pair<Reg, V>;
        using pair_pointer = std::pair<Reg, V>*;
        using reference = std::pair<Reg, V>&;
        using const_reference = const std::pair<Reg, V>&;

        ~AnyRegMap() { clear(); }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return m_has_values.count();
        };

        [[nodiscard]]
        bool empty() const noexcept {
            return m_has_values.none();
        }

        void clear() noexcept {
            for (std::size_t i{}; i < MAX_NOF_REGS; ++i) {
                if (m_has_values.test(i)) get(i)->~pair();
            }

            m_has_values.reset();
        }

        /**
         * Inserts a new value into the map if the register is not already present.
         * Returns an iterator to the inserted or existing element and a boolean indicating if it was inserted.
         */
        std::pair<pair_pointer, bool> try_emplace(const Reg reg, const V& value) noexcept {
            if (contains(reg)) {
                return {get(details::to_index(reg)), false};
            }

            m_has_values.set(details::to_index(reg));
            std::construct_at(get(details::to_index(reg)), reg, value);
            return {get(details::to_index(reg)), true};
        }

        [[nodiscard]]
        bool contains(const Reg reg) const noexcept {
            return m_has_values.test(details::to_index(reg));
        }

        [[nodiscard]]
        const_iterator find(const Reg reg) const noexcept {
            if (!contains(reg)) {
                return end();
            }
            return AnyRegMapIterator(this, details::to_index(reg));
        }

        [[nodiscard]]
        iterator begin() noexcept {
            return AnyRegMapIterator(this, bitutils::find_next_set_bit(m_has_values, 0));
        }

        [[nodiscard]]
        iterator end() noexcept {
            return AnyRegMapIterator(this, MAX_NOF_REGS);
        }

        [[nodiscard]]
        const_iterator begin() const noexcept {
            return AnyRegMapIterator(this, bitutils::find_next_set_bit(m_has_values, 0));
        }

        [[nodiscard]]
        const_iterator end() const noexcept {
            return AnyRegMapIterator(this, MAX_NOF_REGS);
        }

    private:
        friend class AnyRegMapIterator<Reg, V, MAX_NOF_REGS>;

        pair_pointer get(const std::size_t idx) noexcept {
            return reinterpret_cast<std::pair<Reg, V>*>(&m_regs[0]) + idx;
        }

        reference at(const std::size_t idx) noexcept {
            return *get(idx);
        }

        std::bitset<MAX_NOF_REGS> m_has_values{};
        alignas(pair) char m_regs[sizeof(pair) * MAX_NOF_REGS];
    };

    template<typename Reg, typename V, std::size_t MAX_NOF_REGS>
    AnyRegMapIterator<Reg, V, MAX_NOF_REGS>::pointer AnyRegMapIterator<Reg, V, MAX_NOF_REGS>::operator->() noexcept {
        return &m_reg_map->at(m_idx);
    }

    template<typename Reg, typename V, std::size_t MAX_NOF_REGS>
    AnyRegMapIterator<Reg, V, MAX_NOF_REGS>::const_pointer AnyRegMapIterator<Reg, V, MAX_NOF_REGS>::operator->() const noexcept {
        return &m_reg_map->at(m_idx);
    }

    template<typename Reg, typename V, std::size_t MAX_NOF_REGS>
    AnyRegMapIterator<Reg, V, MAX_NOF_REGS>::reference AnyRegMapIterator<Reg, V, MAX_NOF_REGS>::operator*() noexcept {
        return m_reg_map->at(m_idx);
    }

    template<typename Reg, typename V, std::size_t MAX_NOF_REGS>
    AnyRegMapIterator<Reg, V, MAX_NOF_REGS>::const_reference AnyRegMapIterator<Reg, V, MAX_NOF_REGS>::operator*() const noexcept {
        return m_reg_map->at(m_idx);
    }

    template<typename Reg, typename V, std::size_t MAX_NOF_REGS>
    AnyRegMapIterator<Reg, V, MAX_NOF_REGS>& AnyRegMapIterator<Reg, V, MAX_NOF_REGS>::operator++() noexcept {
        m_idx++;
        m_idx = bitutils::find_next_set_bit(m_reg_map->m_has_values, m_idx);
        return *this;
    }

    template<typename Reg, typename V, std::size_t MAX_NOF_REGS>
    AnyRegMapIterator<Reg, V, MAX_NOF_REGS>& AnyRegMapIterator<Reg, V, MAX_NOF_REGS>::operator--() noexcept {
        m_idx--;
        m_idx = bitutils::find_prev_set_bit(m_reg_map->m_has_values, m_idx);
        return *this;
    }

    template<typename V>
    class GPRegMap final: public AnyRegMap<GPReg, V, GPReg::NUMBER_OF_REGISTERS> {};

    template<typename V>
    class XmmRegMap final: public AnyRegMap<GPReg, V, XmmReg::NUMBER_OF_REGISTERS> {};

    template<typename V>
    class RegMap final: public AnyRegMap<Reg, V, GPReg::NUMBER_OF_REGISTERS+XmmReg::NUMBER_OF_REGISTERS> {};
}

static_assert(std::ranges::range<aasm::GPRegMap<int>>, "should be");
static_assert(std::ranges::bidirectional_range<aasm::GPRegMap<int>>, "should be");