#pragma once

#include "ValueInstruction.h"
#include "mir/module/FunctionPrototype.h"
#include "mir/types/TypeMatcher.h"


class TerminateValueInstruction : public ValueInstruction {
public:
    TerminateValueInstruction(const Type *ty, BasicBlock *cont, std::vector<Value>&& values) noexcept:
        ValueInstruction(ty, std::move(values)),
        m_cont(cont) {}

    [[nodiscard]]
    const BasicBlock * cont() const noexcept {
        return m_cont;
    }

    [[nodiscard]]
    std::span<BasicBlock * const> successors() const noexcept {
        return {&m_cont, 1};
    }

private:
    BasicBlock* m_cont;
};

class Call final : public TerminateValueInstruction {
public:
    Call(FunctionPrototype&& proto, BasicBlock* successor, std::vector<Value>&& args) noexcept:
        TerminateValueInstruction(proto.ret_type(), successor, std::move(args)),
        m_prototype(std::move(proto)) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    const FunctionPrototype& prototype() const noexcept {
        return m_prototype;
    }

    static std::unique_ptr<Call> call(FunctionPrototype&& proto, BasicBlock* cont, std::vector<Value>&& args) {
        assertion(!proto.ret_type()->isa(void_type()), "Call instruction must have a non-void return type");
        return std::make_unique<Call>(std::move(proto), cont, std::move(args));
    }

private:
    const FunctionPrototype m_prototype;
};