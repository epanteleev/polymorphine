#pragma once
#include <memory>

#include "mir/module/FunctionData.h"
#include "mir/instruction/Alloc.h"
#include "mir/instruction/Binary.h"
#include "mir/instruction/Compare.h"
#include "mir/instruction/Store.h"
#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/Unary.h"


class FunctionBuilder final {
    explicit FunctionBuilder(std::unique_ptr<FunctionData> functionData);

public:
    static FunctionBuilder make(FunctionPrototype&& prototype, std::vector<ArgumentValue>&& args) noexcept;

    [[nodiscard]]
    const ArgumentValue* arg(const std::size_t index) const {
        return &m_fd->arg(index);
    }

    [[nodiscard]]
    Value alloc(const NonTrivialType* ty) const {
        return m_bb->push_back(Alloc::alloc(ty));
    }

    [[nodiscard]]
    Value load(const Value& pointer) const {
        return m_bb->push_back(Unary::load(pointer));
    }

    void store(const Value& pointer, const Value &value) const {
        m_bb->push_back(Store::store(pointer, value));
    }

    [[nodiscard]]
    Value add(const Value& lhs, const Value& rhs) const {
        return m_bb->push_back(Binary::add(lhs, rhs));
    }

    [[nodiscard]]
    Value icmp(IcmpPredicate predicate, const Value& lhs, const Value& rhs) const {
        return m_bb->push_back(IcmpInstruction::icmp(predicate, lhs, rhs));
    }

    [[nodiscard]]
    BasicBlock* create_basic_block() const {
        return m_fd->create_basic_block();
    }

    void switch_block(BasicBlock* bb) noexcept {
        m_bb = bb;
    }

    void br_cond(const Value& condition, BasicBlock *true_target, BasicBlock *false_target) const {
        m_bb->push_back(CondBranch::br_cond(condition, true_target, false_target));
    }

    void br(BasicBlock* target) const {
        m_bb->push_back(Branch::br(target));
    }

    void ret(const Value& ret_value) const {
        m_bb->push_back(ReturnValue::ret(ret_value));
    }

    [[nodiscard]]
    std::unique_ptr<FunctionData> build() noexcept {
        return std::move(m_fd);
    }

private:
    BasicBlock* m_bb;
    std::unique_ptr<FunctionData> m_fd;
};

