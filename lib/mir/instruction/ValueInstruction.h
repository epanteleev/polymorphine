#pragma once

#include "mir/instruction/Instruction.h"

class ValueInstruction : public Instruction {
public:
    ValueInstruction(const Type* ty, std::vector<Value>&& values) noexcept:
        Instruction(std::move(values)),
        m_ty(ty) {}

    [[nodiscard]]
    const Type* type() const { return m_ty; }

    void add_user(const Instruction* user);

    [[nodiscard]]
    std::span<const Instruction* const> users() const noexcept {
        return m_users;
    }

protected:
    const Type* m_ty;
    std::vector<const Instruction*> m_users;
};