#pragma once

#include <cstddef>
#include <functional>
#include <vector>
#include <iosfwd>
#include <memory>
#include <span>

#include "InstructionVisitor.h"
#include "mir/value/Value.h"

template <typename T>
using InstructionBuilder = std::function<std::unique_ptr<T>(std::size_t, BasicBlock*)>;

class Instruction {
public:
    virtual ~Instruction() = default;

    Instruction(const std::size_t id, BasicBlock *bb, std::vector<Value>&& values)
        : m_owner(bb),
        m_id(id),
        m_values(std::move(values)) {}

    [[nodiscard]]
    std::size_t id() const { return m_id; }

    [[nodiscard]]
    BasicBlock *owner() const { return m_owner; }

    [[nodiscard]]
    std::span<const Value> operands() const {
        return m_values;
    }

    virtual void visit(Visitor& visitor) = 0;

    void print(std::ostream& os) const;

protected:
    BasicBlock* m_owner;
    const std::size_t m_id;
    std::vector<Value> m_values;
};