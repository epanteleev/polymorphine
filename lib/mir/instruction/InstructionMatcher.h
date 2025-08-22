#pragma once

#include "TerminateInstruction.h"
#include "TerminateValueInstruction.h"
#include "Unary.h"

namespace impl {
    inline bool any_return(const Instruction* inst) noexcept {
        if (const auto ret = dynamic_cast<const Return*>(inst); ret != nullptr) {
            return true;
        }

        const auto ret_val = dynamic_cast<const ReturnValue*>(inst);
        return ret_val != nullptr;
    }

    inline bool load(const Instruction* inst) noexcept {
        if (const auto unary = dynamic_cast<const Unary*>(inst); unary != nullptr) {
            return unary->op() == UnaryOp::Load;
        }

        return false;
    }

    inline bool any_terminate(const Instruction* inst) noexcept {
        if (const auto term = dynamic_cast<const TerminateInstruction*>(inst); term != nullptr) {
            return true;
        }

        if (const auto term_val = dynamic_cast<const TerminateValueInstruction*>(inst); term_val != nullptr) {
            return true;
        }

        return false;
    }
}

consteval auto any_return() {
    return impl::any_return;
}

consteval auto any_terminate() {
    return impl::any_terminate;
}

consteval auto load() {
    return impl::load;
}