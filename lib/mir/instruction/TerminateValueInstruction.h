#pragma once

#include "ValueInstruction.h"
#include "mir/module/FunctionPrototype.h"


class TerminateValueInstruction : public ValueInstruction {
public:
    TerminateValueInstruction(const Type *ty, std::vector<BasicBlock *> &&successors, std::vector<Value>&& values) noexcept:
        ValueInstruction(ty, std::move(values)),
        m_successors(std::move(successors)) {}

    [[nodiscard]]
    std::span<BasicBlock *const> successors();

private:
    std::vector<BasicBlock* > m_successors;
};

class Call final : public TerminateValueInstruction {
public:
    Call(FunctionPrototype&& proto, BasicBlock* successor, std::vector<Value>&& args) noexcept:
        TerminateValueInstruction(proto.ret_type(), {successor}, std::move(args)),
        m_prototype(std::move(proto)) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    const FunctionPrototype& prototype() const noexcept {
        return m_prototype;
    }

    static std::unique_ptr<Call> call(FunctionPrototype&& proto, BasicBlock* cont, std::vector<Value>&& args) {
        return std::make_unique<Call>(std::move(proto), cont, std::move(args));
    }

private:
    const FunctionPrototype m_prototype;
};