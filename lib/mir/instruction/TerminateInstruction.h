#pragma once

#include "mir/module/FunctionPrototype.h"
#include "Instruction.h"


class TerminateInstruction: public Instruction {
public:
    TerminateInstruction(const std::size_t id, BasicBlock *bb, std::vector<Value>&& values,
                         std::vector<BasicBlock *> &&successors)
        : Instruction(id, bb, std::move(values)), m_successors(std::move(successors)) {}

    [[nodiscard]]
    std::span<BasicBlock * const> successors() const noexcept;
    
protected:
    std::vector<BasicBlock* > m_successors;
};

class Return final: public TerminateInstruction {
public:
    Return(const std::size_t id, BasicBlock *bb) : TerminateInstruction(id, bb, {}, {}) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static InstructionBuilder<Return> ret() {
        return [&] (std::size_t id, BasicBlock *bb) {
            return std::make_unique<Return>(id, bb);
        };
    }
private:
    std::vector<BasicBlock* > m_successors;
};

class CondBranch final: public TerminateInstruction {
public:
    CondBranch(const std::size_t id, BasicBlock *bb, Value condition,
                      BasicBlock *true_target, BasicBlock *false_target)
        : TerminateInstruction(id, bb, {condition}, {true_target, false_target}) {
    }

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

    static InstructionBuilder<CondBranch> br_cond(const Value& condition, BasicBlock *true_target, BasicBlock *false_target) {
        return [=] (std::size_t id, BasicBlock *bb) {
            return std::make_unique<CondBranch>(id, bb, condition, true_target, false_target);
        };
    }
};

class Branch final: public TerminateInstruction {
public:
    Branch(const std::size_t id, BasicBlock *bb, BasicBlock* target)
        : TerminateInstruction(id, bb, {}, {target}) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    BasicBlock* target() const {
        return m_successors.front();
    }

    static InstructionBuilder<Branch> br(BasicBlock *target) {
        return [=] (std::size_t id, BasicBlock *bb) {
            return std::make_unique<Branch>(id, bb, target);
        };
    }
};

class Switch final: public TerminateInstruction {
public:
    Switch(const std::size_t id, BasicBlock *bb, Value condition, std::vector<Value> &&cases, std::vector<BasicBlock*>&& targets)
        : TerminateInstruction(id, bb, {condition}, std::move(targets)),
          m_cases(cases) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static InstructionBuilder<Switch> sw(const Value &condition, std::vector<Value> &&cases, BasicBlock* default_target, std::vector<BasicBlock*>&& targets) {
        return [=, targets = std::move(targets), cases = std::move(cases)] (std::size_t id, BasicBlock *bb) {
            auto t = targets;
            t.emplace_back(default_target);
            return std::make_unique<Switch>(id, bb, condition, static_cast<std::vector<Value>>(cases), std::move(t));
        };
    }

private:
    std::vector<Value> m_cases;
};

class ReturnValue final: public TerminateInstruction {
public:
    ReturnValue(const std::size_t id, BasicBlock *bb, const Value& ret_value) :
        TerminateInstruction(id, bb, {ret_value}, {}) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    [[nodiscard]]
    const Value& ret_value() const {
        return m_values.at(0);
    }

    static InstructionBuilder<ReturnValue> ret(const Value& ret_value) {
        return [=] (std::size_t id, BasicBlock *bb) {
            return std::make_unique<ReturnValue>(id, bb, ret_value);
        };
    }
private:
    std::vector<BasicBlock* > m_successors;
};

class VCall final: public TerminateInstruction {
public:
    VCall(const std::size_t id, BasicBlock *bb, const FunctionPrototype* prototype,
          std::vector<Value> &&args, BasicBlock * successor)
        : TerminateInstruction(id, bb, std::move(args), {successor}), m_prototype(prototype) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static InstructionBuilder<VCall> call(const FunctionPrototype* prototype, std::vector<Value> &&args, BasicBlock *successor) {
        return [=, args = std::move(args)] (std::size_t id, BasicBlock *bb) {
            auto args1 = args;
            return std::make_unique<VCall>(id, bb, prototype, std::move(args1), successor);
        };
    }
private:
    const FunctionPrototype *m_prototype;
};

class IVCall final: public TerminateInstruction {
public:
    IVCall(const std::size_t id, BasicBlock *bb, const FunctionPrototype* prototype,
          std::vector<Value> &&args, BasicBlock * successor)
        : TerminateInstruction(id, bb, std::move(args), {successor}), m_prototype(prototype) {}

    void visit(Visitor &visitor) override { visitor.accept(this); }

    static InstructionBuilder<IVCall> call(const FunctionPrototype* prototype, const Value& pointer, std::vector<Value> &&args, BasicBlock *successor) {
        return [=, args = std::move(args)] (std::size_t id, BasicBlock *bb) {
            auto values = args;
            values.push_back(pointer);
            return std::make_unique<IVCall>(id, bb, prototype, std::move(values), successor);
        };
    }
private:
    const FunctionPrototype *m_prototype;
};