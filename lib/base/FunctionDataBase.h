#pragma once

#include <ostream>

#include "Constrains.h"
#include "utility/OrderedSet.h"


/**
 * Base class for function data in MIR and LIR.
 *
 * @tparam BB The type of code block (BasicBlock or LIRBlock).
 * @tparam Arg The type of argument (e.g., LIRArg).
 */
template<CodeBlock BB, typename Arg>
class FunctionDataBase {
public:
    using code_block_type = BB;
    using arg_type = Arg;

    explicit FunctionDataBase(std::vector<arg_type>&& args) noexcept:
        m_args(std::move(args)) {}

    FunctionDataBase(FunctionDataBase&& other) noexcept:
        m_args(std::move(other.m_args)),
        m_basic_blocks(std::move(other.m_basic_blocks)) {}

    /**
     * Returns begin block of the function.
     * The beginning block is the first basic block in the function.
     * It doesn't have predecessors.
     */
    [[nodiscard]]
    code_block_type* first() const {
        return m_basic_blocks.begin().get();
    }

    /**
     * Returns a number of basic blocks in the function.
     */
    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_basic_blocks.size();
    }

    [[nodiscard]]
    const OrderedSet<BB>& basic_blocks() const noexcept {
        return m_basic_blocks;
    }

    std::unique_ptr<code_block_type> remove(const code_block_type* bb) {
        return m_basic_blocks.remove(bb->id());
    }

protected:
    std::ostream& print_blocks(std::ostream &os) const {
        os << '{' << std::endl;
        for (const auto &bb : m_basic_blocks) {
            bb.print(os);
        }
        os << '}' << std::endl;
        return os;
    }

    std::vector<arg_type> m_args;
    OrderedSet<code_block_type> m_basic_blocks;
};


template<typename T>
concept Function = std::derived_from<T, FunctionDataBase<typename T::code_block_type, typename T::arg_type>>;