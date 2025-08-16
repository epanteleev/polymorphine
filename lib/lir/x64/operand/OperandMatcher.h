#pragma once

#include "lir/x64/operand/LIRVal.h"
#include "lir/x64/instruction/LIRProducerInstruction.h"

namespace impl {
    template<LIRProdInstKind K>
    bool is_producer(const LIRVal& lhs) noexcept {
        const auto producer = lhs.inst();
        if (!producer.has_value()) {
            return false;
        }

        const auto inst = producer.value();
        if (const auto prod_inst = dynamic_cast<const LIRProducerInstruction*>(inst)) {
            return prod_inst->op() == K;
        }

        return false;
    }

    inline bool parallel_copy(const LIRVal& lhs) noexcept {
        const auto producer = lhs.inst();
        if (!producer.has_value()) {
            return false;
        }

        return dynamic_cast<const ParallelCopy*>(producer.value()) != nullptr;
    }
}

consteval auto gen() {
    return impl::is_producer<LIRProdInstKind::Gen>;
}

consteval auto copy_v() {
    return impl::is_producer<LIRProdInstKind::Copy>;
}

consteval auto parallel_copy_v() {
    return impl::parallel_copy;
}