#pragma once

#include <ostream>

#include "pass/Constrains.h"
#include "utility/OrderedSet.h"

template<CodeBlock BB, typename Arg>
class FunctionDataBase {
public:
    using code_block_type = BB;

    explicit FunctionDataBase(std::vector<Arg>&& args) :
        m_args(std::move(args)) {}

    virtual ~FunctionDataBase() = default;

    [[nodiscard]]
    BB* first() const {
        return m_basic_blocks.begin().get();
    }

    [[nodiscard]]
    BB* last() const {
        const auto last_bb = m_basic_blocks.back();
        return last_bb.value();
    }

    [[nodiscard]]
    const Arg& arg(const std::size_t index) const {
        return m_args[index];
    }

    [[nodiscard]]
    std::span<const Arg> args() const {
        return m_args;
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

    std::vector<Arg> m_args;
    OrderedSet<BB> m_basic_blocks;
};
