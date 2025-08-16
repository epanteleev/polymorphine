#pragma once

#include "LIRInstructionBase.h"
#include "ParallelCopy.h"


namespace x64::matchers {
    constexpr bool parallel_copy(const LIRInstructionBase* inst) {
        return dynamic_cast<const ParallelCopy*>(inst) != nullptr;
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