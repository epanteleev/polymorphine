#pragma once

#include "TerminateInstruction.h"
#include "TerminateValueInstruction.h"

namespace impl {
    inline bool any_return(const Instruction* inst) noexcept {
        if (const auto ret = dynamic_cast<const Return*>(inst); ret != nullptr) {
            return true;
        }

        const auto ret_val = dynamic_cast<const ReturnValue*>(inst);
        return ret_val != nullptr;
    }
}

consteval auto any_return() {
    return impl::any_return;
}