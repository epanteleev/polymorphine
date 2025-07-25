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
}


consteval auto cmp() {
    return impl::is_producer<LIRProdInstKind::Cmp>;
}

consteval auto gen() {
    return impl::is_producer<LIRProdInstKind::Gen>;
}