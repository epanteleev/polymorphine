#pragma once

#include <span>
#include <vector>
#include "lir/x64/lir_frwd.h"
#include "utility/Error.h"
#include "utility/StdExtensions.h"

class LIRUse {
public:
    void add_user(LIRInstructionBase *inst) noexcept {
        m_used_in.push_back(inst);
    }

    void kill_user(LIRInstructionBase *inst) noexcept {
        const auto it = std::ranges::find(m_used_in, inst);
        assertion(it != m_used_in.end(), "must contain");
        remove_fast(m_used_in, it);
    }

    [[nodiscard]]
    std::span<LIRInstructionBase * const> users() const noexcept {
        return m_used_in;
    }

private:
    std::vector<LIRInstructionBase *> m_used_in;
};

static_assert(!std::is_polymorphic_v<LIRUse>, "must be");