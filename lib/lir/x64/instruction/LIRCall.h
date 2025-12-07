#pragma once

#include <memory>

#include "LIRDef.h"
#include "LIRUse.h"
#include "base/FunctionBind.h"
#include "lir/x64/instruction/LIRControlInstruction.h"


enum class LIRCallKind: std::uint8_t {
    VCall,
    Call,
    TupleCall,
    ICall,
    IVCall,
};

class LIRCall final: public LIRControlInstruction, public LIRUse, public LIRDef {
public:
    explicit LIRCall(std::string&& name, const InplaceVec<LIRValType, 2>& ty, const LIRCallKind kind, std::vector<LIROperand>&& operands,
                       LIRBlock *cont, const FunctionBind bind) noexcept:
        LIRControlInstruction(std::move(operands), {cont}),
        LIRDef(ty),
        m_name(std::move(name)),
        m_kind(kind),
        m_bind(bind) {}

    void visit(LIRVisitor &visitor) override;

    [[nodiscard]]
    std::string_view name() const noexcept {
        return m_name;
    }

    static std::unique_ptr<LIRCall> call(std::string&& name, const LIRValType ty, const std::uint8_t size, LIRBlock* cont, std::vector<LIROperand>&& args, FunctionBind bind) {
        InplaceVec<LIRValType, 2> types{ty};
        auto call = std::make_unique<LIRCall>(std::move(name), types, LIRCallKind::Call, std::move(args), cont, bind);
        call->add_def(LIRVal::reg(size, size, 0, call.get()));
        return call;
    }

    static std::unique_ptr<LIRCall> tuple_call(std::string&& name, const LIRValType ty1, const LIRValType ty2, const std::size_t size1, const std::size_t size2, LIRBlock* cont, std::vector<LIROperand>&& args, FunctionBind bind) {
        InplaceVec<LIRValType, 2> types{ty1, ty2};
        auto call = std::make_unique<LIRCall>(std::move(name), types, LIRCallKind::Call, std::move(args), cont, bind);
        call->add_def(LIRVal::reg(size1, size2, 0, call.get()));
        call->add_def(LIRVal::reg(size1, size2, 1, call.get()));
        return call;
    }

    static std::unique_ptr<LIRCall> vcall(std::string&& name, LIRBlock* cont, std::vector<LIROperand>&& args, FunctionBind bind) {
        const InplaceVec<LIRValType, 2> types{LIRValType::GP};
        return std::make_unique<LIRCall>(std::move(name), types, LIRCallKind::VCall, std::move(args), cont, bind);
    }

private:
    std::string m_name;
    const LIRCallKind m_kind;
    const FunctionBind m_bind;
};
