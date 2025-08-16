#pragma once

#include "LIRInstructionBase.h"
#include "ParallelCopy.h"


namespace x64::matchers {
    inline bool parallel_copy(const LIRInstructionBase* inst) {
        return dynamic_cast<const ParallelCopy*>(inst) != nullptr;
    }

    template<LIRProdInstKind kind>
    bool producer(const LIRInstructionBase* inst) {
        if (const auto producer = dynamic_cast<const LIRProducerInstruction*>(inst)) {
            return producer->op() == kind;
        }

        return false;
    }

    inline bool call(const LIRInstructionBase* inst) {
        return dynamic_cast<const LIRCall*>(inst) != nullptr;
    }
}

consteval auto parallel_copy() {
    return x64::matchers::parallel_copy;
}

consteval auto unary_copy() {
    return x64::matchers::producer<LIRProdInstKind::Copy>;
}

consteval auto call() {
    return x64::matchers::call;
}