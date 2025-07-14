#pragma once

#include <ostream>

#include "Constrains.h"
#include "utility/OrderedSet.h"


template<CodeBlock BB, typename Arg>
class FunctionDataBase {
public:
    using code_block_type = BB;
    using arg_type = Arg;

    explicit FunctionDataBase(std::vector<arg_type>&& args) :
        m_args(std::move(args)) {}

    virtual ~FunctionDataBase() = default;

    [[nodiscard]]
    code_block_type* first() const {
        return m_basic_blocks.begin().get();
    }

    [[nodiscard]]
    code_block_type* last() const {
        const auto last_bb = m_basic_blocks.back();
        return last_bb.value();
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_basic_blocks.size();
    }

    [[nodiscard]]
    const OrderedSet<BB>& basic_blocks() const noexcept {
        return m_basic_blocks;
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