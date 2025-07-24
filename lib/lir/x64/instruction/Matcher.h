#pragma once

#include "LIRProducerInstruction.h"
#include "LIRInstructionBase.h"


namespace x64::matchers {
    constexpr bool parallel_copy(const LIRInstructionBase* inst) {
        if (const auto value = dynamic_cast<const LIRProducerInstruction*>(inst)) {
            return value->op() == LIRProdInstKind::ParallelCopy;
        }

        return false;
    };
}

consteval auto parallel_copy() {
    return x64::matchers::parallel_copy;
}
