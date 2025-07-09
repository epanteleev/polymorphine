#pragma once

#include "LIRInstruction.h"
#include "LIRInstructionBase.h"


namespace x64::matchers {
    constexpr bool parallel_copy(const LIRInstructionBase* inst) {
        if (const auto value = dynamic_cast<const LIRInstruction*>(inst)) {
            return value->kind() == LIRInstKind::ParallelCopy;
        }

        return false;
    };
}

consteval auto parallel_copy() {
    return x64::matchers::parallel_copy;
}
