#pragma once

#include "lir/x64/operand/LIRVal.h"
#include "lir/x64/instruction/LIRProducerInstruction.h"

namespace impl {
    inline bool cmp(const LIRVal& lhs) noexcept {
        const auto producer = lhs.inst();
        if (!producer.has_value()) {
            return false;
        }

        const auto inst = producer.value();
        if (const auto cmp_inst = dynamic_cast<const LIRProducerInstruction*>(inst)) {
            return cmp_inst->op() == LIRProdInstKind::Cmp;
        }

        return false;
    }
}


consteval auto cmp() {
    return impl::cmp;
}