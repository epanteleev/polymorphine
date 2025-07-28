#pragma once

#include <iosfwd>

#include "mir/value/ArgumentValue.h"
#include "mir/module/BasicBlock.h"
#include "utility/OrderedSet.h"
#include "mir/module/FunctionPrototype.h"
#include "base/FunctionDataBase.h"


class FunctionData final: public FunctionDataBase<BasicBlock, ArgumentValue> {
public:
    explicit FunctionData(FunctionPrototype&& proto, std::vector<ArgumentValue>&& args);

    [[nodiscard]]
    const ArgumentValue& arg(const std::size_t index) const {
        return m_args[index];
    }

    [[nodiscard]]
    std::span<const ArgumentValue> args() const {
        return m_args;
    }

    BasicBlock* create_basic_block() {
        const auto id = m_basic_blocks.push_back(std::make_unique<BasicBlock>());
        m_basic_blocks[id].set_id(id);
        return &m_basic_blocks[id];
    }

    void print(std::ostream &os) const {
        os << "define ";
        m_prototype.print(os, m_args);
        os << ' ';
        print_blocks(os);
    }

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_prototype.name();
    }

private:
    FunctionPrototype m_prototype;
};

static_assert(Function<FunctionData>, "assumed to be");