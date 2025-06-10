#pragma once

#include <cstddef>
#include <functional>
#include <vector>
#include <iosfwd>
#include <memory>

#include "../InstructionVisitor.h"
#include "../value/Value.h"
#include "../BasicBlock.h"

template <typename T>
using InstructionBuilder = std::function<std::unique_ptr<T>(std::size_t, BasicBlock*)>;

class Instruction {
public:
    virtual ~Instruction() = default;

    Instruction(const std::size_t id, BasicBlock *bb, const std::initializer_list<Value>& values)
        : m_owner(bb), m_id(id), m_values(values) {}

    [[nodiscard]]
    std::size_t id() const { return m_id; }

    [[nodiscard]]
    BasicBlock *owner() const { return m_owner; }

    virtual void visit(Visitor& visitor) = 0;

    void print(std::ostream& os) const;

protected:
    BasicBlock* m_owner;
    const std::size_t m_id;
    std::vector<Value> m_values;
};