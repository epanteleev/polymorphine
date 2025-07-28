#pragma once

#include <span>
#include <vector>
#include <ostream>
#include <utility>

#include "utility/Error.h"
#include "utility/OrderedSet.h"

template<typename Derived, typename Inst>
class BasicBlockBase {
public:
    explicit BasicBlockBase(): m_id(-1) {}

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

    [[nodiscard]]
    std::uint32_t size() const noexcept {
        const auto size = m_instructions.size();
        assertion(std::in_range<std::uint32_t>(size), "size={} is out of range", size);
        return size;
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
    void set_id(const std::size_t id) noexcept {
        assertion(std::in_range<std::uint32_t>(id), "id={} is out of range", id);
        m_id = id;
    }

    std::size_t m_id;
    std::vector<Derived *> m_predecessors;
    OrderedSet<Inst> m_instructions;
};
