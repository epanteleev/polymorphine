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

    constexpr bool call(const LIRInstructionBase* inst) {
        return dynamic_cast<const LIRCall*>(inst) != nullptr;
    }
}

consteval auto parallel_copy() {
    return x64::matchers::parallel_copy;
}

consteval auto call() {
    return x64::matchers::call;
}