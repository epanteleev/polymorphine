#pragma once

#include <cstdint>
#include <memory>

#include "base/FunctionBind.h"
#include "lir/x64/instruction/LIRControlInstruction.h"


enum class LIRCallKind: std::uint8_t {
    VCall,
    Call,
    ICall,
    IVCall,
};

class LIRCall final: public LIRControlInstruction {
public:
    explicit LIRCall(std::string&& name, const LIRCallKind kind, std::vector<LIROperand>&& operands,
                       LIRBlock *cont, const FunctionBind bind) noexcept:
        LIRControlInstruction(std::move(operands), {cont}),
        m_name(std::move(name)),
        m_kind(kind),
        m_bind(bind) {}

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

    void add_user(LIRInstructionBase *inst) noexcept {
        m_used_in.push_back(inst);
    }

    void kill_user(LIRInstructionBase *inst) noexcept {
        std::erase(m_used_in, inst);
    }

    void assign_reg(const std::uint8_t idx, const OptionalGPVReg& reg) {
        assertion(idx < m_assigned_regs.size(), "Index out of bounds");
        m_assigned_regs[idx] = reg;
    }

    [[nodiscard]]
    const OptionalGPVReg& assigned_reg(const std::uint8_t idx) const {
        assertion(idx < m_assigned_regs.size(), "Index out of bounds");
        return m_assigned_regs.at(idx);
    }

    [[nodiscard]]
    std::span<LIRInstructionBase * const> users() const noexcept {
        return m_used_in;
    }

    static std::unique_ptr<LIRCall> call(std::string&& name, const std::uint8_t size, LIRBlock* cont, std::vector<LIROperand>&& args, FunctionBind bind) {
        auto call = std::make_unique<LIRCall>(std::move(name), LIRCallKind::Call, std::move(args), cont, bind);
        call->add_def(LIRVal::reg(size, size, 0, call.get()));
        return call;
    }

    static std::unique_ptr<LIRCall> vcall(std::string&& name, LIRBlock* cont, std::vector<LIROperand>&& args, FunctionBind bind) {
        return std::make_unique<LIRCall>(std::move(name), LIRCallKind::VCall, std::move(args), cont, bind);
    }

protected:
    void add_def(const LIRVal& def) {
        m_defs.push_back(def);
        m_assigned_regs.emplace_back();
    }

private:
    std::string m_name;
    std::vector<LIRVal> m_defs;
    std::vector<OptionalGPVReg> m_assigned_regs;
    const LIRCallKind m_kind;
    const FunctionBind m_bind;
    std::vector<LIRInstructionBase *> m_used_in;
};
