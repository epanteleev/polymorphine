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

    virtual void visit(Visitor& visitor) = 0;

    virtual void visit(Visitor& visitor) const {
        const_cast<Instruction*>(this)->visit(visitor);
    }

    void print(std::ostream& os) const;

    void replace_operand(const ValueInstruction* old_val, const Value& new_val) {
        for (auto& v : m_values) {
            if (v.is<ValueInstruction*>() && v.get<ValueInstruction*>() == old_val) {
                v = new_val;
            }
        }
    }

protected:
    friend class BasicBlock;

    BasicBlock* m_owner{};
    std::size_t m_id{NO_ID};
    std::vector<Value> m_values;
};