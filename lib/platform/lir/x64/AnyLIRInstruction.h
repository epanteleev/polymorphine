#pragma once

#include <iosfwd>

#include "LIROperand.h"
#include "LIRVisitor.h"
#include "platform/lir/x64/Chain.h"

#include "mach_frwd.h"

class AnyLIRInstruction {
public:
    AnyLIRInstruction(const std::size_t id, MachBlock *bb, std::span<LIROperand> uses,
                      const std::span<VReg> defs): m_id(id),
                                                         m_owner(bb),
                                                         m_du_chain(uses, defs) {}

    virtual ~AnyLIRInstruction() = default;

    auto inputs() noexcept {
        return m_du_chain.uses();
    }

    [[nodiscard]]
    auto outputs() const noexcept {
        return m_du_chain.defs();
    }

    [[nodiscard]]
    const VReg& out(const std::size_t idx) const {
        return m_du_chain.def(idx);
    }

    [[nodiscard]]
    const LIROperand& in(const std::size_t idx) const {
        return m_du_chain.use(idx);
    }

    virtual void visit(LIRVisitor& visitor) = 0;

    void print(std::ostream &os) const;

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
    LIRInstruction(std::size_t id, MachBlock *bb, const LIRInstKind kind, std::span<LIROperand> uses, std::span<VReg> defs) :
        AnyLIRInstruction(id, bb, uses, defs),
        m_kind(kind) {}
public:
    void visit(LIRVisitor &visitor) override;

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

    [[nodiscard]]
    std::span<MachBlock * const> successors() const noexcept {
        return m_successors;
    }


    void visit(LIRVisitor &visitor) override;

private:
    LIRControlKind m_kind;
    std::vector<MachBlock* > m_successors;
};