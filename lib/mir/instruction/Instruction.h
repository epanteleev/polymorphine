#pragma once

#include <cstddef>
#include <vector>
#include <iosfwd>
#include <memory>
#include <span>

#include "InstructionVisitor.h"
#include "base/CommonInstruction.h"
#include "mir/value/Value.h"
#include "utility/Error.h"

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

    void print(std::ostream& os) const;

protected:
    friend class BasicBlock;

    BasicBlock* m_owner{};
    std::size_t m_id{NO_ID};
    std::vector<Value> m_values;
};