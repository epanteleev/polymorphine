#pragma once

#include <cstddef>
#include <vector>

#include "lir/x64/lir_frwd.h"
#include "lir/x64/instruction/LIRInstructionBase.h"


class LIRControlInstruction: public LIRInstructionBase {
public:
    explicit LIRControlInstruction(const std::size_t id, MachBlock *bb, std::vector<LIRVal>&& defs, std::vector<LIROperand>&& uses, std::vector<MachBlock* >&& successors) :
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
        return [=](std::size_t idx, MachBlock *bb) {
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
    explicit LIRCall(const std::size_t id, MachBlock *bb, std::string&& name, const LIRCallKind kind, std::vector<LIRVal>&& defs, std::vector<LIROperand>&& operands,
                       MachBlock *on_true, MachBlock *on_false) :
        LIRControlInstruction(id, bb, std::move(defs), std::move(operands), {on_true, on_false}),
        m_name(std::move(name)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

private:
    std::string m_name;
    const LIRCallKind m_kind;
};