#pragma once
#include <memory>

#include "FunctionData.h"
#include "instruction/Alloc.h"
#include "instruction/Binary.h"
#include "instruction/Store.hpp"
#include "instruction/Unary.h"

class FunctionBuilder final {
    explicit FunctionBuilder(std::unique_ptr<FunctionData> functionData);

public:
    static FunctionBuilder make(std::size_t id, FunctionPrototype&& prototype, std::vector<ArgumentValue>&& args) noexcept;

    [[nodiscard]]
    ArgumentValue* arg(std::size_t index) const {
        return m_fd->arg(index);
    }

    [[nodiscard]]
    Value alloc(const NonTrivialType* ty) const {
        return m_bb->push_back(Alloc::alloc(ty));
    }

    [[nodiscard]]
    Value load(const Value& pointer) const {
        return m_bb->push_back(Unary::load(pointer));
    }

    void store(const Value& pointer, Value value) const {
        m_bb->push_back(Store::store(pointer, value));
    }

    [[nodiscard]]
    Value add(const Value& lhs, const Value& rhs) const {
        return m_bb->push_back(Binary::add(lhs, rhs));
    }

    std::unique_ptr<FunctionData> build() noexcept {
        return std::move(m_fd);
    }

private:
    BasicBlock* m_bb;
    std::unique_ptr<FunctionData> m_fd;
};

