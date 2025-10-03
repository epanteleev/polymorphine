#pragma once

#include <memory>

#include "LIRDef.h"
#include "LIRUse.h"
#include "base/FunctionBind.h"
#include "lir/x64/instruction/LIRControlInstruction.h"


enum class LIRCallKind: std::uint8_t {
    VCall,
    Call,
    ICall,
    IVCall,
};

class LIRCall final: public LIRControlInstruction, public LIRUse, public LIRDef {
public:
    explicit LIRCall(std::string&& name, const LIRCallKind kind, std::vector<LIROperand>&& operands,
                       LIRBlock *cont, const FunctionBind bind) noexcept:
        LIRControlInstruction(std::move(operands), {cont}),
        LIRDef(LIRValType::GP),
        m_name(std::move(name)),
        m_kind(kind),
        m_bind(bind) {}

    void visit(LIRVisitor &visitor) override;

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_name;
    }

    static std::unique_ptr<LIRCall> call(std::string&& name, const std::uint8_t size, LIRBlock* cont, std::vector<LIROperand>&& args, FunctionBind bind) {
        auto call = std::make_unique<LIRCall>(std::move(name), LIRCallKind::Call, std::move(args), cont, bind);
        call->add_def(LIRVal::reg(size, size, 0, call.get()));
        return call;
    }

    static std::unique_ptr<LIRCall> vcall(std::string&& name, LIRBlock* cont, std::vector<LIROperand>&& args, FunctionBind bind) {
        return std::make_unique<LIRCall>(std::move(name), LIRCallKind::VCall, std::move(args), cont, bind);
    }

private:
    std::string m_name;
    const LIRCallKind m_kind;
    const FunctionBind m_bind;
};
