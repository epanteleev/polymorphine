#pragma once

#include <cstddef>
#include <vector>

#include "lir/x64/lir_frwd.h"
#include "lir/x64/instruction/LIRInstructionBase.h"


class LIRControlInstruction: public LIRInstructionBase {
public:
    explicit LIRControlInstruction(const std::size_t id, LIRBlock *bb, std::vector<LIROperand>&& uses, std::vector<LIRBlock* >&& successors) :
        LIRInstructionBase(id, bb, std::move(uses)),
        m_successors(std::move(successors)) {}

    [[nodiscard]]
    std::span<LIRBlock * const> successors() const noexcept {
        return m_successors;
    }

    [[nodiscard]]
    const LIRBlock* succ(const std::size_t idx) const {
        return m_successors.at(idx);
    }

private:
    std::vector<LIRBlock* > m_successors;
};

class LIRBranch final: public LIRControlInstruction {
public:
    explicit LIRBranch(const std::size_t id, LIRBlock *bb, std::vector<LIROperand>&& uses,
                       std::vector<LIRBlock* >&& successors) :
        LIRControlInstruction(id, bb, std::move(uses), std::move(successors)) {}

    void visit(LIRVisitor &visitor) override;

    static LIRInstBuilder<LIRBranch> jmp(LIRBlock *target) {
        return [=](std::size_t idx, LIRBlock *bb) {
            return std::make_unique<LIRBranch>(idx, bb, std::vector<LIROperand>{}, std::vector{target});
        };
    }
};

class LIRCondBranch final: public LIRControlInstruction {
public:
    explicit LIRCondBranch(const std::size_t id, LIRBlock *bb, const LIRCondType kind, std::vector<LIROperand>&& uses,
                       std::vector<LIRBlock* >&& successors) :
        LIRControlInstruction(id, bb, std::move(uses), std::move(successors)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

    static LIRInstBuilder<LIRCondBranch> jcc(const LIRCondType kind, LIRBlock *on_true, LIRBlock *on_false) {
        return [=](std::size_t idx, LIRBlock *bb) {
            return std::make_unique<LIRCondBranch>(idx, bb, kind, std::vector<LIROperand>{}, std::vector{on_true, on_false});
        };
    }

private:
    const LIRCondType m_kind;
};

class LIRReturn final: public LIRControlInstruction {
public:
    explicit LIRReturn(const std::size_t id, LIRBlock *bb, std::vector<LIROperand>&& values) :
        LIRControlInstruction(id, bb, std::move(values), {}) {}

    void visit(LIRVisitor &visitor) override;

    static LIRInstBuilder<LIRReturn> ret(const LIROperand& value) {
        return [=](std::size_t idx, LIRBlock *bb) {
            return std::make_unique<LIRReturn>(idx, bb, std::vector{value});
        };
    }

    static LIRInstBuilder<LIRReturn> ret() {
        return [=](std::size_t idx, LIRBlock *bb) {
            return std::make_unique<LIRReturn>(idx, bb, std::vector<LIROperand>{});
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
    explicit LIRCall(const std::size_t id, LIRBlock *bb, std::string&& name, const LIRCallKind kind, std::vector<LIROperand>&& operands,
                       LIRBlock *on_true, LIRBlock *on_false) :
        LIRControlInstruction(id, bb, std::move(operands), {on_true, on_false}),
        m_name(std::move(name)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

private:
    std::string m_name;
    const LIRCallKind m_kind;
};