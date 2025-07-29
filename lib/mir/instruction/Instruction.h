#pragma once

#include <cstddef>
#include <vector>
#include <iosfwd>
#include <memory>
#include <span>

#include "InstructionVisitor.h"
#include "mir/value/Value.h"
#include "utility/Error.h"

class Instruction {
    static constexpr auto NO_ID = std::numeric_limits<std::size_t>::max();

public:
    explicit Instruction(std::vector<Value>&& values) noexcept:
        m_owner(nullptr),
        m_id(NO_ID),
        m_values(std::move(values)) {}

    virtual ~Instruction() = default;

    [[nodiscard]]
    std::size_t id() const {
        assertion(m_id != NO_ID, "Instruction ID is not set");
        return m_id;
    }

    [[nodiscard]]
    BasicBlock *owner() const {
        assertion(m_owner != nullptr, "Instruction owner is not set");
        return m_owner;
    }

    template<typename Matcher>
    bool isa(Matcher&& matcher) const noexcept {
        return matcher(this);
    }

    [[nodiscard]]
    std::span<const Value> operands() const {
        return m_values;
    }

    virtual void visit(Visitor& visitor) = 0;

    void print(std::ostream& os) const;

protected:
    friend class BasicBlock;

    void connect(const std::size_t id, BasicBlock* owner) {
        m_id = id;
        m_owner = owner;
    }

    BasicBlock* m_owner;
    std::size_t m_id;
    std::vector<Value> m_values;
};