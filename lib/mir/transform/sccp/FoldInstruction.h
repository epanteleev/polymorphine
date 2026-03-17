#pragma once

#include <optional>

#include "mir/value/VConstant.h"

class FoldInstruction final {
public:
    [[nodiscard]]
    static std::optional<VConstant> try_fold(const Instruction &inst) noexcept;
};