#pragma once

#include "mir/module/FunctionPrototype.h"
#include "Instruction.h"
#include "mir/types/TupleType.h"


class TerminateInstruction: public Instruction {
public:
    TerminateInstruction(std::vector<Value>&& values, std::vector<BasicBlock *> &&successors) noexcept:
        Instruction(std::move(values)),
        m_successors(std::move(successors)) {}

    [[nodiscard]]
    std::span<BasicBlock * const> successors() const noexcept {
        return m_successors;
    }
    
protected:
    std::vector<BasicBlock* > m_successors;
};

class Return final: public TerminateInstruction {
public:
    Return() noexcept: TerminateInstruction({}, {}) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<Return> ret() {
        return std::make_unique<Return>();
    }
private:
    std::vector<BasicBlock* > m_successors;
};

class CondBranch final: public TerminateInstruction {
public:
    CondBranch(const Value& condition, BasicBlock *true_target, BasicBlock *false_target) noexcept:
        TerminateInstruction({condition}, {true_target, false_target}) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    BasicBlock *on_true() const {
        return m_successors.at(0);
    }

    [[nodiscard]]
    BasicBlock* on_false() const {
        return m_successors.at(1);
    }

    [[nodiscard]]
    const Value &condition() const {
        return m_values.at(0);
    }

    static std::unique_ptr<CondBranch> br_cond(const Value& condition, BasicBlock *true_target, BasicBlock *false_target) {
        return std::make_unique<CondBranch>(condition, true_target, false_target);
    }
};

class Branch final: public TerminateInstruction {
public:
    explicit Branch(BasicBlock* target) noexcept:
        TerminateInstruction({}, {target}) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    BasicBlock* target() const {
        return m_successors.front();
    }

    static std::unique_ptr<Branch> br(BasicBlock *target) {
        return std::make_unique<Branch>(target);
    }
};

class Switch final: public TerminateInstruction {
public:
    Switch(Value condition, std::vector<Value> &&cases, std::vector<BasicBlock*>&& targets) noexcept:
        TerminateInstruction({condition}, std::move(targets)),
        m_cases(cases) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<Switch> sw(const Value &condition, std::vector<Value> &&cases, BasicBlock* default_target, std::vector<BasicBlock*>&& targets) {
        targets.emplace_back(default_target);
        return std::make_unique<Switch>(condition, std::move(cases), std::move(targets));
    }

private:
    std::vector<Value> m_cases;
};

class ReturnValue final: public TerminateInstruction {
public:
    explicit ReturnValue(std::vector<Value>&& values) noexcept:
        TerminateInstruction(std::move(values), {}) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    const Value& first() const {
        return m_values.at(0);
    }

    [[nodiscard]]
    std::optional<Value> second() const {
        if (m_values.size() < 2) {
            return std::nullopt;
        }

        return m_values[1];
    }

    const Type* ret_type() const {
        switch (m_values.size()) {
            case 1: return m_values[0].type();
            case 2: {
                const auto first_type = dynamic_cast<const PrimitiveType*>(m_values[0].type());
                const auto second_type = dynamic_cast<const PrimitiveType*>(m_values[1].type());
                return TupleType::tuple(first_type, second_type);
            }
            default: die("Unsupported number of return values: {}", m_values.size());
        }
    }

    static std::unique_ptr<ReturnValue> ret(const Value& ret_value) {
        return std::make_unique<ReturnValue>(std::vector{ret_value});
    }

    static std::unique_ptr<ReturnValue> ret(const Value& first, const Value& second) {
        return std::make_unique<ReturnValue>(std::vector{first, second});
    }

private:
    std::vector<BasicBlock* > m_successors;
};

class VCall final: public TerminateInstruction {
public:
    VCall(const FunctionPrototype* prototype, std::vector<Value> &&args, BasicBlock * successor) noexcept:
        TerminateInstruction(std::move(args), {successor}), m_prototype(prototype) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<VCall> call(const FunctionPrototype* prototype, std::vector<Value> &&args, BasicBlock *successor) {
        return std::make_unique<VCall>(prototype, std::move(args), successor);
    }
private:
    const FunctionPrototype *m_prototype;
};

class IVCall final: public TerminateInstruction {
public:
    IVCall(const FunctionPrototype* prototype, std::vector<Value> &&args, BasicBlock * successor) noexcept:
        TerminateInstruction(std::move(args), {successor}),
        m_prototype(prototype) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static std::unique_ptr<IVCall> call(const FunctionPrototype* prototype, const Value& pointer, std::vector<Value> &&args, BasicBlock *successor) {
        args.push_back(pointer);
        return std::make_unique<IVCall>(prototype, std::move(args), successor);
    }
private:
    const FunctionPrototype *m_prototype;
};