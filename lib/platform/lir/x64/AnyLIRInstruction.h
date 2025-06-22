#pragma once

#include "LIROperand.h"
#include "platform/lir/x64/Chain.h"

#include "mach_frwd.h"

class AnyLIRInstruction {
public:
    AnyLIRInstruction(std::size_t id, MachBlock *bb, std::span<LIROperand> uses,
                      std::span<LIRInstruction *> defs): m_id(id),
                                                         m_owner(bb),
                                                         m_du_chain(uses, defs) {}

    virtual ~AnyLIRInstruction() = default;

    auto outputs() noexcept {
        return m_du_chain.uses();
    }

protected:
    std::size_t m_id;
    MachBlock* m_owner;
    Chain m_du_chain;
};


enum class LIRInstKind: std::uint8_t {
    Add,
    Sub,
    Mul,
    Div,
    And,
    Or,
    Xor,
    Shl,
    Shr,
    Neg,
    Not,
    Mov,
    Call,
    Cmp,
};

class LIRInstruction final: public AnyLIRInstruction {
    LIRInstruction(std::size_t id, MachBlock *bb, const LIRInstKind kind, std::span<LIROperand> uses, std::span<LIRInstruction*> defs) :
        AnyLIRInstruction(id, bb, uses, defs),
        m_kind(kind) {}

public:


private:
    LIRInstKind m_kind;
};

enum class LIRControlKind: std::uint8_t {
    Jmp,
    Jz,
    Jnz,
    Je,
    Jne,
    Jg,
    Jge,
    Jl,
    Jle,
    Ret
};

class LIRControlInstruction final: public AnyLIRInstruction {
public:
    explicit LIRControlInstruction(const std::size_t id, MachBlock *bb, const LIRControlKind kind, const std::span<LIROperand> uses) :
        AnyLIRInstruction(id, bb, uses, {}),
        m_kind(kind) {}

private:
    LIRControlKind m_kind;
};