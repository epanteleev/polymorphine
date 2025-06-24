#pragma once

#include <iosfwd>

#include "value/ArgumentValue.h"
#include "BasicBlock.h"
#include "utility/OrderedSet.h"
#include "FunctionPrototype.h"

class FunctionData final {
public:
    using code_block_type = BasicBlock;

    explicit FunctionData(FunctionPrototype&& proto, std::vector<ArgumentValue>&& args);

    BasicBlock* create_basic_block() {
        const auto creator = [this](std::size_t id) {
            return std::make_unique<BasicBlock>(id);
        };

        return m_basic_blocks.push_back<BasicBlock>(creator);
    }

    void print(std::ostream& os) const;

    [[nodiscard]]
    BasicBlock* first() const {
        return m_basic_blocks.begin().get();
    }

    [[nodiscard]]
    BasicBlock* last() const;

    [[nodiscard]]
    const ArgumentValue *arg(const std::size_t index) const {
        return &m_args[index];
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_basic_blocks.size();
    }

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_prototype.name();
    }

    [[nodiscard]]
    const OrderedSet<BasicBlock>& basic_blocks() const noexcept {
        return m_basic_blocks;
    }

private:
    FunctionPrototype m_prototype;
    std::vector<ArgumentValue> m_args;
    OrderedSet<BasicBlock> m_basic_blocks;
};


static_assert(Function<FunctionData>, "assumed to be");