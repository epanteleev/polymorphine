#pragma once

#include <vector>
#include "Constrains.h"
#include "FunctionDataBase.h"

template<CodeBlock BB>
class BlockMask final {
    explicit BlockMask(const std::size_t size) noexcept:
        m_mask(size, false) {}

public:
    [[nodiscard]]
    bool contains(const BB* bb) const noexcept {
        return m_mask[bb->id()];
    }

    bool emplace(const BB* bb) noexcept {
        const auto old = m_mask[bb->id()];
        m_mask[bb->id()] = true;
        return old;
    }

    template<typename Arg>
    [[nodiscard]]
    static BlockMask blockMask(const FunctionDataBase<BB, Arg>& data) noexcept {
        return BlockMask(data.max_possible_block_id());
    }

private:
    std::vector<bool> m_mask;
};