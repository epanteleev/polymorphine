#pragma once
#include <memory>

#include "mir/module/FunctionData.h"
#include "mir/instruction/Alloc.h"
#include "mir/instruction/Binary.h"
#include "mir/instruction/Compare.h"
#include "mir/instruction/GetElementPtr.h"
#include "mir/instruction/GetFieldPtr.h"
#include "mir/instruction/Phi.h"
#include "mir/instruction/Select.h"
#include "mir/instruction/Store.h"
#include "mir/instruction/TerminateInstruction.h"
#include "mir/instruction/TerminateValueInstruction.h"
#include "mir/instruction/Unary.h"


class FunctionBuilder final {
    explicit FunctionBuilder(std::unique_ptr<FunctionData>&& functionData);

public:
    static FunctionBuilder make(FunctionPrototype&& prototype, std::vector<ArgumentValue>&& args) noexcept;

    [[nodiscard]]
    const ArgumentValue* arg(const std::size_t index) const {
        return &m_fd->arg(index);
    }

    [[nodiscard]]
    Value alloc(const NonTrivialType* ty) const {
        return m_bb->ins(Alloc::alloc(ty));
    }

    [[nodiscard]]
    Value load(const PrimitiveType* loaded_type, const Value& pointer) const {
        return m_bb->ins(Unary::load(loaded_type, pointer));
    }

    void store(const Value& pointer, const Value &value) const {
        m_bb->ins(Store::store(pointer, value));
    }

    [[nodiscard]]
    Value add(const Value& lhs, const Value& rhs) const {
        return m_bb->ins(Binary::add(lhs, rhs));
    }

    [[nodiscard]]
    Value sub(const Value& lhs, const Value& rhs) const {
        return m_bb->ins(Binary::sub(lhs, rhs));
    }

    [[nodiscard]]
    Value xxor(const Value& lhs, const Value& rhs) const {
        return m_bb->ins(Binary::xxor(lhs, rhs));
    }

    [[nodiscard]]
    Value icmp(const IcmpPredicate predicate, const Value& lhs, const Value& rhs) const {
        return m_bb->ins(IcmpInstruction::icmp(predicate, lhs, rhs));
    }

    [[nodiscard]]
    Value flag2int(const IntegerType* to_type, const Value& flag) const {
        return m_bb->ins(Unary::flag2int(to_type, flag));
    }

    [[nodiscard]]
    Value select(const Value& cond, const Value& lhs, const Value& rhs) const {
        return m_bb->ins(Select::select(cond, lhs, rhs));
    }

    [[nodiscard]]
    Value phi(const PrimitiveType* type, std::vector<Value>&& values, std::vector<BasicBlock*>&& targets) const {
        return m_bb->ins(Phi::phi(type, std::move(values), std::move(targets)));
    }

    [[nodiscard]]
    Value call(FunctionPrototype&& prototype, BasicBlock* cont, std::vector<Value>&& args) const {
        return m_bb->ins(Call::call(std::move(prototype), cont, std::move(args)));
    }

    [[nodiscard]]
    Value gep(const NonTrivialType* ty, const Value& pointer, const Value& index) const {
        return m_bb->ins(GetElementPtr::gep(ty, pointer, index));
    }

    [[nodiscard]]
    Value gfp(const StructType* ty, const Value& pointer, const std::size_t index) const {
        return m_bb->ins(GetFieldPtr::gfp(ty, pointer, index));
    }

    [[nodiscard]]
    Value sext(const IntegerType* to_type, const Value& value) const {
        return m_bb->ins(Unary::sext(to_type, value));
    }

    [[nodiscard]]
    Value zext(const IntegerType* to_type, const Value& value) const {
        return m_bb->ins(Unary::zext(to_type, value));
    }

    [[nodiscard]]
    Value trunc(const IntegerType* to_type, const Value& value) const {
        return m_bb->ins(Unary::trunc(to_type, value));
    }

    [[nodiscard]]
    Value bitcast(const IntegerType* to_type, const Value& value) const {
        return m_bb->ins(Unary::bitcast(to_type, value));
    }

    [[nodiscard]]
    BasicBlock* create_basic_block() const {
        return m_fd->create_basic_block();
    }

    void switch_block(BasicBlock* bb) noexcept {
        m_bb = bb;
    }

    void br_cond(const Value& condition, BasicBlock *true_target, BasicBlock *false_target) const {
        m_bb->ins(CondBranch::br_cond(condition, true_target, false_target));
    }

    void br(BasicBlock* target) const {
        m_bb->ins(Branch::br(target));
    }

    void ret(const Value& ret_value) const {
        m_bb->ins(ReturnValue::ret(ret_value));
    }

    void ret() const {
        m_bb->ins(Return::ret());
    }

    [[nodiscard]]
    std::unique_ptr<FunctionData> build() noexcept;

private:
    BasicBlock* m_bb;
    std::unique_ptr<FunctionData> m_fd;
};

