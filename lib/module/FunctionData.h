#pragma once

#include <iosfwd>

#include "value/ArgumentValue.h"
#include "BasicBlock.h"
#include "utility/OrderedSet.h"
#include "FunctionPrototype.h"

class FunctionData final {
public:
    explicit FunctionData(std::size_t id, FunctionPrototype&& proto, std::vector<ArgumentValue>&& args);
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
    BasicBlock* last() const {
        return m_basic_blocks.end().get();
    }

    [[nodiscard]]
    const ArgumentValue *arg(const std::size_t index) const {
        return &m_args[index];
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return m_basic_blocks.size();
    }

    [[nodiscard]]
    std::size_t id() const noexcept {
        return m_id;
    }

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_prototype.name();
    }

private:
    std::size_t m_id;
    FunctionPrototype m_prototype;
    std::vector<ArgumentValue> m_args;
    OrderedSet<BasicBlock> m_basic_blocks;
};
