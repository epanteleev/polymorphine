#pragma once

#include <span>
#include <vector>
#include <utility>

#include "instruction_list/InstructionList.h"
#include "utility/Error.h"

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
     * Return the predecessor block at the given index.
     */
    const Derived* pred(const std::size_t idx) const {
        assertion(idx < m_predecessors.size(), "Index {} is out of range for predecessors", idx);
        return m_predecessors[idx];
    }

    /**
     * Returns the instructions in the block.
     */
    [[nodiscard]]
    const InstructionList<Inst>& instructions() const noexcept {
        return m_instructions;
    }

    [[nodiscard]]
    const InstructionList<Inst>& instructions() noexcept {
        return m_instructions;
    }

    /**
     * Get instruction by index.
     */
    [[nodiscard]]
    Inst& at(std::size_t idx) {
        return m_instructions.at(idx);
    }

    void remove_instruction(const Inst* id) {
        m_instructions.remove(id->id());
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
    template<typename Os>
    Os &print_short_name(Os &os) const {
        if (m_id == 0) {
            os << "entry";
        } else {
            os << 'L' << m_id;
        }

        return os;
    }

    template<typename Os>
    void print(Os &os) const {
        print_short_name(os);
        os << ":\n";

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

    template<typename T>
    static void make_edges(T *inst)  {
        for (auto block: inst->successors()) {
            block->m_predecessors.push_back(inst->owner());
        }
    }

    std::size_t m_id{NO_ID};
    std::vector<Derived *> m_predecessors;
    InstructionList<Inst> m_instructions;
};
