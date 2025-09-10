#pragma once

#include <iosfwd>

#include "utility/OrderedSet.h"
#include "base/FunctionDataBase.h"
#include "mir/instruction/InstructionMatcher.h"

#include "mir/value/ArgumentValue.h"
#include "mir/module/BasicBlock.h"
#include "mir/module/FunctionPrototype.h"


class FunctionData final: public FunctionDataBase<BasicBlock, ArgumentValue> {
public:
    explicit FunctionData(const FunctionPrototype* prototype, std::vector<ArgumentValue>&& args) noexcept;

    FunctionData(FunctionData&& other) noexcept = default;

    [[nodiscard]]
    const ArgumentValue& arg(const std::size_t index) const {
        return m_args[index];
    }

    [[nodiscard]]
    std::span<const ArgumentValue> args() const {
        return m_args;
    }

    BasicBlock* create_basic_block() {
        const auto id = add_basic_block(std::make_unique<BasicBlock>());
        m_basic_blocks[id].set_id(id);
        return &m_basic_blocks[id];
    }

    [[nodiscard]]
    BasicBlock* last() const {
        const auto last_bb = m_basic_blocks.back();
        assertion(last_bb.has_value(), "last basic block is null");
        assertion(last_bb.value()->last().isa(any_return()), "last basic block is not a return block");
        return last_bb.value();
    }

    friend std::ostream &operator<<(std::ostream &os, const FunctionData &fd);

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_prototype->name();
    }

    std::size_t add_basic_block(std::unique_ptr<BasicBlock>&& bb) {
        return m_basic_blocks.push_back(std::move(bb));
    }

private:
    const FunctionPrototype* m_prototype;
};

static_assert(Function<FunctionData>, "assumed to be");