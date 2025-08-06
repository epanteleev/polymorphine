#pragma once

#include <cstddef>
#include <vector>

#include "lir/x64/lir_frwd.h"
#include "lir/x64/instruction/LIRInstructionBase.h"


class LIRControlInstruction: public LIRInstructionBase {
public:
    explicit LIRControlInstruction(std::vector<LIROperand>&& uses, std::vector<LIRBlock* >&& successors) :
        LIRInstructionBase(std::move(uses)),
        m_successors(std::move(successors)) {}

    [[nodiscard]]
    std::span<LIRBlock * const> successors() const noexcept {
        return m_successors;
    }

    [[nodiscard]]
    const LIRBlock* succ(const std::size_t idx) const {
        return m_successors.at(idx);
    }

protected:
    std::vector<LIRBlock* > m_successors;
};

class LIRBranch final: public LIRControlInstruction {
public:
    explicit LIRBranch(std::vector<LIROperand>&& uses,
                       std::vector<LIRBlock* >&& successors) :
        LIRControlInstruction(std::move(uses), std::move(successors)) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRBranch> jmp(LIRBlock *target) {
        return std::make_unique<LIRBranch>(std::vector<LIROperand>{}, std::vector{target});
    }
};

class LIRCondBranch final: public LIRControlInstruction {
public:
    explicit LIRCondBranch(const LIRCondType kind, std::vector<LIROperand>&& uses,
                       std::vector<LIRBlock* >&& successors) :
        LIRControlInstruction(std::move(uses), std::move(successors)),
        m_kind(kind) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRCondBranch> jcc(const LIRCondType kind, LIRBlock *on_true, LIRBlock *on_false) {
        return std::make_unique<LIRCondBranch>(kind, std::vector<LIROperand>{}, std::vector{on_true, on_false});
    }

private:
    const LIRCondType m_kind;
};

class LIRReturn final: public LIRControlInstruction {
public:
    explicit LIRReturn(std::vector<LIROperand>&& values) :
        LIRControlInstruction(std::move(values), {}) {}

    void visit(LIRVisitor &visitor) override;

    static std::unique_ptr<LIRReturn> ret(const LIROperand& value) {
        return std::make_unique<LIRReturn>(std::vector{value});
    }

    static std::unique_ptr<LIRReturn> ret() {
        return std::make_unique<LIRReturn>(std::vector<LIROperand>{});
    }
};

enum class LIRCallKind: std::uint8_t {
    VCall,
    Call,
    ICall,
    IVCall,
};

enum class LIRLinkage: std::uint8_t {
    EXTERNAL,
    INTERNAL,
};

class LIRCall final: public LIRControlInstruction {
public:
    explicit LIRCall(std::string&& name, const LIRCallKind kind, std::vector<LIROperand>&& operands,
                       LIRBlock *cont, LIRLinkage linkage) noexcept:
        LIRControlInstruction(std::move(operands), {cont}),
        m_name(std::move(name)),
        m_kind(kind),
        m_linkage(linkage) {}

    void visit(LIRVisitor &visitor) override;

    [[nodiscard]]
    std::span<LIRVal const> defs() const noexcept {
        return m_defs;
    }

    [[nodiscard]]
    const LIRVal& def(const std::size_t idx) const {
        return m_defs.at(idx);
    }

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_name;
    }

    static std::unique_ptr<LIRCall> call(std::string&& name, std::uint8_t size, LIRBlock* cont, std::vector<LIROperand>&& args, LIRLinkage linkage) {
        auto call = std::make_unique<LIRCall>(std::move(name), LIRCallKind::Call, std::move(args), cont, linkage);
        call->add_def(LIRVal::reg(size, 0, call.get()));
        return call;
    }

protected:
    void add_def(const LIRVal& def) {
        m_defs.push_back(def);
    }

private:
    std::string m_name;
    std::vector<LIRVal> m_defs;
    const LIRCallKind m_kind;
    const LIRLinkage m_linkage;
};