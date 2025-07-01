#pragma once
#include <span>
#include <vector>
#include <ostream>

#include "utility/OrderedSet.h"

template<typename Derived, typename Inst>
class BasicBlockBase {
public:
    explicit BasicBlockBase(const std::size_t id): m_id(id) {}

    virtual ~BasicBlockBase() = default;

    [[nodiscard]]
    std::size_t id() const noexcept { return m_id; }

    [[nodiscard]]
    std::span<Derived* const> predecessors() const {
        return m_predecessors;
    }

    [[nodiscard]]
    const OrderedSet<Inst>& instructions() const noexcept {
        return m_instructions;
    }

    std::ostream &print_short_name(std::ostream &os) const {
        if (m_id == 0) {
            os << "entry";
        } else {
            os << 'L' << m_id;
        }

        return os;
    }

    void print(std::ostream &os) const {
        print_short_name(os);
        os << ':' << std::endl;

        for (const auto &inst : m_instructions) {
            os << "  ";
            inst.print(os);
            os << '\n';
        }
    }

protected:
    const std::size_t m_id;
    std::vector<Derived *> m_predecessors;
    OrderedSet<Inst> m_instructions;
};
