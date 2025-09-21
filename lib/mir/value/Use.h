#pragma once

#include <span>
#include <vector>
#include "mir/mir_frwd.h"

class Use {
public:
    explicit Use(const Type *type) noexcept:
        m_type(type) {}

    void add_user(const Instruction *user) {
        m_users.push_back(user);
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