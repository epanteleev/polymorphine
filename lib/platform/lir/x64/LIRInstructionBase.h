#pragma once

#include <functional>
#include <iosfwd>
#include <memory>

#include "LIROperand.h"
#include "LIRVisitor.h"
#include "platform/lir/x64/Chain.h"
#include "platform/lir/x64/lower/VregBuilder.hpp"

class LIRInstructionBase {
public:
    LIRInstructionBase(const std::size_t id, MachBlock *bb, std::vector<LIROperand>&& uses,
                      std::vector<VReg>&& defs): m_id(id),
                                                         m_owner(bb),
                                                         m_du_chain(std::move(uses), std::move(defs)) {}

    virtual ~LIRInstructionBase() = default;

    [[nodiscard]]
    std::span<LIROperand const> inputs() const noexcept {
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

    [[nodiscard]]
    const MachBlock* owner() const noexcept {
        return m_owner;
    }


    template<typename Fn>
    [[nodiscard]]
    bool isa(const Fn& matcher) const noexcept {
        return matcher(this);
    }

protected:
    void add_def(const VReg& def) {
        m_du_chain.add_def(def);
    }

    std::size_t m_id;
    MachBlock* m_owner;
    Chain m_du_chain;
};

template<typename T>
using LIRInstBuilder = std::function<std::unique_ptr<T>(std::size_t, MachBlock*, VregBuilder&)>;

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
    ParallelCopy,
    Mov,
    Copy,
    Cmp,
};

class LIRInstruction final: public LIRInstructionBase {
public:
    LIRInstruction(const std::size_t id, MachBlock *bb, const LIRInstKind kind, std::vector<LIROperand>&& uses, std::vector<VReg>&& defs) :
        LIRInstructionBase(id, bb, std::move(uses), std::move(defs)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

    static LIRInstBuilder<LIRInstruction> copy(const LIROperand& op);

    [[nodiscard]]
    LIRInstKind kind() const noexcept {
        return m_kind;
    }

private:
    const LIRInstKind m_kind;
};

class LIRControlInstruction: public LIRInstructionBase {
public:
    explicit LIRControlInstruction(const std::size_t id, MachBlock *bb, std::vector<VReg>&& defs, std::vector<LIROperand>&& uses, std::vector<MachBlock* >&& successors) :
        LIRInstructionBase(id, bb, std::move(uses), std::move(defs)),
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

class LIRReturn final: public LIRControlInstruction {
public:
    explicit LIRReturn(const std::size_t id, MachBlock *bb, std::vector<LIROperand>&& values) :
        LIRControlInstruction(id, bb, {}, std::move(values), {}) {}

    void visit(LIRVisitor &visitor) override;

    static LIRInstBuilder<LIRReturn> ret(const LIROperand& value) {
        return [=](std::size_t idx, MachBlock *bb, VregBuilder&) {
            return std::make_unique<LIRReturn>(idx, bb, std::vector{value});
        };
    }
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