#pragma once

#include <iosfwd>

#include "utility/OrderedSet.h"
#include "base/FunctionDataBase.h"
#include "mir/global/GValuePool.h"
#include "mir/instruction/InstructionMatcher.h"

#include "mir/value/ArgumentValue.h"
#include "mir/module/BasicBlock.h"
#include "mir/module/FunctionPrototype.h"


class FunctionData final: public FunctionDataBase<BasicBlock, ArgumentValue> {
public:
    explicit FunctionData(std::size_t uid, const FunctionPrototype* prototype, std::vector<ArgumentValue>&& args) noexcept;

    FunctionData(FunctionData&& other) noexcept = default;

    [[nodiscard]]
    const ArgumentValue& arg(const std::size_t index) const {
        assertion(index < m_args.size(), "invariant");
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
        assertion(last_bb != m_basic_blocks.end(), "last basic block is null");
        assertion(last_bb->last().isa(any_return()), "last basic block is not a return block");
        return last_bb.get();
    }

    friend std::ostream &operator<<(std::ostream &os, const FunctionData &fd);

    [[nodiscard]]
    const FunctionPrototype* prototype() const noexcept {
        return m_prototype;
    }

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_prototype->name();
    }

    std::size_t add_basic_block(std::unique_ptr<BasicBlock>&& bb) {
        return m_basic_blocks.push_back(std::move(bb));
    }

    [[nodiscard]]
    std::expected<const GlobalValue*, Error> add_constant(const std::string_view name, const NonTrivialType* type, Initializer&& value) {
        return m_local_constant_pool.add_constant(name, type, std::move(value));
    }

    [[nodiscard]]
    const GValuePool& local_constant_pool() const noexcept {
        return m_local_constant_pool;
    }

private:
    const FunctionPrototype* m_prototype;
    GValuePool m_local_constant_pool;
};

static_assert(Function<FunctionData>, "assumed to be");