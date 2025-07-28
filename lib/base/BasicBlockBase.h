#pragma once

#include <span>
#include <vector>
#include <ostream>
#include <utility>

#include "utility/Error.h"
#include "utility/OrderedSet.h"

/**
 * Base class for basic blocks in MIR an LIR.
 * This class provides common functionality for basic blocks, such as managing
 * instructions, predecessors, and block IDs etc.
 *
 * @tparam Derived The derived type of the basic block. It must be satisfied to concept @class CodeBlock.
 * @tparam Inst The type of instructions contained in the block.
 */
template<typename Derived, typename Inst>
class BasicBlockBase {
    static constexpr auto NO_ID = std::numeric_limits<std::size_t>::max();

public:
    explicit BasicBlockBase(): m_id(NO_ID) {}

    virtual ~BasicBlockBase() = default;

    /**
     * Returns the unique identifier of the block in the function.
     */
    [[nodiscard]]
    std::size_t id() const noexcept {
        assertion(m_id != NO_ID, "Block ID is not set");
        return m_id;
    }

    /**
     * Returns the predecessors of this block.
     * @return span of pointers to predecessor blocks. It is empty if there are no predecessors.
     */
    [[nodiscard]]
    std::span<Derived* const> predecessors() const {
        return m_predecessors;
    }

    /**
     * Returns the instructions in the block.
     */
    [[nodiscard]]
    const OrderedSet<Inst>& instructions() const noexcept {
        return m_instructions;
    }

    /**
     * Returns number of instructions in the block.
     */
    [[nodiscard]]
    std::uint32_t size() const noexcept {
        const auto size = m_instructions.size();
        assertion(std::in_range<std::uint32_t>(size), "size={} is out of range", size);
        return size;
    }

    /**
     * Prints only label name of the block to the output stream.
     */
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
