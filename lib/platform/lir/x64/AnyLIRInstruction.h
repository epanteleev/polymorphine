#pragma once

#include <iosfwd>

#include "LIROperand.h"
#include "LIRVisitor.h"
#include "platform/lir/x64/Chain.h"

#include "mach_frwd.h"

class AnyLIRInstruction {
public:
    AnyLIRInstruction(const std::size_t id, MachBlock *bb, std::vector<LIROperand>&& uses,
                      std::vector<VReg>&& defs): m_id(id),
                                                         m_owner(bb),
                                                         m_du_chain(std::move(uses), std::move(defs)) {}

    virtual ~AnyLIRInstruction() = default;

    std::span<LIROperand const> inputs() noexcept {
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
    Cmp,
};

class LIRInstruction final: public AnyLIRInstruction {
    LIRInstruction(std::size_t id, MachBlock *bb, const LIRInstKind kind, std::vector<LIROperand>&& uses, std::vector<VReg>&& defs) :
        AnyLIRInstruction(id, bb, std::move(uses), std::move(defs)),
        m_kind(kind) {}
public:
    void visit(LIRVisitor &visitor) override;

private:
    LIRInstKind m_kind;
};

class LIRControlInstruction: public AnyLIRInstruction {
public:
    explicit LIRControlInstruction(const std::size_t id, MachBlock *bb, std::vector<VReg>&& defs, std::vector<LIROperand>&& uses, std::vector<MachBlock* >&& successors) :
        AnyLIRInstruction(id, bb, std::move(uses), std::move(defs)),
        m_successors(std::move(successors)) {}

    [[nodiscard]]
    std::span<MachBlock * const> successors() const noexcept {
        return m_successors;
    }

    [[nodiscard]]
    const MachBlock* succ(const std::size_t idx) const {
        return m_successors.at(idx);
    }

private:
    std::vector<MachBlock* > m_successors;
};

enum class LIRBranchKind: std::uint8_t {
    Jmp,
    Je,
    Jne,
    Jg,
    Jge,
    Jl,
    Jle,
    Ret
};

class LIRBranch final: public LIRControlInstruction {
public:
    explicit LIRBranch(const std::size_t id, MachBlock *bb, const LIRBranchKind kind, const LIROperand& condition,
                       MachBlock *on_true, MachBlock *on_false) :
        LIRControlInstruction(id, bb, {}, {condition}, {on_true, on_false}),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

private:
    const LIRBranchKind m_kind;
};

enum class LIRCallKind: std::uint8_t {
    VCall,
    Call,
    ICall,
    IVCall,
};

class LIRCall final: public LIRControlInstruction {
public:
    explicit LIRCall(const std::size_t id, MachBlock *bb, std::string&& name, const LIRCallKind kind, std::vector<VReg>&& defs, std::vector<LIROperand>&& operands,
                       MachBlock *on_true, MachBlock *on_false) :
        LIRControlInstruction(id, bb, std::move(defs), std::move(operands), {on_true, on_false}),
        m_name(std::move(name)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

private:
    std::string m_name;
    const LIRCallKind m_kind;
};