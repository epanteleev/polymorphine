#pragma once

#include <span>
#include <vector>

#include "mir/mir_frwd.h"
#include "lir/x64/operand/LIROperand.h"

class ParallelCopyContext final {
public:
    explicit ParallelCopyContext(std::vector<std::size_t>&& indexes, std::vector<ValueInstruction*>&& values) noexcept:
        m_indexes(std::move(indexes)),
        m_values(std::move(values)) {}

    [[nodiscard]]
    std::span<const std::size_t> indexes() const noexcept {
        return m_indexes;
    }

    [[nodiscard]]
    std::span<ValueInstruction* const> values() const noexcept {
        return m_values;
    }

private:
    std::vector<std::size_t> m_indexes;
    std::vector<ValueInstruction*> m_values;
};