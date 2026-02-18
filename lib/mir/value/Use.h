#pragma once

#include <span>
#include "mir/mir_frwd.h"
#include "utility/StdExtensions.h"
#include "utility/Error.h"

class Use {
public:
    explicit Use(const Type *type) noexcept:
        m_type(type) {}

    void add_user(const Instruction *user) {
        m_users.push_back(user);
    }

    void remove_user(const Instruction *user) {
        const auto it = std::ranges::find(m_users, user);
        assertion(it != m_users.end(), "must contain");
        remove_fast(m_users, it);
    }

    [[nodiscard]]
    std::span<const Instruction* const> users() const noexcept {
        return m_users;
    }

    [[nodiscard]]
    const Type* type() const noexcept {
        return m_type;
    }

protected:
    const Type *m_type;
    std::vector<const Instruction*> m_users;
};
