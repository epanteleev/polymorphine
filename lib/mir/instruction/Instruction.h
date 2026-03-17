#pragma once

#include <cstddef>
#include <vector>
#include <iosfwd>
#include <span>

#include "InstructionVisitor.h"
#include "base/CommonInstruction.h"
#include "mir/value/Value.h"

template<typename F>
concept InstructionMatcher = std::is_invocable_r_v<bool, F, const Instruction*>;

class Instruction : public CommonInstruction<BasicBlock> {
    static constexpr auto NO_ID = std::numeric_limits<std::size_t>::max();

public:
    explicit Instruction(std::vector<Value>&& values) noexcept:
        m_values(std::move(values)) {}

    virtual ~Instruction() = default;

    template<InstructionMatcher Matcher>
    bool isa(Matcher&& matcher) const noexcept {
        return matcher(this);
    }

    [[nodiscard]]
    std::span<const Value> operands() const noexcept {
        return m_values;
    }

    void release_instruction_users() const noexcept;

    virtual void visit(Visitor& visitor) = 0;

    virtual void visit(Visitor& visitor) const {
        const_cast<Instruction*>(this)->visit(visitor);
    }

    void print(std::ostream& os) const;

    void update_operand(std::size_t idx, const Value& new_val);

protected:
    friend class ValueInstruction;

    void raw_update_operand(std::size_t idx, const Value& new_val);

    friend class BasicBlock;
    std::vector<Value> m_values;
};